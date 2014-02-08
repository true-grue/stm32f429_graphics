#include "tools.h"
#include "lcd.h"
#include "graphics.h"

extern const struct font Open_sans36_font;

void draw(void) {
  Current_font = &Open_sans36_font;
  Current_color = RGB(0, 0, 0);
  fill_rect(0, 0, LCD_WIDTH, LCD_HEIGHT);
  Current_color = RGB(255, 0, 0);
  draw_centered_text("Hello, world!", 0, LCD_HEIGHT / 2, LCD_WIDTH);
}

int main(void) {
  configure_tools();
  configure_lcd();
  while (1) {
    begin_draw();
    draw();
    end_draw();
    delay_ms(500);
  }
}
