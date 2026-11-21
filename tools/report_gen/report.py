"""Спільні засоби формування звітів: HTML за вимогами ДСТУ 3008 -> PDF (Edge)."""
import html
import os
import re
import subprocess

from flowchart import svg as fc_svg

ROOT = 'D:/Downloads/alg/vykonannya'
STUDENT = 'Цибульський Андрій Іванович'
STUDENT_SHORT = 'Цибульський А. І.'
GROUP = 'АВ-11'
VARIANT = 19
DISCIPLINE = 'Алгоритмізація, програмування та тестування програмного забезпечення'
EDGE = r'C:\Program Files (x86)\Microsoft\Edge\Application\msedge.exe'

CSS = """
@page { size: A4; margin: 20mm 10mm 20mm 25mm; }
html { font-family: 'Times New Roman', Times, serif; font-size: 14pt; line-height: 1.5; color: #000; }
body { margin: 0; }
p { text-align: justify; text-indent: 1.25cm; margin: 0; }
p.ni { text-indent: 0; }
h1 { font-size: 14pt; text-transform: uppercase; text-align: center; margin: 0 0 12pt; font-weight: bold; }
h2 { font-size: 14pt; text-indent: 1.25cm; margin: 14pt 0 6pt; font-weight: bold; page-break-after: avoid; }
h3 { font-size: 14pt; text-indent: 1.25cm; margin: 10pt 0 4pt; font-weight: bold; font-style: italic; page-break-after: avoid; }
.newpage { page-break-before: always; }
ul, ol { margin: 0 0 0 1.25cm; padding-left: 0.6cm; }
li { text-align: justify; }
pre.code { font-family: 'Courier New', monospace; font-size: 10pt; line-height: 1.15; border: 0.6pt solid #888;
           padding: 4pt 6pt; margin: 4pt 0 8pt; white-space: pre-wrap; word-break: break-all; background: #fafafa; }
pre.console { font-family: Consolas, 'Courier New', monospace; font-size: 8.5pt; line-height: 1.15; background: #0c0c0c;
              color: #e8e8e8; padding: 6pt 8pt; margin: 4pt 0 8pt; white-space: pre-wrap; word-break: break-all;
              border-radius: 3pt; }
pre.console .in { color: #7fd17f; }
pre.console .cmd { color: #f2d15b; }
.fname { font-family: 'Courier New', monospace; font-size: 11pt; font-weight: bold; margin-top: 8pt; page-break-after: avoid; }
.fig { text-align: center; margin: 8pt 0; page-break-inside: avoid; }
.fig svg, .fig img { max-width: 100%; height: auto; }
.caption { text-align: center; text-indent: 0; margin: 2pt 0 10pt; }
.tcap { text-indent: 1.25cm; text-align: left; margin: 8pt 0 2pt; page-break-after: avoid; }
table { border-collapse: collapse; width: 100%; margin: 2pt 0 10pt; font-size: 12pt; line-height: 1.2; }
td, th { border: 0.7pt solid #000; padding: 2pt 4pt; vertical-align: top; text-align: left; }
th { font-weight: bold; text-align: center; }
.formula { text-align: center; text-indent: 0; margin: 6pt 0; }
.title { text-align: center; text-indent: 0; line-height: 1.5; height: 250mm; position: relative; }
.title p { text-align: center; text-indent: 0; }
.title .who { position: absolute; right: 0; top: 150mm; text-align: left; width: 95mm; }
.title .who p { text-align: left; }
.title .city { position: absolute; bottom: 0; width: 100%; }
code { font-family: 'Courier New', monospace; font-size: 12pt; }
"""


def esc(s):
    return html.escape(s, quote=False)


def read(path):
    with open(os.path.join(ROOT, path), encoding='utf8') as f:
        return f.read()


def title_page(kind_line, topic, number=None, teacher='ст. викл. Смолянкін О. О.'):
    num = f' №{number}' if number else ''
    return f"""
<div class="title">
<p>МІНІСТЕРСТВО ОСВІТИ І НАУКИ УКРАЇНИ</p>
<p>ЛУЦЬКИЙ НАЦІОНАЛЬНИЙ ТЕХНІЧНИЙ УНІВЕРСИТЕТ</p>
<p>Кафедра автоматизації та безпілотних систем</p>
<p style="margin-top:55mm"><b>ЗВІТ</b></p>
<p>{kind_line}{num}</p>
<p>з дисципліни «{DISCIPLINE}»</p>
<p>на тему: «{esc(topic)}»</p>
<p>Варіант {VARIANT}</p>
<div class="who">
<p>Виконав: студент групи {GROUP}</p>
<p>{STUDENT}</p>
<p style="margin-top:10mm">Перевірив: {teacher}</p>
</div>
<div class="city"><p>Луцьк – 2026</p></div>
</div>
"""


