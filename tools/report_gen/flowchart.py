"""Мінімальний генератор блок-схем (символи ДСТУ ISO 5807) у SVG.

Структура алгоритму задається вкладеними кортежами:
  ('term', 'Початок')            - термінатор
  ('proc', 'S = a*b/2')          - процес
  ('io',   'Ввести a, c')        - дані (паралелограм)
  ('sub',  'calc_y(x)')          - визначений процес
  ('if', 'умова', [then...], [else...])         - розгалуження (так - праворуч)
  ('loop', 'i = 1..n', [body...])               - цикл із символом «Підготовка»
  ('while', 'умова', [body...])                 - цикл з передумовою (рішення)
  ('dowhile', [body...], 'умова')               - цикл з післяумовою
Лінії потоку вниз і вправо - без стрілок, вгору і вліво - зі стрілками.
"""
import html

W = 220          # ширина символу
GAP = 26         # вертикальний проміжок між символами
LINE_H = 16      # висота рядка тексту
FONT = 13
CH_W = 6.1       # середня ширина символу шрифту


def wrap(text, width_chars):
    out = []
    for para in text.split('\n'):
        words, cur = para.split(' '), ''
        for w in words:
            if len(cur) + len(w) + (1 if cur else 0) <= width_chars:
                cur = (cur + ' ' + w) if cur else w
            else:
                if cur:
                    out.append(cur)
                cur = w
        out.append(cur)
    return out


class Block:
    def __init__(self, left, right, height, render):
        self.left, self.right, self.height, self.render = left, right, height, render


def text_svg(cx, cy, lines):
    n = len(lines)
    y0 = cy - (n - 1) * LINE_H / 2 + 4.5
    return ''.join(
        f'<text x="{cx:.1f}" y="{y0 + i * LINE_H:.1f}" text-anchor="middle">{html.escape(l)}</text>'
        for i, l in enumerate(lines))


def line(x1, y1, x2, y2, arrow=False):
    a = ' marker-end="url(#arr)"' if arrow else ''
    return f'<line x1="{x1:.1f}" y1="{y1:.1f}" x2="{x2:.1f}" y2="{y2:.1f}"{a}/>'


def label(x, y, t, anchor='start'):
    return f'<text x="{x:.1f}" y="{y:.1f}" class="lbl" text-anchor="{anchor}">{t}</text>'


def node(kind, text):
    chars = int((W - (44 if kind in ('io', 'dec', 'prep') else 18)) / CH_W)
    lines = wrap(text, chars)
    h = max(40, len(lines) * LINE_H + 16)
    if kind == 'dec':
        h = max(64, len(lines) * LINE_H + 40)
    w = W

    def render(cx, y):
        x0 = cx - w / 2
        cy = y + h / 2
        if kind == 'term':
            s = f'<rect x="{x0:.1f}" y="{y:.1f}" width="{w}" height="{h}" rx="{h/2:.1f}"/>'
        elif kind == 'proc':
            s = f'<rect x="{x0:.1f}" y="{y:.1f}" width="{w}" height="{h}"/>'
        elif kind == 'sub':
            s = (f'<rect x="{x0:.1f}" y="{y:.1f}" width="{w}" height="{h}"/>'
                 + line(x0 + 10, y, x0 + 10, y + h) + line(x0 + w - 10, y, x0 + w - 10, y + h))
        elif kind == 'io':
            d = 16
            s = (f'<polygon points="{x0+d:.1f},{y:.1f} {x0+w:.1f},{y:.1f} '
                 f'{x0+w-d:.1f},{y+h:.1f} {x0:.1f},{y+h:.1f}"/>')
        elif kind == 'dec':
            s = (f'<polygon points="{cx:.1f},{y:.1f} {x0+w:.1f},{cy:.1f} '
                 f'{cx:.1f},{y+h:.1f} {x0:.1f},{cy:.1f}"/>')
        elif kind == 'prep':
            d = 18
            s = (f'<polygon points="{x0+d:.1f},{y:.1f} {x0+w-d:.1f},{y:.1f} {x0+w:.1f},{cy:.1f} '
                 f'{x0+w-d:.1f},{y+h:.1f} {x0+d:.1f},{y+h:.1f} {x0:.1f},{cy:.1f}"/>')
        return s + text_svg(cx, cy, lines)
    return Block(w / 2, w / 2, h, render), h


