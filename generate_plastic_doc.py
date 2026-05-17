from docx import Document
from docx.shared import Pt, RGBColor, Inches
from docx.enum.text import WD_ALIGN_PARAGRAPH
from docx.oxml.ns import qn
from docx.oxml import OxmlElement

doc = Document()

# ── Page margins ──────────────────────────────────────────────────────────────
section = doc.sections[0]
section.top_margin    = Inches(1)
section.bottom_margin = Inches(1)
section.left_margin   = Inches(1.2)
section.right_margin  = Inches(1.2)

# ── Helper: set paragraph shading ────────────────────────────────────────────
def shade_paragraph(para, fill_hex):
    pPr = para._p.get_or_add_pPr()
    shd = OxmlElement('w:shd')
    shd.set(qn('w:val'),   'clear')
    shd.set(qn('w:color'), 'auto')
    shd.set(qn('w:fill'),  fill_hex)
    pPr.append(shd)

# ── Helper: add a horizontal rule ────────────────────────────────────────────
def add_hr(doc):
    p   = doc.add_paragraph()
    pPr = p._p.get_or_add_pPr()
    pBdr = OxmlElement('w:pBdr')
    bottom = OxmlElement('w:bottom')
    bottom.set(qn('w:val'),   'single')
    bottom.set(qn('w:sz'),    '6')
    bottom.set(qn('w:space'), '1')
    bottom.set(qn('w:color'), '2E86AB')
    pBdr.append(bottom)
    pPr.append(pBdr)
    p.paragraph_format.space_after = Pt(4)

# ══════════════════════════════════════════════════════════════════════════════
# TITLE
# ══════════════════════════════════════════════════════════════════════════════
title = doc.add_paragraph()
title.alignment = WD_ALIGN_PARAGRAPH.CENTER
shade_paragraph(title, '1B3A5C')
run = title.add_run('🌍  Enzymes & Chemicals Used in Fighting Plastic Decomposition')
run.bold      = True
run.font.size = Pt(18)
run.font.color.rgb = RGBColor(0xFF, 0xFF, 0xFF)
title.paragraph_format.space_before = Pt(6)
title.paragraph_format.space_after  = Pt(6)

# subtitle
sub = doc.add_paragraph()
sub.alignment = WD_ALIGN_PARAGRAPH.CENTER
sr = sub.add_run('Extracted from: "How the World is Fighting Plastic Decomposition"')
sr.italic          = True
sr.font.size       = Pt(10)
sr.font.color.rgb  = RGBColor(0x88, 0x88, 0x88)
sub.paragraph_format.space_after = Pt(12)

add_hr(doc)

# ══════════════════════════════════════════════════════════════════════════════
# SECTION 1 — ENZYMES
# ══════════════════════════════════════════════════════════════════════════════
s1 = doc.add_paragraph()
shade_paragraph(s1, '0D2137')
s1r = s1.add_run('🧬  Section 1: Enzymes Used in Plastic Decomposition')
s1r.bold           = True
s1r.font.size      = Pt(14)
s1r.font.color.rgb = RGBColor(0x4F, 0xC3, 0xF7)
s1.paragraph_format.space_before = Pt(10)
s1.paragraph_format.space_after  = Pt(6)

# ── Point 1 ──────────────────────────────────────────────────────────────────
p1_head = doc.add_paragraph()
r = p1_head.add_run('1.  Mutated Natural Enzymes (PETase Variants)')
r.bold           = True
r.font.size      = Pt(12)
r.font.color.rgb = RGBColor(0x00, 0xB4, 0x78)
p1_head.paragraph_format.space_after = Pt(2)

