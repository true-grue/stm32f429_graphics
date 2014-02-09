import sys
from PIL import Image

Code = '''\
#include <stdint.h>
#include "graphics.h"

static const uint8_t Image_data[%(width)s * %(height)s] = {
%(png)s
};

static const struct bmfont_data Font_data[%(count)s] = {
%(font)s
};

const struct font %(name)s_font = {
  Image_data,
  %(width)s,
  Font_data
};
'''

def get_png_data(name):
  im = Image.open(name)
  pixels = list(im.getdata())
  width, height = im.size
  r = '  '
  i = 1
  for p in pixels:
    if i % 8:
      r += str(p) + ', '
    else:
      if i != len(pixels): 
         r += str(p) + ',\n  '
      else:
         r += str(p)
    i += 1
  return width, height, r

def get_font_data(name):
  font = open(name, 'r').read().split('\n')
  count = int(font[3].split()[1].split('=')[1])
  r = ''
  for i in range(count):
    line = font[4 + i].split()
    r += '  {'
    for j in range(7):
      name, value = line[2 + j].split('=')
      r += value
      if j != 6:
        r += ', '
    if i != count - 1:
      r += '},\n'
    else:
      r += '}'
  return count, r

def main(png_file, fnt_file, font_name):
  width, height, png = get_png_data(png_file)
  count, font = get_font_data(fnt_file)
  f = open(font_name.lower() + '.c', 'w')
  f.write(Code % {
    'width': width,
    'height': height,
    'png': png,
    'count': count,
    'font': font,
    'name': font_name
  })
  f.close()

if len(sys.argv) != 4:
  print('bmfont_to_c png_file fnt_file font_name')
else:
  main(sys.argv[1], sys.argv[2], sys.argv[3])
