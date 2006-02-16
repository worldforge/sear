// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2004 - 2006 Simon Goodall

// $Id: Sprite.cpp,v 1.10 2006-02-16 15:59:01 simon Exp $

#include "Sprite.h"
#include "TextureManager.h"
#include "RenderSystem.h"
#include "ImageUtils.h"

#include <iostream>

#include "src/System.h"
#include "src/FileHandler.h"
#include "Render.h"
#include "Graphics.h"

using std::cerr;
using std::endl;

namespace Sear
{

Sprite::Sprite(const std::string& spriteName)
{
    TextureManager* texm = RenderSystem::getInstance().getTextureManager();
    m_data = texm->getSpriteData(spriteName); // might return NULL
    if (m_data) m_data->addRef();
}

Sprite::~Sprite()
{
    if (m_data) m_data->decRef();
}

Sprite::Sprite(const Sprite& other) :
    m_data(other.m_data)
{
    if (m_data) m_data->addRef();
}

Sprite& Sprite::operator=(const Sprite& other)
{
    if (m_data) m_data->decRef();
    m_data = other.m_data;
    if (m_data) m_data->addRef();
    return *this;
}

void Sprite::draw(Render* r)
{
    if (m_data)
        m_data->draw(r);
    else
        cerr << "called draw on NULL sprite reference" << endl;
}

SpriteData::SpriteData(const std::string& spriteName) :
    m_refCount(0),
    m_valid(false),
    m_textureId(0)
{   
    assert(!spriteName.empty());
    m_name = spriteName;
    // Check if the texture has a filename specified
    if (!getSpriteConfig().findItem(spriteName, "filename")) {
      // cerr causes an error in valgrind.
      fprintf(stderr, "Sprite %s has no filename defined\n", spriteName.c_str());
//        std::cerr << "Sprite " << spriteName << " has no filename defined" << std::endl;
    }
}

SpriteData::~SpriteData()
{
    assert(m_refCount == 0);
    contextDestroyed(true);
}

void SpriteData::contextCreated() {

}

void SpriteData::contextDestroyed(bool check)
{
    if (m_valid && check && glIsTexture(m_textureId)) {
        glDeleteTextures(1, &m_textureId);
    }
    
    m_textureId = 0;
    m_valid = false;
}

unsigned int SpriteData::twoN(unsigned int size)
{
    for (int N = 0; N < 12; ++N) {
        const unsigned int num = (1 << N);
        if (size <= num) return num;
    }
    
    return 1 << 12; // arbitrary maximum size
}

void SpriteData::load()
{
    assert(!m_valid);
    
// read config file
    std::string filename = getSpriteConfig().getItem(m_name, "filename");
    System::instance()->getFileHandler()->expandString(filename);

// read in image data
    SDL_Surface *img = loadImageFromPath(filename);
    if (img == NULL) {
        loadFail();
        return;
    }
    
    const unsigned int textur_w = twoN(img->w);
    const unsigned int textur_h = twoN(img->h);
    m_width = img->w;
    m_height = img->h;
     
// scale to a power of 2 size
    SDL_Surface * texImage = SDL_CreateRGBSurface(SDL_SWSURFACE, textur_w,
                                                  textur_h, 32,
#if SDL_BYTEORDER == SDL_LIL_ENDIAN
        0x000000FF, 
        0x0000FF00, 
        0x00FF0000, 
        0xFF000000
#else
        0xFF000000,
        0x00FF0000, 
        0x0000FF00, 
        0x000000FF
#endif
        );

    SDL_SetAlpha(img, 0, 0);
    SDL_Rect dest = { 0, textur_h - m_height, m_width, m_height };
    SDL_BlitSurface(img, NULL, texImage, &dest);
    SDL_FreeSurface(img);
    
// actually insert the texture into OpenGL            
    glGenTextures(1, &m_textureId);
    glBindTexture(GL_TEXTURE_2D, m_textureId);
    
    int format, fmt;
    int bpp = texImage->format->BitsPerPixel;
                                                                                
#if SDL_BYTEORDER == SDL_LIL_ENDIAN
    if (texImage->format->Rshift > texImage->format->Bshift) {
#else // SDL_BYTEORDER == SDL_LIL_ENDIAN
    if (texImage->format->Rshift < texImage->format->Bshift) {
#endif // SDL_BYTEORDER == SDL_LIL_ENDIAN
      if (sage_ext[GL_EXT_bgra]) {
        format = (bpp == 24) ? GL_BGR_EXT : GL_BGRA_EXT;
      } else {
        fprintf(stderr, "Unsupported sprite format bgr. No GL_EXT_bgra extension\n");
        SDL_FreeSurface(texImage);
        loadFail();
        return;
      }
    } else {
        format = (bpp == 24) ? GL_RGB : GL_RGBA;
    }

    if (getSpriteConfig().findItem(m_name, "internal_format")) {
        std::string formatString = getSpriteConfig().getItem(m_name, "internal_format");
        fmt = TextureManager::getFormat(formatString);
    } else {
        // presumably this is 16-bit only / mask only hack
        fmt = (bpp == 24) ? GL_RGB5 : GL_RGB5_A1;
    }
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  
    glTexImage2D(GL_TEXTURE_2D, 0, fmt, textur_w, textur_h, 
        0, format, GL_UNSIGNED_BYTE, texImage->pixels);
    SDL_FreeSurface(texImage);

// bonus stuff, set prioirty if requested
    if (getSpriteConfig().findItem(m_name, "priority")) {
        float priority = (double) getSpriteConfig().getItem(m_name, "priority");
        glPrioritizeTextures(1, &m_textureId, &priority);
    }

    m_pw = (float) m_width / textur_w;
    m_ph = (float) m_height / textur_h;
    
    RenderSystem::getInstance().getTextureManager()->clearLastTexture(0);
    m_valid = true;
}

void SpriteData::loadFail()
{
    m_valid = false;
    
    m_textureId = 1;
    m_pw = m_ph = 1;
    m_width = m_height = 64;
}

void SpriteData::draw(Render* render)
{
    if (m_textureId == 0) load();
    
    RenderSystem::getInstance().switchState(RenderSystem::getInstance().requestState("sprite"));

    render->setColour(1.f, 1.f, 1.f, 1.f);
    
    glBindTexture(GL_TEXTURE_2D, m_textureId);
    
    float texcoords[] = { 0, 0,
                          m_pw, 0,
                          m_pw, m_ph,
                          0, m_ph };
    float vertices[] = { -m_width/2.f, -m_height/2.f, 0.f,
                          m_width/2.f, -m_height/2.f, 0.f,
                          m_width/2.f, m_height/2.f, 0.f,
                         -m_width/2.f, m_height/2.f, 0.f };
  
    render->renderArrays(RES_QUADS, 0, 4, (Vertex_3*) vertices, (Texel*) texcoords, NULL, false);
    
    RenderSystem::getInstance().getTextureManager()->clearLastTexture(0);
}

void SpriteData::decRef()
{
    assert(m_refCount > 0);
    if (--m_refCount == 0) {
        
    }
}

varconf::Config& SpriteData::getSpriteConfig()
{
    return RenderSystem::getInstance().getTextureManager()->getSpriteConfig();
}

}
