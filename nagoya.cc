#include <SDL.h>

Uint32 getpixel(SDL_Surface *surface, int x, int y) {
    int bpp = surface->format->BytesPerPixel;
    Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;

    switch(bpp) {
    case 1:
        return *p;

    case 2:
        return *(Uint16 *)p;

    case 3:
        if(SDL_BYTEORDER == SDL_BIG_ENDIAN)
            return p[0] << 16 | p[1] << 8 | p[2];
        else
            return p[0] | p[1] << 8 | p[2] << 16;
    case 4:
        return *(Uint32 *)p;

    default:
        return 0;
    }
}

void putpixel(SDL_Surface *surface, int x, int y, Uint32 pixel) {
    int bpp = surface->format->BytesPerPixel;
    Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;

    switch(bpp) {
    case 1:
        *p = pixel;
        break;

    case 2:
        *(Uint16 *)p = pixel;
        break;

    case 3:
        if(SDL_BYTEORDER == SDL_BIG_ENDIAN) {
            p[0] = (pixel >> 16) & 0xff;
            p[1] = (pixel >> 8) & 0xff;
            p[2] = pixel & 0xff;
        } else {
            p[0] = pixel & 0xff;
            p[1] = (pixel >> 8) & 0xff;
            p[2] = (pixel >> 16) & 0xff;
        }
        break;

    case 4:
        *(Uint32 *)p = pixel;
        break;
    }
}

int main() {
  SDL_Init(SDL_INIT_VIDEO);
  SDL_Surface* s = SDL_LoadBMP("nagoya.bmp");
  SDL_Surface* o = SDL_CreateRGBSurface(SDL_SWSURFACE, s->w, s->h, 32,
                                        0, 0, 0, 0);

  for (int y = 0; y < s->h; y++) {
    for (int x = 0; x < s->w; x++) {
      Uint32 p = getpixel(s, x, y);
      //printf("%d %d %u\n", x, y, p&1);
      if (p & 1) {
        putpixel(o, x, y, 0xffffffff);
      }
    }
  }
  SDL_SaveBMP(o, "out.bmp");
}
