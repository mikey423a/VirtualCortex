"""Generate the app's UTF-8 factual reference inventory from the checked-in snapshot."""
import argparse, json, pathlib
ORIGINALS=[(1,'Gate','Dynamics'),(2,'Amp','Amplifier'),(3,'Cabinet','Cabinet / IR'),(4,'EQ','EQ'),(5,'Delay','Delay'),(6,'Reverb','Reverb'),(7,'Tremolo','Modulation'),(8,'Compressor','Dynamics'),(9,'Overdrive','Drive'),(10,'Distortion','Drive'),(11,'Chorus','Modulation'),(12,'Flanger','Modulation'),(13,'Phaser','Modulation'),(14,'Wah','Filter'),(15,'Bit-crusher','Drive'),(16,'Vibrato','Modulation'),(17,'Ring modulator','Modulation'),(18,'Gain','Utility'),(19,'Low / High Cut','Filter'),(20,'Tone EQ','EQ'),(21,'Room Reverb','Reverb'),(23,'Stereo Delay','Post-chain'),(24,'Stereo Reverb','Post-chain'),(25,'Four-head Echo','Post-chain'),(26,'Chase Pitch Delay','Post-chain'),(27,'Layer Loom','Post-chain'),(28,'Prism Synth','Input synth'),(29,'Ottocrush','Input effect'),(30,'Grid Stereo Delay','Grid effect'),(31,'Grid Stereo Reverb','Grid effect'),(32,'Grid Four-head Echo','Grid effect'),(33,'Grid Chase Pitch Delay','Grid effect'),(34,'Grid Layer Loom','Grid effect'),(35,'Grid Prism Synth','Grid effect'),(36,'Grid Ottocrush','Grid effect')]
FIELDS=['id','section','category','name','basedOn','added','previous','updated','replaces','plugin']
def generate(data):
    def literal(x):return 'u8'+json.dumps(x,ensure_ascii=True)
    rows=[]
    for t,name,category in ORIGINALS:
        rows.append([f'original-{t}','Virtual Cortex originals',category,name,'','','','','','',False,t])
    for e in data['entries']:
        m=e['metadata']
        rows.append([e['id'],e['section'],e['category'],e['name'],m.get('Based on',''),m.get('Added in CorOS',''),m.get('Previous name',''),m.get('Updated in CorOS',''),m.get('Replaces',''),m.get('Required plugin',''),e['announced_only'],0])
    out=['#pragma once','#include <array>','namespace vc {',
         'struct CatalogRecord { const char* '+', *'.join(FIELDS)+'; bool announced; int implementationType; };',
         'inline constexpr const char* catalogSnapshotDate = '+literal(data['retrieved'])+';',
         'inline constexpr std::array<CatalogRecord, '+str(len(rows))+'> deviceCatalog{{']
    for r in rows:out.append(' {'+', '.join(literal(x) for x in r[:10])+', '+str(r[10]).lower()+', '+str(r[11])+'},')
    out+=['}};','} // namespace vc','']
    return '\n'.join(out)
if __name__=='__main__':
    a=argparse.ArgumentParser();a.add_argument('snapshot',type=pathlib.Path);a.add_argument('header',type=pathlib.Path);a.add_argument('--check',action='store_true');args=a.parse_args()
    result=generate(json.loads(args.snapshot.read_text(encoding='utf-8')))
    if args.check:
        if args.header.read_text(encoding='utf-8')!=result:raise SystemExit('Catalog header is stale')
    else:args.header.write_text(result,encoding='utf-8')
