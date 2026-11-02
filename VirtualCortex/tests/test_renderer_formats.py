import struct,math,subprocess,pathlib,sys,tempfile
base=pathlib.Path(tempfile.mkdtemp(prefix='vc-wav-test-'))
renderer=sys.argv[1]
for kind in ['float','extfloat','pcm24']:
    data=b''.join(struct.pack('<f',.2*math.sin(i*.03)) for i in range(1024)) if kind!='pcm24' else b''.join(int(.2*8388607*math.sin(i*.03)).to_bytes(3,'little',signed=True) for i in range(1024))
    tag=3 if kind!='pcm24' else 1;bits=32 if tag==3 else 24
    fmt=struct.pack('<HHIIHH',tag if kind!='extfloat' else 0xfffe,1,48000,48000*(bits//8),bits//8,bits)
    if kind=='extfloat':fmt+=struct.pack('<HHI',22,bits,0)+struct.pack('<I',3)+bytes.fromhex('00001000800000aa00389b71')
    riff=b'WAVEfmt '+struct.pack('<I',len(fmt))+fmt+b'data'+struct.pack('<I',len(data))+data
    inp=base/(kind+'.wav');out=base/(kind+'-out.wav');inp.write_bytes(b'RIFF'+struct.pack('<I',len(riff))+riff)
    result=subprocess.run([renderer,str(inp),str(out)],capture_output=True,text=True)
    assert result.returncode==0,(kind,result.stderr)
    b=out.read_bytes();assert b[:4]==b'RIFF' and struct.unpack_from('<H',b,20)[0]==tag and struct.unpack_from('<H',b,34)[0]==bits
    print(kind,result.stdout.strip(),len(b))