bullets_1 = [
    ('Developed by',         'University of Texas at Austin'),
    ('Method',               'Machine learning was used to generate mutations in naturally occurring enzymes'),
    ('Target Plastic',       'PET (Polyethylene Terephthalate) — used in bottles and food packaging'),
    ('Speed',                'Can break down PET plastic in as little as 24 hours'),
    ('Enzyme Name',          'PETase (naturally found in Ideonella sakaiensis bacteria, then mutated for speed)'),
]
for label, val in bullets_1:
    bp = doc.add_paragraph(style='List Bullet')
    lr = bp.add_run(f'{label}: ')
    lr.bold           = True
    lr.font.size      = Pt(11)
    lr.font.color.rgb = RGBColor(0x33, 0x33, 0x33)
    vr = bp.add_run(val)
    vr.font.size      = Pt(11)
    vr.font.color.rgb = RGBColor(0x22, 0x22, 0x22)
    bp.paragraph_format.space_after = Pt(2)

doc.add_paragraph().paragraph_format.space_after = Pt(4)

# ── Point 2 ──────────────────────────────────────────────────────────────────
p2_head = doc.add_paragraph()
r = p2_head.add_run('2.  Engineered / Designed Enzymes')
r.bold           = True
r.font.size      = Pt(12)
r.font.color.rgb = RGBColor(0x00, 0xB4, 0x78)
p2_head.paragraph_format.space_after = Pt(2)

bullets_2 = [
    ('Developed by',   'University of Toulouse (France), University of Portsmouth (UK), US Dept. of Energy'),
    ('Method',         'Specially engineered enzymes designed to target and break chemical bonds in plastic polymers'),
    ('Target Plastic', 'PET and other synthetic polymers'),
    ('Enzyme Type',    'FAST-PETase, LCC (Leaf-branch Compost Cutinase) — a thermostable enzyme variant'),
]
for label, val in bullets_2:
    bp = doc.add_paragraph(style='List Bullet')
    lr = bp.add_run(f'{label}: ')
    lr.bold           = True
    lr.font.size      = Pt(11)
    lr.font.color.rgb = RGBColor(0x33, 0x33, 0x33)
    vr = bp.add_run(val)
    vr.font.size      = Pt(11)
    vr.font.color.rgb = RGBColor(0x22, 0x22, 0x22)
    bp.paragraph_format.space_after = Pt(2)

doc.add_paragraph().paragraph_format.space_after = Pt(4)

# ── Point 3 ──────────────────────────────────────────────────────────────────
p3_head = doc.add_paragraph()
r = p3_head.add_run('3.  Microbial / Bacterial Enzymes (Natural)')
r.bold           = True
r.font.size      = Pt(12)
r.font.color.rgb = RGBColor(0x00, 0xB4, 0x78)
p3_head.paragraph_format.space_after = Pt(2)

bullets_3 = [
    ('Source',         'Naturally occurring microbes found in soil, water, and compost'),
    ('Method',         'Microbial activity + enzymatic degradation breaks polymer chains'),
    ('Environments',   'Industrial composting facilities and anaerobic digestion plants'),
    ('Used For',       'Biodegradable plastics (PLA, PHA, PBAT) decomposition'),
    ('Key Enzymes',    'Lipases, Cutinases, Proteases — secreted by soil bacteria & fungi'),
]
for label, val in bullets_3:
    bp = doc.add_paragraph(style='List Bullet')
    lr = bp.add_run(f'{label}: ')
    lr.bold           = True
    lr.font.size      = Pt(11)
    lr.font.color.rgb = RGBColor(0x33, 0x33, 0x33)
    vr = bp.add_run(val)
    vr.font.size      = Pt(11)
    vr.font.color.rgb = RGBColor(0x22, 0x22, 0x22)
    bp.paragraph_format.space_after = Pt(2)

add_hr(doc)

# ══════════════════════════════════════════════════════════════════════════════
# SECTION 2 — CHEMICALS
# ══════════════════════════════════════════════════════════════════════════════
s2 = doc.add_paragraph()
shade_paragraph(s2, '0D2137')
s2r = s2.add_run('⚗️  Section 2: Chemical Processes Used')
s2r.bold           = True
s2r.font.size      = Pt(14)
s2r.font.color.rgb = RGBColor(0xFF, 0xB7, 0x40)
s2.paragraph_format.space_before = Pt(10)
s2.paragraph_format.space_after  = Pt(6)

