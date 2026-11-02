"""Import factual device-table metadata from a saved official device-list HTML page."""
import argparse, collections, hashlib, html.parser, json, pathlib, re

class Tables(html.parser.HTMLParser):
    def __init__(self):
        super().__init__(convert_charrefs=True)
        self.tables=[];self.table=None;self.row=None;self.cell=None
    def handle_starttag(self,tag,attrs):
        attrs=dict(attrs)
        if tag=='table':
            if self.table is not None: raise ValueError('Nested table')
            self.table={'section':attrs.get('aria-label',''),'headers':[],'rows':[]}
        elif self.table is not None:
            if tag=='tr': self.row=[]
            elif tag in ('td','th'):self.cell={'header':tag=='th','text':[]}
            elif tag=='br' and self.cell is not None:self.cell['text'].append(' ')
    def handle_data(self,data):
        if self.cell is not None:self.cell['text'].append(data)
    def handle_endtag(self,tag):
        if tag in ('td','th') and self.cell is not None:
            text=' '.join(''.join(self.cell['text']).split())
            if self.cell['header']:text=re.sub(r'\s*i$','',text)
            self.row.append(text);self.cell=None
        elif tag=='tr' and self.row is not None:
            if self.row:
                if not self.table['headers']:self.table['headers']=self.row
                else:self.table['rows'].append(self.row)
            self.row=None
        elif tag=='table' and self.table is not None:
            self.tables.append(self.table);self.table=None

def parse(raw,date):
    p=Tables();p.feed(raw.decode('utf-8'))
    if len(p.tables)!=23:raise ValueError('Expected all 23 source tables, review website structure')
    entries=[]
    for table in p.tables:
        aliases={'device name':'Name','device category':'Device category','based on':'Based on'}
        table['headers']=[aliases.get(h.lower(),h) for h in table['headers']]
        section=table['section']
        if not section or 'Name' not in table['headers']:raise ValueError(('Missing section/name',table['headers']))
        for row in table['rows']:
            if len(row)!=len(table['headers']):raise ValueError(('Table column mismatch',section,row))
            fields=dict(zip(table['headers'],row));name=fields.pop('Name')
            if not name:raise ValueError('Empty device name')
            category=fields.get('Device category',section)
            key=section+'\0'+category+'\0'+name+'\0'+fields.get('Required plugin','')
            entries.append({'id':hashlib.sha256(key.encode()).hexdigest()[:16],
                            'section':section,'category':category,'name':name,'metadata':fields,
                            'announced_only':'Announced' in section,
                            'availability':'reference_only','implementation_type':0})
    if len({e['id'] for e in entries})!=len(entries):raise ValueError('Duplicate source identities')
    return {'schema':1,'source':'https://neuraldsp.com/device-list','retrieved':date,
            'source_html_sha256':hashlib.sha256(raw).hexdigest(),
            'section_counts':dict(collections.Counter(e['section'] for e in entries)), 'entries':entries}

if __name__=='__main__':
    a=argparse.ArgumentParser();a.add_argument('html',type=pathlib.Path);a.add_argument('output',type=pathlib.Path);a.add_argument('--date',required=True)
    args=a.parse_args();data=parse(args.html.read_bytes(),args.date)
    args.output.write_text(json.dumps(data,ensure_ascii=False,indent=2)+'\n',encoding='utf-8')
    print(json.dumps({'total':len(data['entries']),'sections':data['section_counts']},indent=2))
