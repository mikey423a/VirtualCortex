import sys
from pathlib import Path
import numpy as np
sys.path.insert(0, str(Path(__file__).resolve().parents[1] / 'tools'))
from train_neural import estimate_delay, align_recordings, train, evaluate

rng = np.random.default_rng(72)
x = rng.normal(0, 0.25, 5000)
y = np.concatenate([np.zeros(113), np.tanh(2*x[:-113])])
lag, confidence = estimate_delay(x, y, max_lag=150)
assert abs(lag-113) <= 2, (lag, confidence)
a,b = align_recordings(x,y,lag)
assert len(a) == len(b) == 5000-lag
assert np.mean((np.tanh(2*a[80:])-b[80:])**2) < 0.0002
params, mse = train(a[:4000], b[:4000], steps=100, seed=8)
assert np.isfinite(mse) and np.isfinite(evaluate(params, a[4000:], b[4000:]))
try:
    estimate_delay(np.zeros(1000), np.zeros(1000))
except ValueError:
    pass
else:
    raise AssertionError('Silent capture accepted')
print('Training alignment and validation passed')
