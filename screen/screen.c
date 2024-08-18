#include "screen.h"
#include <SDL2/SDL.h>

uint8_t screen_init(struct ScreenState* state, const char* name, uint32_t width, uint32_t height) {
  if (SDL_Init(SDL_INIT_VIDEO) < 0)
    return 1;
  
  state->window = SDL_CreateWindow(name,
				   SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
				   width, height,
				   SDL_WINDOW_SHOWN);
  state->width = width;
  state->height = height;
  
  if (state->window == NULL)
    return 1;

  state->renderer = SDL_CreateRenderer(state->window, -1, SDL_RENDERER_ACCELERATED);
  if (state->renderer == NULL)
    return 1;

  state->framebuffer = SDL_CreateTexture(state->renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, width, height);
  if (state->framebuffer == NULL)
    return 1;

  state->pixels = malloc(width * height * sizeof(uint32_t));
  if (state->pixels == NULL)
    return 1;
  
  state->quit = 0;
  return 0;
}

void screen_drawpixel(struct ScreenState* state, uint32_t x, uint32_t y, uint32_t pixel) {
  state->pixels[(y * state->width) + x] = pixel;
}

void screen_render(struct ScreenState* state) {
  SDL_UpdateTexture(state->framebuffer, NULL, state->pixels, state->width * sizeof(uint32_t));
  SDL_RenderClear(state->renderer);
  SDL_RenderCopy(state->renderer, state->framebuffer, NULL, NULL);
  SDL_RenderPresent(state->renderer);

  for (int i = 0; i < state->width * state->height; i++) {
    state->pixels[i] = 0x0;
  }
}

void screen_loop(struct ScreenState* state) {
  SDL_Event event;
  uint8_t pressed = 0;
  if(SDL_PollEvent(&event)) {
    if (event.type == SDL_QUIT)
      state->quit = 1;

    if (event.type == SDL_KEYUP || event.type == SDL_KEYDOWN) {
      if (event.type == SDL_KEYDOWN)
	pressed = 1;
      switch (event.key.keysym.sym) {
      case SDLK_ESCAPE:
	state->quit = pressed;
	break;
      case SDLK_UP:
	state->keys[SCREEN_UPARROW] = pressed;
	break;
      case SDLK_DOWN:
	state->keys[SCREEN_DOWNARROW] = pressed;
	break;
      case SDLK_LEFT:
	state->keys[SCREEN_LEFTARROW] = pressed;
	break;
      case SDLK_RIGHT:
	state->keys[SCREEN_RIGHTARROW] = pressed;
	break;
      }
    }
    else if (event.type == SDL_MOUSEBUTTONDOWN || event.type == SDL_MOUSEBUTTONUP) {
      if (event.type == SDL_MOUSEBUTTONDOWN)
	pressed = 1;
      
      state->keys[event.button.button == SDL_BUTTON_LEFT ? SCREEN_MOUSEL : SCREEN_MOUSER] = pressed;
    }
  }
}

void screen_quit(struct ScreenState* state) {
  SDL_DestroyTexture(state->framebuffer);
  SDL_DestroyRenderer(state->renderer);
  SDL_DestroyWindow(state->window);
  free(state->pixels);
}