# ── Point 4 ──────────────────────────────────────────────────────────────────
p4_head = doc.add_paragraph()
r = p4_head.add_run('4.  Chemical Recycling (Pyrolysis & Solvolysis)')
r.bold           = True
r.font.size      = Pt(12)
r.font.color.rgb = RGBColor(0xFF, 0x8C, 0x00)
p4_head.paragraph_format.space_after = Pt(2)

bullets_4 = [
    ('Used In',          'Europe (leading adopters)'),
    ('Pyrolysis',        'Heating plastic to ~400–600°C without oxygen — converts it into oil, gas, and char'),
    ('Solvolysis',       'Using solvents (glycolysis, hydrolysis, methanolysis) to chemically dissolve polymer chains'),
    ('Key Chemicals',    'Ethylene glycol (glycolysis), water + heat (hydrolysis), methanol (methanolysis)'),
    ('Advantage',        'Handles mixed and contaminated polymers — no need to clean or sort plastic'),
    ('Output',           'Reformed monomers that can be reused to make new plastic or fuel'),
]
for label, val in bullets_4:
    bp = doc.add_paragraph(style='List Bullet')
    lr = bp.add_run(f'{label}: ')
    lr.bold           = True
    lr.font.size      = Pt(11)
    lr.font.color.rgb = RGBColor(0x33, 0x33, 0x33)
    vr = bp.add_run(val)
    vr.font.size      = Pt(11)
    vr.font.color.rgb = RGBColor(0x22, 0x22, 0x22)
    bp.paragraph_format.space_after = Pt(2)

add_hr(doc)

# ══════════════════════════════════════════════════════════════════════════════
# SECTION 3 — BIOLOGICAL ENVIRONMENTS
# ══════════════════════════════════════════════════════════════════════════════
s3 = doc.add_paragraph()
shade_paragraph(s3, '0D2137')
s3r = s3.add_run('🌿  Section 3: Biological Breakdown Environments')
s3r.bold           = True
s3r.font.size      = Pt(14)
s3r.font.color.rgb = RGBColor(0x81, 0xC7, 0x84)
s3.paragraph_format.space_before = Pt(10)
s3.paragraph_format.space_after  = Pt(6)

bio_points = [
    ('5.  Anaerobic Digestion',
     [('Process',   'Microbes break down biodegradable plastics in oxygen-free environments'),
      ('Output',    'Biogas (methane) + digestate (used as fertilizer)'),
      ('Plastics',  'PHA, PLA in certified compostable form')]),
    ('6.  Industrial Composting',
     [('Process',   'High-temperature compost (55–70°C) accelerates enzymatic breakdown'),
      ('Duration',  '90–180 days for certified compostable plastics'),
      ('Plastics',  'PLA, PBAT, starch-based blends')]),
]
for head, blist in bio_points:
    ph = doc.add_paragraph()
    r  = ph.add_run(head)
    r.bold           = True
    r.font.size      = Pt(12)
    r.font.color.rgb = RGBColor(0x2E, 0x7D, 0x32)
    ph.paragraph_format.space_after = Pt(2)

    for label, val in blist:
        bp = doc.add_paragraph(style='List Bullet')
        lr = bp.add_run(f'{label}: ')
        lr.bold           = True
        lr.font.size      = Pt(11)
        lr.font.color.rgb = RGBColor(0x33, 0x33, 0x33)
        vr = bp.add_run(val)
        vr.font.size      = Pt(11)
        vr.font.color.rgb = RGBColor(0x22, 0x22, 0x22)
        bp.paragraph_format.space_after = Pt(2)
    doc.add_paragraph().paragraph_format.space_after = Pt(4)

add_hr(doc)

# ══════════════════════════════════════════════════════════════════════════════
# QUICK REFERENCE TABLE
# ══════════════════════════════════════════════════════════════════════════════
th = doc.add_paragraph()
tr = th.add_run('📋  Quick Reference Table')
tr.bold           = True
tr.font.size      = Pt(13)
tr.font.color.rgb = RGBColor(0x1B, 0x3A, 0x5C)
th.paragraph_format.space_before = Pt(6)
th.paragraph_format.space_after  = Pt(6)

