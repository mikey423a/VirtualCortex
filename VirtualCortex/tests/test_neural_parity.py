"""Check Python trainer serialization matches C++ runtime inference, including temporal history."""
import subprocess
import sys
import tempfile
from pathlib import Path
import numpy as np
sys.path.insert(0,str(Path(__file__).resolve().parents[1]/'tools'))
from train_neural import write_model, evaluate

exe=sys.argv[1]
rng=np.random.default_rng(934)
params=[rng.normal(0,0.5,(8,8)),rng.normal(0,0.2,8),rng.normal(0,0.5,8),np.array([0.07])]
# More than 8 samples forces the C++ circular history to wrap multiple times.
x=rng.normal(0,0.3,151).astype(np.float64)
with tempfile.TemporaryDirectory() as td:
    model=Path(td)/'model.vcmlp'; inp=Path(td)/'samples.txt'
    write_model(model,params)
    inp.write_text('\n'.join(format(float(v),'.9g') for v in x)+'\n')
    run=subprocess.run([exe,str(model),str(inp)],capture_output=True,text=True,check=True)
    got=np.fromstring(run.stdout,sep='\n')
    W,b,v,c=params
    # Quantize to the exact decimal representations saved to the files, then float32
    # to match C++ model and input storage.
    x32=np.array([np.float32(float(format(float(a),'.9g'))) for a in x])
    W32=np.array([np.float32(float(format(float(a),'.9g'))) for a in W.flat]).reshape(8,8)
    b32=np.array([np.float32(float(format(float(a),'.9g'))) for a in b])
    v32=np.array([np.float32(float(format(float(a),'.9g'))) for a in v])
    c32=np.float32(float(format(float(c[0]),'.9g')))
    history=np.column_stack([np.pad(x32,(k,0))[:len(x32)] for k in range(8)])
    expected=np.tanh(history.astype(np.float64)@W32.astype(np.float64).T+b32.astype(np.float64))@v32.astype(np.float64)+float(c32)
    assert len(got)==len(x), (len(got),len(x))
    assert np.max(np.abs(got-expected))<2e-6, np.max(np.abs(got-expected))
    # The verification utility must reject bad models and input files.
    bad=Path(td)/'bad.vcmlp';bad.write_text('NAM\n1 2 3\n')
    assert subprocess.run([exe,str(bad),str(inp)],capture_output=True).returncode!=0
    bad_samples=Path(td)/'bad.txt';bad_samples.write_text('0.1\nnot_a_number\n')
    assert subprocess.run([exe,str(model),str(bad_samples)],capture_output=True).returncode!=0
print('C++ inference matches Python serialization and temporal history')
