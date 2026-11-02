import sys
import tempfile
import wave
from pathlib import Path
import numpy as np
sys.path.insert(0, str(Path(__file__).resolve().parents[1] / 'tools'))
from train_neural import read_mono, train, write_model


def main():
    rng = np.random.default_rng(8)
    x = rng.uniform(-0.8, 0.8, 4096)
    y = np.tanh(2*x) * 0.7
    with tempfile.TemporaryDirectory() as d:
        path = Path(d) / 'dry.wav'
        with wave.open(str(path), 'wb') as w:
            w.setnchannels(1); w.setsampwidth(2); w.setframerate(48000)
            w.writeframes(np.round(x*32767).astype('<i2').tobytes())
        decoded, rate = read_mono(path)
        assert rate == 48000 and np.max(abs(decoded-x)) < 0.0001
        params, mse = train(decoded, y, steps=700, seed=42)
        assert mse < 0.005, mse
        model = Path(d) / 'test.vcmlp'
        write_model(model, params)
        tokens = model.read_text().split()
        assert tokens[0] == 'VCMLP1' and len(tokens) == 82
        assert all(np.isfinite(float(v)) for v in tokens[1:])
    print('Neural trainer test passed, MSE:', mse)


if __name__ == '__main__':
    main()
