
#include <sys/time.h>
#include <stdio.h>
#include <screen/screen.h>

const char* gameName = "Renderer";
struct ScreenState screen;

//
int winWidth = 1280;
int winHeight = 720;
//

//
#define mapWidth 20
#define mapHeight 20
//

//
float playerX = 7;
float playerY = 5;

float playerAngle = 0;

float playerDirX = -1;
float playerDirY = 0;

float planeX = 0;
float planeY = 0.66;
//

const int map[mapWidth][mapHeight] = {
  {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
  {1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1},
  {1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1},
  {1, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1},
  {1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
  {1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
  {1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
  {1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
  {1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
  {1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
  {1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
  {1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
  {1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
  {1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
  {1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
  {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
  {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
  {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
  {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
  {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
};

const SDL_Surface** textures = NULL;
unsigned int textures_len = 0;

// Load a texture from a BMP file, and store it into the textures array
// Return -1 on failure, or the index of the texture on success
int loadTexture(const char* file) {
  SDL_Surface* texture = SDL_LoadBMP(file);
  if (texture == NULL)
    return -1;

  void* temp = realloc(textures, ++textures_len * sizeof(SDL_Surface*));
  if (temp == NULL)
    return -1;
  
  textures = temp;
  textures[textures_len - 1] = texture;
  
  return (textures_len - 1);
}

// https://stackoverflow.com/questions/53033971/how-to-get-the-color-of-a-specific-pixel-from-sdl-surface
uint32_t getPixel(SDL_Surface* surface, int x, int y)
{
  int bpp = surface->format->BytesPerPixel;
  /* Here p is the address to the pixel we want to retrieve */
  uint8_t* p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;

  switch (bpp)
    {
    case 1:
      return *p;
      break;

    case 2:
      return *(uint16_t*)p;
      break;

    case 3:
      if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
	return p[0] << 16 | p[1] << 8 | p[2];
      else
	return p[0] | p[1] << 8 | p[2] << 16;
      break;

    case 4:
      return *(uint32_t*)p;
      break;

    default:
      return 0;       /* shouldn't happen, but avoids warnings */
    }
}

int timeMill() {
  struct timeval tv;
  gettimeofday(&tv, NULL);

  return (tv.tv_sec) * 1000 + (tv.tv_usec) / 1000;
}

void render() {
  float pointX = (playerX);
  float pointY = (playerY);
  for (int x = 0; x < winWidth; x++) {
    float cameraX = 2 * x / (float)winWidth - 1;
    float rayDirX = playerDirX + planeX * cameraX;
    float rayDirY = playerDirY + planeY * cameraX;

    int mapX = (int)pointX;
    int mapY = (int)pointY;

    // DDA variables
    int stepX;
    int stepY;

    // Distance to next X side
    float sideDistX;

    // Distance to next Y side
    float sideDistY;

    float deltaDistX = (rayDirX == 0) ? 1e30 : fabs(1 / rayDirX);
    float deltaDistY = (rayDirY == 0) ? 1e30 : fabs(1 / rayDirY);
    int side;

    // Initialise DDA variables
    if (rayDirX >= 0) {
      stepX = 1;
      sideDistX = (mapX + 1.0 - pointX) * deltaDistX;
    }
    else {
      stepX = -1;
      sideDistX = (pointX - mapX) * deltaDistX;
    }
    if (rayDirY >= 0) {
      stepY = 1;
      sideDistY = (mapY + 1.0 - pointY) * deltaDistY;
    }
    else {
      stepY = -1;
      sideDistY = (pointY - mapY) * deltaDistY;
    }

    int hit = 0;
    float dist;

    // Cast the ray
    while (hit == 0) {
      // Stepping in X or Y direction
      if (sideDistX < sideDistY) {
          sideDistX += deltaDistX;
          mapX += stepX;
          side = 0;
      }
      else {
          sideDistY += deltaDistY;
          mapY += stepY;
          side = 1;
      }
      if (map[mapY][mapX] > 0) hit = map[mapY][mapX];
    }

    // If we hit a wall
    if (hit) {
      if(side == 0) dist = (sideDistX - deltaDistX);
      else          dist = (sideDistY - deltaDistY);
	
      int lineHeight = (int)(winHeight / dist);
      int drawStart = (winHeight - lineHeight) / 2;
      if(drawStart < 0) drawStart = 0;
      int drawEnd = (winHeight + lineHeight) / 2;
      if(drawEnd >= winHeight) drawEnd = winHeight - 1;

      float wallX;
      // If the ray hit a N/S side
      if (side)
	wallX = playerX + (dist * rayDirX);
      else
	wallX = playerY + (dist * rayDirY);
      wallX -= (int)wallX;
	
      const SDL_Surface* texture = textures[hit - 1];
      float texStep = (float)texture->h / (float)lineHeight;
      float texPos = (drawStart - winHeight / 2 + lineHeight / 2) * texStep;
      printf("Texpos: %f\n", texPos);
      //int textureX = (float)texture->w * wallX;

      int textureX = (wallX * (double)texture->w);
      if(side == 0 && rayDirX > 0) textureX = texture->w - textureX - 1;
      if(side == 1 && rayDirY < 0) textureX = texture->w - textureX - 1;

      int j = 0;
      for (int i = drawStart; i < drawEnd; i++) {
	int textureY = (int)texPos & (texture->h - 1);
	texPos += texStep;
	
	uint32_t pixel = getPixel(texture, textureX, textureY);
	uint8_t r;
	uint8_t g;
	uint8_t b;
	
	SDL_GetRGB(pixel, texture->format, &r, &g, &b);
	
	screen_drawpixel(&screen, x, i, (r << 24) | (g << 16) | (b << 8) | 0xFF);
	j++;
      }
    }
  }
  screen_render(&screen);
}

int main() {
  if (screen_init(&screen, gameName, winWidth, winHeight)) {
    fprintf(stderr, "screen_init() failed\n");
  }

  if (loadTexture("texture/wall.bmp") == -1) {
    return -1;
  }
  
  int times = timeMill();
  int oldtime = times;
  float deltatime = 0;
  float speed = 2;
  float turnspeed = 2;
  
  while (!screen.quit) {
    times = timeMill();
    deltatime = (float)(times - oldtime) / 1000.0f;
    screen_loop(&screen);
    if (screen.keys[SCREEN_UPARROW]) {
      float moveX = playerDirX * deltatime * speed;
      float moveY = playerDirY * deltatime * speed;
      if (map[(int)(playerY + moveY)][(int)(playerX + moveX)] == 0) {
	playerX += moveX;
	playerY += moveY;
      }
    }
    else if (screen.keys[SCREEN_DOWNARROW]) {
      float moveX = playerDirX * deltatime * -speed;
      float moveY = playerDirY * deltatime * -speed;
      if (map[(int)(playerY + moveY)][(int)(playerX + moveX)] == 0) {
	playerX += moveX;
	playerY += moveY;
      }
    }
    if (screen.keys[SCREEN_LEFTARROW]) {
      playerDirX = (cos(1 * deltatime * turnspeed) * playerDirX) + (-sin(1 * deltatime * turnspeed) * playerDirY);
      playerDirY = (sin(1 * deltatime * turnspeed) * playerDirX) + (cos(1 * deltatime * turnspeed) * playerDirY);

      planeX = (cos(1 * deltatime * turnspeed) * planeX) + (-sin(1 * deltatime * turnspeed) * planeY);
      planeY = (sin(1 * deltatime * turnspeed) * planeX) + (cos(1 * deltatime * turnspeed) * planeY);
    }
    else if (screen.keys[SCREEN_RIGHTARROW]) {
      playerDirX = (cos(-1 * deltatime * turnspeed) * playerDirX) + (-sin(-1 * deltatime * turnspeed) * playerDirY);
      playerDirY = (sin(-1 * deltatime * turnspeed) * playerDirX) + (cos(-1 * deltatime * turnspeed) * playerDirY);

      planeX = (cos(-1 * deltatime * turnspeed) * planeX) + (-sin(-1 * deltatime * turnspeed) * planeY);
      planeY = (sin(-1 * deltatime * turnspeed) * planeX) + (cos(-1 * deltatime * turnspeed) * planeY);
    }
   
    render();
    oldtime = times;
  }
}