def build(item):
    kind = item[0]
    if kind in ('term', 'proc', 'io', 'sub'):
        return node(kind, item[1])[0]
    if kind == 'if':
        return build_if(item[1], item[2], item[3] if len(item) > 3 else [])
    if kind == 'loop':
        return build_loop('prep', item[1], item[2])
    if kind == 'while':
        return build_loop('dec', item[1], item[2])
    if kind == 'dowhile':
        return build_dowhile(item[1], item[2])
    raise ValueError(kind)


def seq(items):
    blocks = [build(i) for i in items]
    if not blocks:
        return Block(0, 0, 0, lambda cx, y: '')
    left = max(b.left for b in blocks)
    right = max(b.right for b in blocks)
    height = sum(b.height for b in blocks) + GAP * (len(blocks) - 1)

    def render(cx, y):
        s = ''
        for i, b in enumerate(blocks):
            s += b.render(cx, y)
            y += b.height
            if i < len(blocks) - 1:
                s += line(cx, y, cx, y + GAP)
                y += GAP
        return s
    return Block(left, right, height, render)


def build_if(cond, then_items, else_items):
    d, dh = node('dec', cond)
    t = seq(then_items)
    e = seq(else_items)
    # гілка «так» праворуч від ромба і від усього, що займає гілка «ні» (вкладені розгалуження)
    tx = max(d.right, e.right) + 30 + t.left
    body_h = max(t.height, e.height)
    height = dh + GAP + body_h + GAP
    left = max(d.left, e.left)
    right = tx + max(t.right, 20)
    # гілка, що закінчується термінатором (повернення), не з'єднується з точкою злиття
    t_ends = bool(then_items) and then_items[-1][0] == 'term'
    e_ends = bool(else_items) and else_items[-1][0] == 'term'

    def render(cx, y):
        s = d.render(cx, y)
        cy = y + dh / 2
        top = y + dh + GAP
        bottom = top + body_h + GAP / 2
        # «так» - праворуч
        s += line(cx + d.right, cy, cx + tx, cy) + label(cx + d.right + 4, cy - 5, 'так')
        if t.height:
            s += line(cx + tx, cy, cx + tx, top) + t.render(cx + tx, top)
            if not t_ends:
                s += line(cx + tx, top + t.height, cx + tx, bottom)
        else:
            s += line(cx + tx, cy, cx + tx, bottom)
        if not t_ends:
            s += line(cx + tx, bottom, cx, bottom, arrow=True)
        # «ні» - вниз
        s += label(cx + 6, y + dh + 12, 'ні')
        if e.height:
            s += line(cx, y + dh, cx, top) + e.render(cx, top)
            if not e_ends:
                s += line(cx, top + e.height, cx, bottom)
        else:
            s += line(cx, y + dh, cx, bottom)
        if not (e_ends and t_ends):              # потік продовжується після точки злиття
            s += line(cx, bottom, cx, y + height)
        return s
    return Block(left, right, height, render)


