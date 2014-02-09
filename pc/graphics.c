#include <stdlib.h>
#include "SDL2/SDL.h"
#include "tools.h"
#include "lcd.h"
#include "graphics.h"

extern SDL_Surface* Screen_surface;
extern SDL_Surface* Font_surface;

int Current_color;
const struct font* Current_font;

int to_rgb888(int color) {
}

void fill_rect(int x, int y, int width, int height) {
  SDL_Rect rect;
  rect.x = x;
  rect.y = y;
  rect.w = width;
  rect.h = height;
  SDL_FillRect(Screen_surface, &rect, Current_color);
}

void draw_rect(int x, int y, int width, int height) {
  fill_rect(x, y, width, 1);
  fill_rect(x, y, 1, height);
  fill_rect(x + width - 1, y, 1, height);
  fill_rect(x, y + height - 1, width, 1);
}

void draw_image(const uint16_t* image,
    int image_x,
    int image_y,
    int image_width,
    int x,
    int y,
    int width,
    int height) {
}

void draw_alpha(const uint8_t* image,
    int image_x,
    int image_y,
    int image_width,
    int x,
    int y,
    int width,
    int height) {
  int i;
  int j;
  const uint8_t* src;
  uint32_t* dst;
  SDL_Rect font_rect;
  SDL_Rect screen_rect;
  font_rect.x = 0;
  font_rect.y = 0;
  font_rect.w = width;
  font_rect.h = height;
  screen_rect.x = x;
  screen_rect.y = y;
  screen_rect.w = width;
  screen_rect.h = height;
  src = &image[image_y * image_width + image_x];
  dst = (uint32_t*) Font_surface->pixels;
  for (j = 0; j < height; j += 1) {
    for (i = 0; i < width; i += 1) {
      dst[i] = (Current_color << 8) | src[i];
    }
    src += image_width;
    dst += LCD_WIDTH;
  }
  SDL_BlitSurface(Font_surface, &font_rect, Screen_surface, &screen_rect);
}

void draw_image_alpha(const uint32_t* image,
    int image_x,
    int image_y,
    int image_width,
    int x,
    int y,
    int width,
    int height) {
  SDL_Rect font_rect;
  SDL_Rect screen_rect;
  font_rect.x = 0;
  font_rect.y = 0;
  font_rect.w = width;
  font_rect.h = height;
  screen_rect.x = x;
  screen_rect.y = y;
  screen_rect.w = width;
  screen_rect.h = height;
  SDL_Surface* image_surface = SDL_CreateRGBSurfaceFrom(
    (uint32_t*) &image[image_y * image_width + image_x],
    width,
    height,
    32,
    image_width * 4,
    0x00ff0000,
    0x0000ff00,
    0x000000ff,
    0xff000000);
  SDL_BlitSurface(image_surface, &font_rect, Screen_surface, &screen_rect);
  SDL_FreeSurface(image_surface);
}

void draw_text(char* text, int x, int y) {
  const struct bmfont_data* f;
  while (*text) {
    f = &Current_font->data[*text - 32];
    draw_alpha(Current_font->image,
      f->x,
      f->y,
      Current_font->width,
      x + f->xoffset,
      y + f->yoffset,
      f->width,
      f->height);
    x += f->xadvance;
    text += 1;
  }
}

int get_text_width(char* text) {
  int width = 0;
  while(*text) {
    width += Current_font->data[*text - 32].xadvance;
    text += 1;
  }
  return width;
}

void draw_centered_text(char* text, int x, int y, int width) {
  draw_text(text, x + width / 2 - get_text_width(text) / 2, y);
}

void draw_point(int x, int y) {
  uint32_t* buffer = (uint32_t*) Screen_surface->pixels;
  SDL_LockSurface(Screen_surface);
  buffer[LCD_WIDTH * y + x] = Current_color;
  SDL_UnlockSurface(Screen_surface);
}

void draw_line(int x1, int y1, int x2, int y2) {
  int i;
  int dx = x2 - x1;
  int dy = y2 - y1;
  int steps = MAX(MAX(abs(dx), abs(dy)), 1);
  int x_step = (dx << 16) / steps;
  int y_step = (dy << 16) / steps;
  x1 <<= 16;
  y1 <<= 16;
  for (i = 0; i <= steps; i += 1) {
    draw_point((x1 + 32768) >> 16, (y1 + 32768) >> 16);
    x1 += x_step;
    y1 += y_step;
  }
}

void draw_circle(int cx, int cy, int r) {
  int x = r;
  int y = 0;
  int d = 1 - r;
  while (x >= y) {
    draw_point(x + cx, y + cy);
    draw_point(-x + cx, y + cy);
    draw_point(x + cx, -y + cy);
    draw_point(-x + cx, -y + cy);
    draw_point(y + cx, x + cy);
    draw_point(-y + cx, x + cy);
    draw_point(y + cx, -x + cy);
    draw_point(-y + cx, -x + cy);
    y += 1;
    if (d < 0) {
      d += y * 2 + 1;
    } else {
      x -= 1;
      d += (y - x + 1) * 2;
    }
  }
}

void fill_circle(int cx, int cy, int r) {
  int x = r;
  int y = 0;
  int d = 1 - r;
  while (x >= y) {
    fill_rect(-x + cx, y + cy, x * 2 + 1, 1);
    fill_rect(-x + cx, -y + cy, x * 2 + 1, 1);
    fill_rect(-y + cx, x + cy, y * 2 + 1, 1);
    fill_rect(-y + cx, -x + cy, y * 2 + 1, 1);
    y += 1;
    if (d < 0) {
      d += y * 2 + 1;
    } else {
      x -= 1;
      d += (y - x + 1) * 2;
    }
  }
}

void draw_rounded_rect(int x1, int y1, int width, int height, int r) {
  int x = r;
  int y = 0;
  int d = 1 - r;
  int x2 = x1 + width - 1;
  int y2 = y1 + height - 1;
  while (x >= y) {
    draw_point(x2 + x - r, y2 + y - r);
    draw_point(x2 + y - r, y2 + x - r);
    draw_point(x1 - x + r, y2 + y - r);
    draw_point(x1 - y + r, y2 + x - r);
    draw_point(x2 + x - r, y1 - y + r);
    draw_point(x2 + y - r, y1 - x + r);
    draw_point(x1 - x + r, y1 - y + r);
    draw_point(x1 - y + r, y1 - x + r);
    y += 1;
    if (d < 0) {
      d += y * 2 + 1;
    } else {
      x -= 1;
      d += (y - x + 1) * 2;
    }
  }
  fill_rect(x1 + r, y1, width - r * 2, 1);
  fill_rect(x1 + r, y2, width - r * 2, 1);
  fill_rect(x1, y1 + r, 1, height - r * 2);
  fill_rect(x2, y1 + r, 1, height - r * 2);
}

void fill_rounded_rect(int x1, int y1, int width, int height, int r) {
  int x = r;
  int y = 0;
  int d = 1 - r;
  int y2 = y1 + height - 1;
  while (x >= y) {
    fill_rect(x1 - x + r, y2 + y - r, width + x * 2 - r * 2, 1);
    fill_rect(x1 - y + r, y2 + x - r, width + y * 2 - r * 2, 1);
    fill_rect(x1 - x + r, y1 - y + r, width + x * 2 - r * 2, 1);
    fill_rect(x1 - y + r, y1 - x + r, width + y * 2 - r * 2, 1);
    y += 1;
    if (d < 0) {
      d += y * 2 + 1;
    } else {
      x -= 1;
      d += (y - x + 1) * 2;
    }
  }
  fill_rect(x1, y1 + r, width, height - r * 2);
}
