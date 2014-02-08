#ifndef GRAPHICS_H
#define GRAPHICS_H

struct image {
  const uint32_t* data;
  int width;
  int height;
};

struct bmfont_data {
  int16_t x;
  int16_t y;
  int16_t width;
  int16_t height;
  int16_t xoffset;
  int16_t yoffset;
  int16_t xadvance;
};

struct font {
  const uint8_t* image;
  int width;
  const struct bmfont_data* data;
};

extern int Current_color;
extern const struct font* Current_font;

int to_rgb888(int color);
void fill_rect(int x, int y, int width, int height);
void draw_rect(int x, int y, int width, int height);
void draw_image(const uint16_t* image,
    int image_x,
    int image_y,
    int image_width,
    int x,
    int y,
    int width,
    int height);
void draw_alpha(const uint8_t* image,
    int image_x,
    int image_y,
    int image_width,
    int x,
    int y,
    int width,
    int height);
void draw_image_alpha(const uint32_t* image,
    int image_x,
    int image_y,
    int image_width,
    int x,
    int y,
    int width,
    int height);
void draw_text(char* text, int x, int y);
int get_text_width(char* text);
void draw_centered_text(char* text, int x, int y, int width);
void draw_point(int x, int y);
void draw_line(int x1, int y1, int x2, int y2);
void draw_circle(int cx, int cy, int r);
void fill_circle(int cx, int cy, int r);
void draw_rounded_rect(int x1, int y1, int width, int height, int r);
void fill_rounded_rect(int x1, int y1, int width, int height, int r);

#endif
