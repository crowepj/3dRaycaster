#pragma once
#include <stdint.h>
#include <SDL2/SDL.h>

#define SCREEN_UPARROW 0
#define SCREEN_DOWNARROW 1
#define SCREEN_LEFTARROW 2
#define SCREEN_RIGHTARROW 3

#define SCREEN_MOUSEL 4
#define SCREEN_MOUSER 5

struct ScreenState {
  SDL_Window* window;
  SDL_Renderer* renderer;
  SDL_Texture* framebuffer;

  uint32_t* pixels;
  uint32_t width;
  uint32_t height;
  uint8_t keys[6];
  
  uint8_t quit;
};

uint8_t screen_init(struct ScreenState* state, const char* name, uint32_t width, uint32_t height);
void screen_drawpixel(struct ScreenState* state, uint32_t x, uint32_t y, uint32_t pixel);
void screen_render(struct ScreenState* state);
void screen_loop(struct ScreenState* state);
void screen_quit(struct ScreenState* state);