table = doc.add_table(rows=1, cols=4)
table.style = 'Table Grid'

# Header row
hdr_cells = table.rows[0].cells
headers   = ['#', 'Name', 'Type', 'Target Plastic']
for i, h in enumerate(headers):
    p  = hdr_cells[i].paragraphs[0]
    p.alignment = WD_ALIGN_PARAGRAPH.CENTER
    r  = p.add_run(h)
    r.bold           = True
    r.font.size      = Pt(11)
    r.font.color.rgb = RGBColor(0xFF, 0xFF, 0xFF)
    tc     = hdr_cells[i]._tc
    tcPr   = tc.get_or_add_tcPr()
    shd    = OxmlElement('w:shd')
    shd.set(qn('w:val'),   'clear')
    shd.set(qn('w:color'), 'auto')
    shd.set(qn('w:fill'),  '1B3A5C')
    tcPr.append(shd)

rows_data = [
    ('1', 'PETase (mutated)',           'Enzyme',          'PET plastic'),
    ('2', 'FAST-PETase / LCC',          'Enzyme',          'PET plastic'),
    ('3', 'Lipases, Cutinases',         'Microbial Enzyme', 'PLA, PHA, PBAT'),
    ('4', 'Pyrolysis',                  'Chemical Process', 'Mixed / contaminated'),
    ('5', 'Glycolysis (ethylene glycol)','Chemical Process','PET'),
    ('6', 'Hydrolysis (water + heat)',  'Chemical Process', 'PET, PA'),
    ('7', 'Methanolysis (methanol)',     'Chemical Process', 'PET'),
    ('8', 'Anaerobic Digestion',        'Biological',       'PHA, PLA'),
    ('9', 'Industrial Composting',      'Biological',       'PLA, PBAT'),
]
fill_colors = ['F0F8FF', 'FFFFFF']
for idx, (num, name, typ, target) in enumerate(rows_data):
    row_cells = table.add_row().cells
    fill      = fill_colors[idx % 2]
    for ci, text in enumerate([num, name, typ, target]):
        p  = row_cells[ci].paragraphs[0]
        p.alignment = WD_ALIGN_PARAGRAPH.CENTER if ci == 0 else WD_ALIGN_PARAGRAPH.LEFT
        r  = p.add_run(text)
        r.font.size      = Pt(10)
        r.font.color.rgb = RGBColor(0x1A, 0x1A, 0x1A)
        tc   = row_cells[ci]._tc
        tcPr = tc.get_or_add_tcPr()
        shd  = OxmlElement('w:shd')
        shd.set(qn('w:val'),   'clear')
        shd.set(qn('w:color'), 'auto')
        shd.set(qn('w:fill'),  fill)
        tcPr.append(shd)

# ══════════════════════════════════════════════════════════════════════════════
# FOOTER NOTE
# ══════════════════════════════════════════════════════════════════════════════
doc.add_paragraph()
fn = doc.add_paragraph()
fn.alignment = WD_ALIGN_PARAGRAPH.CENTER
shade_paragraph(fn, 'EAF4FB')
fnr = fn.add_run(
    'Sources: University of Texas at Austin · University of Toulouse · University of Portsmouth · '
    'US Dept. of Energy · World Economic Forum · PubMed Central · MDPI'
)
fnr.italic          = True
fnr.font.size       = Pt(9)
fnr.font.color.rgb  = RGBColor(0x55, 0x55, 0x55)
fn.paragraph_format.space_before = Pt(10)
fn.paragraph_format.space_after  = Pt(6)

# ── Save ──────────────────────────────────────────────────────────────────────
output_path = r'c:\Users\suhas b.s\remote robo\T-01-The-Explorer\Plastic_Decomposition_Enzymes_Chemicals.docx'
doc.save(output_path)
print(f'[OK] Document saved to: {output_path}')
