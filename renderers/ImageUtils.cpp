#ifdef HAVE_CONFIG_H
    #include "config.h"
#endif

#include "ImageUtils.h"

#ifdef __APPLE__
    #include <SDL_image/SDL_image.h>
#else
    #include <SDL/SDL_image.h>
#endif

#include <iostream>
#include "common/Log.h"
#include "common/Utility.h"

namespace Sear
{

static SDL_Surface* mipmapSurface32(SDL_Surface* src, SDL_Surface* dst);
static SDL_Surface* mipmapSurface24(SDL_Surface* src, SDL_Surface* dst);
static SDL_Surface* mipmapSurface8(SDL_Surface* src, SDL_Surface* dst);

static SDL_Surface* mipmapSurfaceN1(SDL_Surface* src, SDL_Surface* dst);
static SDL_Surface* mipmapSurface1N(SDL_Surface* src, SDL_Surface* dst);

SDL_Surface* loadImageFromPath(const std::string& filename)
{
  SDL_Surface* image = IMG_Load(filename.c_str());
  if ( image == NULL ) {
    Log::writeLog(std::string("Unable to load ") + filename + std::string(": ") + 
        string_fmt( SDL_GetError()), Log::LOG_ERROR);
    return(NULL);
  }
  
  /* GL surfaces are upside-down */
  Uint8* tmpbuf = (Uint8 *) malloc(image->pitch);
  assert(tmpbuf);
  
  Uint8* rowhi = (Uint8 *)image->pixels;
  Uint8* rowlo = rowhi + ((image->h - 1)* image->pitch);
  
  for (int i=0; i < (image->h/2); ++i ) {
    memcpy(tmpbuf, rowhi, image->pitch);
    memcpy(rowhi, rowlo, image->pitch);
    memcpy(rowlo, tmpbuf, image->pitch);
    rowhi += image->pitch;
    rowlo -= image->pitch;
  }
  
  free(tmpbuf);
  return image;
}

SDL_Surface* mipmapSurface(SDL_Surface* src)
{
    assert(src);
    //assert((src->w % 2) == 0);
    //assert((src->h % 2) == 0);

    int dstW = std::max(1, src->w / 2), dstH = std::max(1, src->h / 2);
    SDL_Surface* dst = SDL_CreateRGBSurface(SDL_SWSURFACE, dstW, dstH, 
        src->format->BitsPerPixel,
        src->format->Rmask, src->format->Bmask, src->format->Gmask, src->format->Amask);
        
    if (!dst) {
        std::cerr << "error creating destination surface for mipmap" << std::endl;
        return NULL;
    }
    
    if (src->w == 1)
        return mipmapSurface1N(src, dst);
        
    if (src->h == 1)
        return mipmapSurfaceN1(src, dst);
        
    if (src->format->BitsPerPixel == 32)
        return mipmapSurface32(src, dst);
        
    if (src->format->BitsPerPixel == 24)
        return mipmapSurface24(src, dst);
        
    if (src->format->BitsPerPixel == 8)
        return mipmapSurface8(src, dst);
        
    std::cerr << "requested mip-mapping of surface with unsuported BitPerPixel: " << 
        (src->format->BitsPerPixel) << std::endl;
    return NULL;
}

SDL_Surface* mipmapSurface32(SDL_Surface* src, SDL_Surface* dst)
{
    int sums[4]; // sums of each channel for the 4 src channels
    Uint8* dstPixel; 
    Uint8* srcRow0, *srcRow1;
    
    for (int Y = 0; Y < dst->h; ++Y) {
        srcRow0 = (Uint8*) src->pixels + (src->pitch * Y * 2);
        srcRow1 = srcRow0 + src->pitch;
        dstPixel = (Uint8*) dst->pixels + (dst->pitch * Y);
        
        for (int X = 0; X < dst->w; ++X) {
            // top-left pixel
            sums[0] = *(srcRow0++);
            sums[1] = *(srcRow0++);
            sums[2] = *(srcRow0++);
            sums[3] = *(srcRow0++);
            
            // top-right pixel
            sums[0] += *(srcRow0++);
            sums[1] += *(srcRow0++);
            sums[2] += *(srcRow0++);
            sums[3] += *(srcRow0++);
            
            // bottom-left pixel
            sums[0] += *(srcRow1++);
            sums[1] += *(srcRow1++);
            sums[2] += *(srcRow1++);
            sums[3] += *(srcRow1++);
            
            // bottom-right pixel
            sums[0] += *(srcRow1++);
            sums[1] += *(srcRow1++);
            sums[2] += *(srcRow1++);
            sums[3] += *(srcRow1++);
            
            // store the averaged values
            *dstPixel++ = (Uint8)(sums[0] >> 2);
            *dstPixel++ = (Uint8)(sums[1] >> 2);
            *dstPixel++ = (Uint8)(sums[2] >> 2);
            *dstPixel++ = (Uint8)(sums[3] >> 2);
        }
        
        srcRow0 += src->pitch;
        srcRow1 += src->pitch;
    }
    
    return dst;
}

SDL_Surface* mipmapSurface24(SDL_Surface* src, SDL_Surface* dst)
{
    int sums[3]; // sums of each channel for the 3 src channels
    Uint8* dstPixel; 
    Uint8* srcRow0, *srcRow1;
    
    for (int Y = 0; Y < dst->h; ++Y) {
        srcRow0 = (Uint8*) src->pixels + (src->pitch * Y * 2);
        srcRow1 = srcRow0 + src->pitch;
        dstPixel = (Uint8*) dst->pixels + (dst->pitch * Y);
        
        for (int X = 0; X < dst->w; ++X) {
            // top-left pixel
            sums[0] = *(srcRow0++);
            sums[1] = *(srcRow0++);
            sums[2] = *(srcRow0++);
            
            // top-right pixel
            sums[0] += *(srcRow0++);
            sums[1] += *(srcRow0++);
            sums[2] += *(srcRow0++);
            
            // bottom-left pixel
            sums[0] += *(srcRow1++);
            sums[1] += *(srcRow1++);
            sums[2] += *(srcRow1++);
            
            // bottom-right pixel
            sums[0] += *(srcRow1++);
            sums[1] += *(srcRow1++);
            sums[2] += *(srcRow1++);
            
            // store the averaged values
            *dstPixel++ = (Uint8)(sums[0] >> 2);
            *dstPixel++ = (Uint8)(sums[1] >> 2);
            *dstPixel++ = (Uint8)(sums[2] >> 2);
        }
        
        srcRow0 += src->pitch;
        srcRow1 += src->pitch;
    }
    
    return dst;
}

SDL_Surface* mipmapSurface8(SDL_Surface* src, SDL_Surface* dst)
{
    int sum; // sums of each channel for the 3 src channels
    Uint8* dstPixel; 
    Uint8* srcRow0, *srcRow1;
    
    for (int Y = 0; Y < dst->h; ++Y) {
        srcRow0 = (Uint8*) src->pixels + (src->pitch * Y * 2);
        srcRow1 = srcRow0 + src->pitch;
        dstPixel = (Uint8*) dst->pixels + (dst->pitch * Y);
        
        for (int X = 0; X < dst->w; ++X) {
            // top-left pixel
            sum = *(srcRow0++);
            // top-right pixel
            sum += *(srcRow0++);
            // bottom-left pixel
            sum += *(srcRow1++);
            // bottom-right pixel
            sum += *(srcRow1++);
                       // store the averaged values
            *dstPixel++ = (Uint8)(sum >> 2);
        }
    }
    
    return dst;
}

static SDL_Surface* mipmapSurfaceN1(SDL_Surface* src, SDL_Surface* dst)
{
    int sums[4];
    
    Uint8* dstPixel = (Uint8*) dst->pixels;
    Uint8* srcPixel = (Uint8*) src->pixels;

    for (int X=0; X < dst->w; ++X)
    {
        for (int chan = 0; chan < dst->format->BytesPerPixel; ++chan) {
          sums[chan] = *srcPixel++;
        }
        
        for (int chan = 0; chan < dst->format->BytesPerPixel; ++ chan) {
            sums[chan] += *srcPixel++;
            *dstPixel++ = (Uint8)(sums[chan] >> 1);
        }
    }
    
    return dst;
}

static SDL_Surface* mipmapSurface1N(SDL_Surface* src, SDL_Surface* dst)
{
    int sums[4];
    Uint8* srcPixel, *dstPixel;
    
    for (int Y=0; Y < dst->h; ++Y)
    {   
        dstPixel = (Uint8*) dst->pixels + (dst->pitch * Y);
        srcPixel = (Uint8*) src->pixels + (src->pitch * Y * 2);
    
        for (int chan = 0; chan < dst->format->BytesPerPixel; ++chan) {
          sums[chan] = srcPixel[chan];
        }
        
        srcPixel += src->pitch; // move on a row
        
        for (int chan = 0; chan < dst->format->BytesPerPixel; ++chan) {
            sums[chan] += srcPixel[chan];
            *dstPixel++ = (Uint8)(sums[chan] >> 1);
        }
    }
    
    return dst;
}

} // of namespace Sear
