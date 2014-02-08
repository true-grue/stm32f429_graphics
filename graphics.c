#include <stdlib.h>
#include "tools.h"
#include "lcd.h"
#include "graphics.h"

int Current_color;
const struct font* Current_font;

int to_rgb888(int color) {
  int r5 = (color & 0xf800) >> 11;
  int g6 = (color & 0x7e0) >> 5;
  int b5 = color & 0x1f;
  int r8 = (r5 << 3) | (r5 >> 2);
  int g8 = (g6 << 2) | (g6 >> 4);
  int b8 = (b5 << 3) | (b5 >> 2);
  return (r8 << 16) | (g8 << 8) | b8;
}

static void dma_to_screen(int x, int y, int width, int height) {
  DMA2D->OPFCCR = CM_RGB565;
  DMA2D->OCOLR = Current_color;
  DMA2D->OMAR = (uint32_t) &LCD_buffer[LCD_WIDTH * y + x];
  DMA2D->OOR = LCD_WIDTH - width;
  DMA2D->NLR = (uint32_t) ((width << 16) | height);
  DMA2D->CR |= 1;
  while (DMA2D->CR & DMA2D_CR_START) {
  }
}

void fill_rect(int x, int y, int width, int height) {
  DMA2D->CR = DMA2D_R2M;
  dma_to_screen(x, y, width, height);
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
  DMA2D->FGPFCCR = CM_RGB565;
  DMA2D->FGCOLR = 0;
  DMA2D->FGMAR = (uint32_t) &image[image_width * image_y + image_x];
  DMA2D->FGOR = image_width - width;
  DMA2D->CR = DMA2D_M2M;
  dma_to_screen(x, y, width, height);
}

void draw_alpha(const uint8_t* image,
    int image_x,
    int image_y,
    int image_width,
    int x,
    int y,
    int width,
    int height) {
  DMA2D->FGPFCCR = CM_A8;
  DMA2D->FGCOLR = to_rgb888(Current_color);
  DMA2D->FGMAR = (uint32_t) &image[image_width * image_y + image_x];
  DMA2D->FGOR = image_width - width;
  DMA2D->BGPFCCR = CM_RGB565;
  DMA2D->BGCOLR = 0;
  DMA2D->BGMAR = (uint32_t) &LCD_buffer[LCD_WIDTH * y + x];
  DMA2D->BGOR = LCD_WIDTH - width;
  DMA2D->CR = DMA2D_M2M_BLEND;
  dma_to_screen(x, y, width, height);
}

void draw_image_alpha(const uint32_t* image,
    int image_x,
    int image_y,
    int image_width,
    int x,
    int y,
    int width,
    int height) {
  DMA2D->FGPFCCR = CM_ARGB8888;
  DMA2D->FGCOLR = 0;
  DMA2D->FGMAR = (uint32_t) &image[image_width * image_y + image_x];
  DMA2D->FGOR = image_width - width;
  DMA2D->BGPFCCR = CM_RGB565;
  DMA2D->BGCOLR = 0;
  DMA2D->BGMAR = (uint32_t) &LCD_buffer[LCD_WIDTH * y + x];
  DMA2D->BGOR = LCD_WIDTH - width;
  DMA2D->CR = DMA2D_M2M_BLEND;
  dma_to_screen(x, y, width, height);
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
  LCD_buffer[(LCD_WIDTH * y + x) & LCD_BUFFER_MASK] = Current_color;
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
