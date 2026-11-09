"""Protect source coverage, identity and honest availability reporting."""
import collections, hashlib, json, pathlib, sys
root=pathlib.Path(__file__).resolve().parents[1]
sys.path.insert(0,str(root/'tools'))
from generate_device_catalog import generate, ORIGINALS
from update_device_catalog import parse
p=json.loads((root/'data/device-catalog.json').read_text(encoding='utf-8'))
assert p['schema']==1 and p['source']=='https://neuraldsp.com/device-list'
entries=p['entries'];assert len(entries)==689 and len(p['section_counts'])==23
assert collections.Counter(e['section'] for e in entries)==p['section_counts']
assert sum(e['announced_only'] for e in entries)==57
assert len({e['id'] for e in entries})==len(entries)
assert all(e['availability']=='reference_only' and e['implementation_type']==0 for e in entries)
assert len(ORIGINALS)==30 and len({t for t,_,_ in ORIGINALS})==30
assert {t for t,_,_ in ORIGINALS}==set(range(1,22))|{23,24,25,26,27,28,29,30,31}
# Shared display names remain distinct by plugin ownership.
compressors=[e for e in entries if e['section']=='Plugin devices' and e['name']=='Compressor']
assert len(compressors)==3 and len({e['metadata']['Required plugin'] for e in compressors})==3
assert any(e['name']=='Brit 2203 87' and e['section']=='Neural Captures V2' for e in entries)
assert any('John Mayer' in e['metadata'].get('Required plugin','') for e in entries)
assert generate(p)==(root/'Source/DeviceCatalogData.h').read_text(encoding='utf-8')
# Importer fixture exercises header aliases, HTML entities and plugin identity.
tables=[]
for i in range(23):
    tables.append(f'<table aria-label="Section {i}"><tr><th>Device Category</th><th>Device Name</th><th>Required plugin</th></tr><tr><td>Reverb</td><td>A &amp; B</td><td>Plugin {i}</td></tr></table>')
fixture=parse(''.join(tables).encode(),'2026-11-02')
assert len(fixture['entries'])==23 and fixture['entries'][0]['name']=='A & B'
assert fixture['source_html_sha256']==hashlib.sha256(''.join(tables).encode()).hexdigest()
print('PASS: all 689 source rows, 23 source sections, 57 announced entries, distinct plugin identities, 30 playable originals, generated data and importer')