def build_loop(kind, header, body_items):
    p, ph = node(kind, header)
    b = seq(body_items)
    lx = max(p.left, b.left) + 24            # відступ лінії повернення ліворуч
    rx = max(p.right, b.right) + 24          # відступ лінії виходу праворуч
    height = GAP / 2 + ph + GAP + b.height + GAP + GAP
    exit_label = 'ні' if kind == 'dec' else ''

    def render(cx, y):
        s = line(cx, y, cx, y + GAP / 2)
        py = y + GAP / 2
        s += p.render(cx, py)
        cy = py + ph / 2
        by = py + ph + GAP
        s += line(cx, py + ph, cx, by)
        if kind == 'dec':
            s += label(cx + 6, py + ph + 12, 'так')
        s += b.render(cx, by)
        end = by + b.height
        back = end + GAP / 2
        s += line(cx, end, cx, back)
        s += line(cx, back, cx - lx, back, arrow=True)
        s += line(cx - lx, back, cx - lx, y + GAP / 4, arrow=True)
        s += line(cx - lx, y + GAP / 4, cx, y + GAP / 4, arrow=False)
        # вихід з циклу
        s += line(cx + p.right, cy, cx + rx, cy)
        if exit_label:
            s += label(cx + p.right + 4, cy - 5, exit_label)
        out_y = y + height - GAP / 2
        s += line(cx + rx, cy, cx + rx, out_y)
        s += line(cx + rx, out_y, cx, out_y, arrow=True)
        s += line(cx, out_y, cx, y + height)
        return s
    return Block(max(p.left, b.left) + lx - max(p.left, b.left) + 6,
                 rx + 6, height, render)


def build_dowhile(body_items, cond):
    b = seq(body_items)
    d, dh = node('dec', cond)
    lx = max(d.left, b.left) + 24
    height = GAP / 2 + b.height + GAP + dh + GAP / 2

    def render(cx, y):
        s = line(cx, y, cx, y + GAP / 2)
        by = y + GAP / 2
        s += b.render(cx, by)
        dy = by + b.height + GAP
        s += line(cx, by + b.height, cx, dy)
        s += d.render(cx, dy)
        cy = dy + dh / 2
        s += line(cx - d.left, cy, cx - lx, cy, arrow=False) + label(cx - d.left - 4, cy - 5, 'так', 'end')
        s += line(cx - lx, cy, cx - lx, y + GAP / 4, arrow=True)
        s += line(cx - lx, y + GAP / 4, cx, y + GAP / 4)
        s += label(cx + 6, dy + dh + 12, 'ні')
        s += line(cx, dy + dh, cx, y + height)
        return s
    return Block(lx + 6, max(d.right, b.right), height, render)


def svg(items, title=None, scale=1.0, max_w=650, max_h=860):
    blk = seq(items)
    pad = 14
    w = blk.left + blk.right + pad * 2
    h = blk.height + pad * 2
    scale = min(scale, max_w / w, max_h / h)   # вміщуємо рисунок на сторінку
    body = blk.render(pad + blk.left, pad)
    style = ('<style>rect,polygon{fill:#fff;stroke:#000;stroke-width:1.3}'
             'line{stroke:#000;stroke-width:1.3}'
             'text{font-family:"Times New Roman",serif;font-size:%dpx;fill:#000}'
             '.lbl{font-size:12px;font-style:italic}</style>' % FONT)
    defs = ('<defs><marker id="arr" viewBox="0 0 10 10" refX="10" refY="5" markerWidth="7" '
            'markerHeight="7" orient="auto"><path d="M0,0 L10,5 L0,10 z" fill="#000"/></marker></defs>')
    return (f'<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 {w:.0f} {h:.0f}" '
            f'width="{w*scale:.0f}" height="{h*scale:.0f}">{style}{defs}{body}</svg>')


if __name__ == '__main__':
    demo = [('term', 'Початок'), ('io', 'Ввести n'), ('proc', 's = 0'),
            ('loop', 'k = 1, n', [('proc', 's = sqrt(2 + s)')]),
            ('if', 'n > 5', [('io', 'Вивести "велике"')], [('io', 'Вивести "мале"')]),
            ('while', 'i < n', [('proc', 'i = i + 1')]),
            ('dowhile', [('proc', 'k++')], 'k < n'),
            ('io', 'Вивести s'), ('term', 'Кінець')]
    open('demo.svg', 'w', encoding='utf8').write(svg(demo))
