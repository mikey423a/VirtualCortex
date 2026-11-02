#!/usr/bin/env python3
"""Train the ORIGINAL VCMLP1 eight-tap/eight-neuron model from aligned mono WAVs.
Not a .nam trainer and not a Quad Cortex capture utility.
Requires numpy. Both recordings must be sample-aligned and share a sample rate.
"""
import argparse
import struct
import wave
from pathlib import Path
import numpy as np


def read_mono(path):
    with wave.open(str(path), 'rb') as f:
        channels, width, rate, frames = f.getnchannels(), f.getsampwidth(), f.getframerate(), f.getnframes()
        if channels != 1 or width not in (2, 3):
            raise ValueError('Expected mono PCM16 or PCM24 WAV: ' + str(path))
        raw = f.readframes(frames)
    if width == 2:
        values = np.frombuffer(raw, dtype='<i2').astype(np.float64) / 32768.0
    else:
        b = np.frombuffer(raw, dtype=np.uint8).reshape(-1, 3).astype(np.int32)
        v = b[:, 0] | (b[:, 1] << 8) | (b[:, 2] << 16)
        values = ((v ^ 0x800000) - 0x800000).astype(np.float64) / 8388608.0
    return values, rate


def estimate_delay(dry, processed, max_lag=2048):
    """Estimate integer-sample latency by correlating signal envelopes.

    Returns positive lag when the processed signal is delayed. Envelope
    correlation tolerates nonlinear distortion better than raw correlation.
    """
    if max_lag < 0 or len(dry) != len(processed) or len(dry) < 128:
        raise ValueError('Equal-length recordings (>=128 samples) required')
    limit = min(max_lag, len(dry) // 4)
    # Smooth squared amplitude over 64 samples to remove polarity dependence.
    kernel = np.ones(64, dtype=np.float64) / 64
    a = np.convolve(np.asarray(dry, dtype=np.float64)**2, kernel, mode='same')
    b = np.convolve(np.asarray(processed, dtype=np.float64)**2, kernel, mode='same')
    a -= a.mean(); b -= b.mean()
    if np.linalg.norm(a) < 1e-10 or np.linalg.norm(b) < 1e-10:
        raise ValueError('Cannot align silent or constant-energy recordings')
    best_lag, best_score = 0, -np.inf
    for lag in range(-limit, limit + 1):
        if lag >= 0:
            x, y = a[:len(a)-lag], b[lag:]
        else:
            x, y = a[-lag:], b[:len(b)+lag]
        denom = np.linalg.norm(x) * np.linalg.norm(y)
        score = float(np.dot(x, y) / denom) if denom > 1e-12 else -np.inf
        if score > best_score:
            best_score, best_lag = score, lag
    return best_lag, best_score


def align_recordings(dry, processed, lag):
    """Trim edges to align a delayed (or advanced) processed recording."""
    if lag > 0:
        return dry[:-lag], processed[lag:]
    if lag < 0:
        return dry[-lag:], processed[:lag]
    return dry, processed


def evaluate(params, dry, target):
    W, b, v, c = params
    X = np.column_stack([np.pad(dry, (k, 0))[:len(dry)] for k in range(8)])
    predicted = np.tanh(X @ W.T + b) @ v + c[0]
    return float(np.mean((predicted - target)**2))

def train(x, target, steps=500, lr=0.01, seed=0, batch=2048):
    if len(x) != len(target) or len(x) < 128:
        raise ValueError('Aligned recordings of equal length (>=128 samples) required')
    if not np.isfinite(x).all() or not np.isfinite(target).all():
        raise ValueError('Nonfinite training audio')
    rng = np.random.default_rng(seed)
    # Columns: current sample, then seven previous samples, zero-padded at start.
    X = np.column_stack([np.pad(x, (k, 0))[:len(x)] for k in range(8)])
    W = rng.normal(0, 0.3, (8, 8)); b = np.zeros(8)
    v = rng.normal(0, 0.1, 8); c = float(np.mean(target))
    params = [W, b, v, np.array([c])]
    m = [np.zeros_like(p) for p in params]; z = [np.zeros_like(p) for p in params]
    for t in range(1, steps + 1):
        ids = rng.integers(0, len(x), size=min(batch, len(x)))
        inp, goal = X[ids], target[ids]
        h = np.tanh(inp @ W.T + b)
        err = h @ v + params[3][0] - goal
        d = (2.0 / len(ids)) * err
        grad_v = h.T @ d
        dh = (d[:, None] * v) * (1.0 - h*h)
        grads = [dh.T @ inp, dh.sum(axis=0), grad_v, np.array([d.sum()])]
        for i, (p, g) in enumerate(zip(params, grads)):
            np.clip(g, -10.0, 10.0, out=g)
            m[i] = 0.9*m[i] + 0.1*g
            z[i] = 0.999*z[i] + 0.001*g*g
            p -= lr*(m[i]/(1.0-0.9**t))/(np.sqrt(z[i]/(1.0-0.999**t))+1e-8)
    return params, float(np.mean((np.tanh(X @ W.T+b) @ v + params[3][0]-target)**2))


def write_model(path, params):
    W, b, v, c = params
    values = np.concatenate((W.ravel(), b, v, c))
    if len(values) != 81 or not np.isfinite(values).all():
        raise ValueError('Invalid model weights')
    with open(path, 'w', encoding='ascii') as f:
        f.write('VCMLP1\n')
        f.write(' '.join(format(float(a), '.9g') for a in values) + '\n')


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('dry_wav', type=Path)
    parser.add_argument('processed_wav', type=Path)
    parser.add_argument('output_model', type=Path)
    parser.add_argument('--steps', type=int, default=500)
    parser.add_argument('--learning-rate', type=float, default=0.01)
    parser.add_argument('--seed', type=int, default=0)
    parser.add_argument('--auto-align', action='store_true', help='Estimate and remove integer-sample recording latency')
    parser.add_argument('--max-lag', type=int, default=2048, help='Maximum alignment search range in samples')
    args = parser.parse_args()
    if args.steps < 1 or not (0 < args.learning_rate <= 0.1):
        parser.error('steps must be positive and learning rate in (0, 0.1]')
    x, rx = read_mono(args.dry_wav)
    y, ry = read_mono(args.processed_wav)
    if rx != ry:
        parser.error('WAV sample rates must match; resample before training')
    if args.max_lag < 0:
        parser.error('--max-lag must be nonnegative')
    if args.auto_align:
        try:
            lag, confidence = estimate_delay(x, y, args.max_lag)
        except ValueError as exc:
            parser.error(str(exc))
        if confidence < 0.2:
            parser.error(f'Unreliable audio alignment (correlation {confidence:.3f}); align manually')
        x, y = align_recordings(x, y, lag)
        print(f'Estimated recording latency: {lag} samples (envelope correlation {confidence:.3f})')
    # A chronological holdout catches overfitting to the training excerpt.
    split = int(len(x) * 0.9)
    if split < 128 or len(x) - split < 16:
        parser.error('Need at least 160 aligned samples')
    params, mse = train(x[:split], y[:split], steps=args.steps, lr=args.learning_rate, seed=args.seed)
    validation_mse = evaluate(params, x[split:], y[split:])
    write_model(args.output_model, params)
    print(f'Saved {args.output_model} | sample rate {rx} Hz | training MSE {mse:.7f} | holdout MSE {validation_mse:.7f}')
    print('Note: This tiny model cannot reproduce a complex amplifier or long-memory dynamics.')


if __name__ == '__main__':
    main()