def h(text, level=2, newpage=False):
    cls = ' class="newpage"' if newpage else ''
    return f'<h{level}{cls}>{text}</h{level}>'


def p(*paras):
    return ''.join(f'<p>{x}</p>' for x in paras)


def ul(items, ordered=False):
    tag = 'ol' if ordered else 'ul'
    return f'<{tag}>' + ''.join(f'<li>{x}</li>' for x in items) + f'</{tag}>'


def code_file(path, title=None):
    name = title or os.path.basename(path)
    return f'<div class="fname">Файл {esc(name)}</div><pre class="code">{esc(read(path).rstrip())}</pre>'


def code_text(text):
    return f'<pre class="code">{esc(text.rstrip())}</pre>'


def console_text(text, max_lines=None):
    lines = text.rstrip('\n').split('\n')
    if max_lines and len(lines) > max_lines:
        lines = lines[:max_lines] + ['... (вивід скорочено)']
    out = []
    for l in lines:
        e = esc(l)
        if l.startswith('[ввід] '):
            e = f'<span class="in">{esc(l[7:])}</span>'
        elif l.startswith('> ') or l.startswith('$ '):
            e = f'<span class="cmd">{e}</span>'
        out.append(e)
    return '<pre class="console">' + '\n'.join(out) + '</pre>'


def console_file(path, max_lines=None):
    return console_text(read(path), max_lines)


class Counter:
    """Нумерація рисунків і таблиць у межах розділу."""
    def __init__(self):
        self.section = 0
        self.fig = 0
        self.tab = 0

    def set_section(self, n):
        self.section, self.fig, self.tab = n, 0, 0

    def fig_no(self):
        self.fig += 1
        return f'{self.section}.{self.fig}' if self.section else str(self.fig)

    def tab_no(self):
        self.tab += 1
        return f'{self.section}.{self.tab}' if self.section else str(self.tab)


C = Counter()


def figure(svg_or_html, caption):
    n = C.fig_no()
    return f'<div class="fig">{svg_or_html}</div><p class="caption">Рисунок {n} – {caption}</p>'


def flow(items, caption, scale=1.0, **kw):
    return figure(fc_svg(items, scale=scale, **kw), caption)


def table(header, rows, caption, widths=None):
    n = C.tab_no()
    cols = ''
    if widths:
        cols = '<colgroup>' + ''.join(f'<col style="width:{w}">' for w in widths) + '</colgroup>'
    th = '<tr>' + ''.join(f'<th>{x}</th>' for x in header) + '</tr>' if header else ''
    trs = ''.join('<tr>' + ''.join(f'<td>{c}</td>' for c in r) + '</tr>' for r in rows)
    return f'<p class="tcap">Таблиця {n} – {caption}</p><table>{cols}{th}{trs}</table>'


def qa(questions):
    """Відповіді на контрольні питання: список пар (питання, відповідь)."""
    out = ''
    for i, (q, a) in enumerate(questions, 1):
        out += f'<p><b>{i}. {esc(q)}</b> {a}</p>'
    return out


def build_pdf(body, out_pdf, title):
    html_doc = (f'<!doctype html><html lang="uk"><head><meta charset="utf-8"><title>{esc(title)}</title>'
                f'<style>{CSS}</style></head><body>{body}</body></html>')
    tmp = os.path.join(os.path.dirname(os.path.abspath(__file__)), 'html')
    os.makedirs(tmp, exist_ok=True)
    html_path = os.path.join(tmp, os.path.splitext(os.path.basename(out_pdf))[0] + '.html')
    os.makedirs(os.path.dirname(out_pdf), exist_ok=True)
    with open(html_path, 'w', encoding='utf8') as f:
        f.write(html_doc)
    url = 'file:///' + html_path.replace('\\', '/')
    subprocess.run([EDGE, '--headless=new', '--disable-gpu', '--no-pdf-header-footer',
                    f'--print-to-pdf={out_pdf}', url],
                   stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL, timeout=180)
    return html_path
