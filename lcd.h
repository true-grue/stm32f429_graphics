#ifndef LCD_H
#define LCD_H

#define VRAM ((uint16_t*) 0xd0000000)

#define LCD_BUFFER_MASK 0x1ffff

#define LCD_WIDTH 480
#define LCD_HEIGHT 272

#define VSYNC 10
#define HSYNC 10

#define VBP 12
#define VFP 4

#define HBP 43
#define HFP 8

#define RGB(r, g, b) ((((r) >> 3) << 11) | (((g) >> 2) << 5) | ((b) >> 3))

extern uint16_t* LCD_buffer;

void begin_draw(void);
void end_draw(void);
void configure_lcd(void);

#endif
