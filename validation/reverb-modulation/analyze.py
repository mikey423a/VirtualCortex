"""Offline tone probe; intentional FM is included, not an aliasing/audibility test."""
import argparse
from pathlib import Path
import numpy as np

parser = argparse.ArgumentParser()
parser.add_argument("directory", type=Path)
args = parser.parse_args()
for index, depth in enumerate((0, .8, 2)):
    audio = np.fromfile(args.directory / f"mod-{index}.f32", dtype="<f4").reshape(-1, 2).astype(float)
    assert audio.shape == (384000, 2) and np.isfinite(audio).all()
    # Eight seconds contains integer cycles of both carrier and modulation.
    spectrum = np.fft.rfft(audio, axis=0)
    energy = np.sum(abs(spectrum) ** 2, axis=1)
    energy[1:-1] *= 2
    frequency = np.fft.rfftfreq(len(audio), 1 / 48000)
    carrier = energy[frequency == 1000].sum()
    total = energy.sum()
    assert total > 0 and carrier > 0
    outside = energy[abs(frequency - 1000) > 20].sum()
    print(f"depth_ms={depth} rms={np.sqrt(np.mean(audio*audio)):.9g} "
          f"noncarrier_fraction={(total-carrier)/total:.9g} "
          f"outside_980_1020_fraction={outside/total:.9g}")
    # An unmodulated linear tank should retain the pure input tone after warmup.
    if index == 0:
        assert (total-carrier)/total < 1e-8
