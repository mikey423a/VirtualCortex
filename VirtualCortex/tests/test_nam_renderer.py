"""Independent numerical checks using synthetic, original NAM fixtures."""
import json, math, pathlib, struct, subprocess, sys, tempfile, wave
exe = pathlib.Path(sys.argv[1]).resolve()

def wav(path, rate, channels, values):
    with wave.open(str(path), 'wb') as f:
        f.setparams((channels, 2, rate, 0, 'NONE', 'not compressed'))
        f.writeframes(struct.pack('<'+'h'*len(values), *values))

def run(*args, ok=True):
    p = subprocess.run([str(exe), *map(str,args)], capture_output=True, text=True, timeout=90)
    assert (p.returncode == 0) == ok, (p.returncode,p.stdout,p.stderr)
    return p

with tempfile.TemporaryDirectory() as folder:
    root = pathlib.Path(folder)
    model = root/'linear.nam'
    config = {'version':'0.5.4','architecture':'Linear','config':{'receptive_field':2,'bias':False},
              'weights':[0.5,0.25], 'sample_rate':48000, 'metadata':{}}
    model.write_text(json.dumps(config))
    # Odd sizes exercise the final partial processing chunk, with independent channels.
    for channels in (1,2):
        frames = 513
        values = []
        for i in range(frames):
            values.extend([int(7000*math.sin(i*.1))] if channels==1 else
                          [int(7000*math.sin(i*.1)), 0])
        source, target = root/'input.wav', root/'output.wav'
        wav(source,48000,channels,values)
        run(model,source,target)
        blob=target.read_bytes()
        assert struct.unpack_from('<HHI',blob,20)==(3,channels,48000)
        samples=struct.unpack_from('<'+'f'*(frames*channels),blob,44)
        for i in range(frames):
            for ch in range(channels):
                now=values[i*channels+ch]/32768
                previous=values[(i-1)*channels+ch]/32768 if i else 0
                expected=.5*now+.25*previous
                assert abs(samples[i*channels+ch]-expected)<2e-6, (i,ch,samples[i*channels+ch],expected)
        before=source.read_bytes()
        run(model,source,source,ok=False)
        assert source.read_bytes()==before
    wav(root/'wrong.wav',44100,1,[1000]*256)
    run(model,root/'wrong.wav',root/'wrong-out.wav',ok=False)
    (root/'invalid.nam').write_text('{}')
    run(root/'invalid.nam',source,target,ok=False)
    run('--probe',model)
print('PASS: actual NAM DSP, independent FIR oracle, mono/stereo isolation, 128-frame boundaries, rate mismatch, malformed model, input preservation')
