#include <stdio.h>
#include <stdint.h>
#include "SDL2/SDL.h"
#include "lcd.h"
#include "graphics.h"

SDL_Surface* Screen_surface;
SDL_Surface* Font_surface;

extern const struct font Open_sans36_font;

void draw(void) {
  Current_font = &Open_sans36_font;
  Current_color = RGB(0, 0, 0);
  fill_rect(0, 0, LCD_WIDTH, LCD_HEIGHT);
  Current_color = RGB(255, 0, 0);
  draw_centered_text("Hello, world!", 0, LCD_HEIGHT / 2, LCD_WIDTH);
}

int main(int argc, char** argv) { 
  SDL_Event e;
  SDL_Window* window;
  int quit = 0;
  SDL_Init(SDL_INIT_VIDEO);
  window = SDL_CreateWindow("Graphics",
    SDL_WINDOWPOS_UNDEFINED,
    SDL_WINDOWPOS_UNDEFINED,
    LCD_WIDTH,
    LCD_HEIGHT,
    0);
  Screen_surface = SDL_GetWindowSurface(window);
  Font_surface = SDL_CreateRGBSurface(0,
    LCD_WIDTH,
    LCD_HEIGHT,
    32,
    0xff000000,
    0x00ff0000,
    0x0000ff00,
    0x000000ff);
  while (!quit) {
    while(SDL_PollEvent(&e) != 0) {
      if(e.type == SDL_QUIT) {
        quit = 1;
      }
    }
    draw();
    SDL_UpdateWindowSurface(window);
    SDL_Delay(500);
  }
  SDL_FreeSurface(Screen_surface);
  SDL_FreeSurface(Font_surface);
  SDL_DestroyWindow(window); 
  SDL_Quit(); 
  return 0;   
}
