#ifndef SEAR_IMAGE_UTILS_H
#define SEAR_IMAGE_UTILS_H

#include <SDL/SDL.h>
#include <string>

namespace Sear
{
    /** load an image file from the specified path, and flip it
    vertically, so it may be used for OpenGL texturing (perhaps this
    should be optional?
    
    If the image file cannot be loaded for any reason, NULL is returned.
    */
    SDL_Surface* loadImageFromPath(const std::string& filename);

    /** create a copy of the input surface, halved in size, with each 2x2 box
    of pixels in the source image averaged to produce a single destination
    pixel. The source surface is unchanged. Note the source surface's width
    and height must both be divisible by two.
    
    Returns NULL if the mipmap cannot be created for any reason.
    */
    SDL_Surface* mipmapSurface(SDL_Surface* src);
}

#endif