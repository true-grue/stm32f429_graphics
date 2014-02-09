import sys
from PIL import Image

Code = '''\
#include <stdint.h>
#include "graphics.h"

static const uint32_t Image_data[%(width)s * %(height)s] = {
%(png)s
};

const struct image %(name)s_image = {
  Image_data,
  %(width)s,
  %(height)s
};
'''

def get_png_data(name):
  im = Image.open(name).convert('RGBA')
  pixels = list(im.getdata())
  width, height = im.size
  s = '  '
  i = 1
  for p in pixels:
    r, g, b, a = p[0], p[1], p[2], p[3]
    c = (a << 24) | (r << 16) | (g << 8) | b
    if i % 4:
      s += '0x%x, ' % c
    else:
      if i != len(pixels): 
        s += '0x%x,\n  ' % c
      else:
        s += '0x%x' % c
    i += 1
  return width, height, s

def main(png_file, image_name):
  width, height, png = get_png_data(png_file)
  f = open(image_name.lower() + '.c', 'w')
  f.write(Code % {
    'width': width,
    'height': height,
    'png': png,
    'name': image_name
  })
  f.close()

if len(sys.argv) != 3:
  print('pnga_to_c png_file image_name')
else:
  main(sys.argv[1], sys.argv[2])
