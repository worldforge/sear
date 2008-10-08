/*      _______   __   __   __   ______   __   __   _______   __   __
 *     / _____/\ / /\ / /\ / /\ / ____/\ / /\ / /\ / ___  /\ /  |\/ /\
 *    / /\____\// / // / // / // /\___\// /_// / // /\_/ / // , |/ / /
 *   / / /__   / / // / // / // / /    / ___  / // ___  / // /| ' / /
 *  / /_// /\ / /_// / // / // /_/_   / / // / // /\_/ / // / |  / /
 * /______/ //______/ //_/ //_____/\ /_/ //_/ //_/ //_/ //_/ /|_/ /
 * \______\/ \______\/ \_\/ \_____\/ \_\/ \_\/ \_\/ \_\/ \_\/ \_\/
 *
 * Copyright (c) 2004, 2005, 2006, 2007 Olof Naessén and Per Larsson
 *
 *                                                         Js_./
 * Per Larsson a.k.a finalman                          _RqZ{a<^_aa
 * Olof Naessén a.k.a jansem/yakslem                _asww7!uY`>  )\a//
 *                                                 _Qhm`] _f "'c  1!5m
 * Visit: http://guichan.darkbits.org             )Qk<P ` _: :+' .'  "{[
 *                                               .)j(] .d_/ '-(  P .   S
 * License: (BSD)                                <Td/Z <fP"5(\"??"\a.  .L
 * Redistribution and use in source and          _dV>ws?a-?'      ._/L  #'
 * binary forms, with or without                 )4d[#7r, .   '     )d`)[
 * modification, are permitted provided         _Q-5'5W..j/?'   -?!\)cam'
 * that the following conditions are met:       j<<WP+k/);.        _W=j f
 * 1. Redistributions of source code must       .$%w\/]Q  . ."'  .  mj$
 *    retain the above copyright notice,        ]E.pYY(Q]>.   a     J@\
 *    this list of conditions and the           j(]1u<sE"L,. .   ./^ ]{a
 *    following disclaimer.                     4'_uomm\.  )L);-4     (3=
 * 2. Redistributions in binary form must        )_]X{Z('a_"a7'<a"a,  ]"[
 *    reproduce the above copyright notice,       #}<]m7`Za??4,P-"'7. ).m
 *    this list of conditions and the            ]d2e)Q(<Q(  ?94   b-  LQ/
 *    following disclaimer in the                <B!</]C)d_, '(<' .f. =C+m
 *    documentation and/or other materials      .Z!=J ]e []('-4f _ ) -.)m]'
 *    provided with the distribution.          .w[5]' _[ /.)_-"+?   _/ <W"
 * 3. Neither the name of Guichan nor the      :$we` _! + _/ .        j?
 *    names of its contributors may be used     =3)= _f  (_yQmWW$#(    "
 *    to endorse or promote products derived     -   W,  sQQQQmZQ#Wwa]..
 *    from this software without specific        (js, \[QQW$QWW#?!V"".
 *    prior written permission.                    ]y:.<\..          .
 *                                                 -]n w/ '         [.
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT       )/ )/           !
 * HOLDERS AND CONTRIBUTORS "AS IS" AND ANY         <  (; sac    ,    '
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING,               ]^ .-  %
 * BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF            c <   r
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR            aga<  <La
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE          5%  )P'-3L
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR        _bQf` y`..)a
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,          ,J?4P'.P"_(\?d'.,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES               _Pa,)!f/<[]/  ?"
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT      _2-..:. .r+_,.. .
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,     ?a.<%"'  " -'.a_ _,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION)                     ^
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
 * IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 * For comments regarding functions please see the header file.
 */

#include "imagefontxpm.h"

#include <sstream>

#include <SDL_image.h>

#include <guichan/color.hpp>
#include <guichan/exception.hpp>
#include <guichan/graphics.hpp>
#include <guichan/image.hpp>
#include <guichan/sdl/sdlimage.hpp>

#include <cassert>

namespace gcn
{

ImageFontXPM::ImageFontXPM(const std::string &name, const char **array, const std::string &glyphs) {
  mFilename = name;

  SDL_Surface *s = IMG_ReadXPMFromArray(const_cast<char**>(array));
  SDL_Surface *surface = convertToStandardFormat(s);
  SDL_FreeSurface(s);

  mImage = new OpenGLImage((unsigned int*)surface->pixels,
                                                 surface->w,
                                                 surface->h,
                                                 false);
  SDL_FreeSurface(surface);

  Color separator = mImage->getPixel(0, 0);

  int i = 0;
  for (i=0; separator == mImage->getPixel(i, 0)
           && i < mImage->getWidth(); ++i)
  {
  }

  if (i >= mImage->getWidth())
  {
      throw GCN_EXCEPTION("Corrupt image.");
  }

  int j = 0;
  for (j = 0; j < mImage->getHeight(); ++j)
  {
      if (separator == mImage->getPixel(i, j))
      {
          break;
      }
  }

  mHeight = j;
  int x = 0, y = 0;
  unsigned char k;

  for (i=0; i < (int)glyphs.size(); ++i)
  {
      k = glyphs.at(i);
      addGlyph(k, x, y, separator);
  }

  mImage->convertToDisplayFormat();

  mRowSpacing = 0;
  mGlyphSpacing = 0;
}



    ImageFontXPM::ImageFontXPM(const std::string& filename, const std::string& glyphs)
    {
        mFilename = filename;
        mImage = Image::load(filename, false);

        Color separator = mImage->getPixel(0, 0);

        int i = 0;
        for (i=0; separator == mImage->getPixel(i, 0)
                 && i < mImage->getWidth(); ++i)
        {
        }

        if (i >= mImage->getWidth())
        {
            throw GCN_EXCEPTION("Corrupt image.");
        }

        int j = 0;
        for (j = 0; j < mImage->getHeight(); ++j)
        {
            if (separator == mImage->getPixel(i, j))
            {
                break;
            }
        }

        mHeight = j;
        int x = 0, y = 0;
        unsigned char k;

        for (i=0; i < (int)glyphs.size(); ++i)
        {
            k = glyphs.at(i);
            addGlyph(k, x, y, separator);
        }

        mImage->convertToDisplayFormat();

        mRowSpacing = 0;
        mGlyphSpacing = 0;
    }

    ImageFontXPM::ImageFontXPM(const std::string& filename, unsigned char glyphsFrom,
                         unsigned char glyphsTo)
    {
        mFilename = filename;
        mImage = Image::load(filename, false);

        Color separator = mImage->getPixel(0, 0);

        int i = 0;
        for (i=0; separator == mImage->getPixel(i, 0)
                 && i < mImage->getWidth(); ++i)
        {
        }

        if (i >= mImage->getWidth())
        {
            throw GCN_EXCEPTION("Corrupt image.");
        }

        int j = 0;
        for (j = 0; j < mImage->getHeight(); ++j)
        {
            if (separator == mImage->getPixel(i, j))
            {
                break;
            }
        }

        mHeight = j;
        int x = 0, y = 0;
//        unsigned char k;

        for (i=glyphsFrom; i<glyphsTo+1; i++)
        {
            addGlyph(i, x, y, separator);
        }

        mImage->convertToDisplayFormat();

        mRowSpacing = 0;
        mGlyphSpacing = 0;
    }

    ImageFontXPM::~ImageFontXPM()
    {
        delete mImage;
    }

    int ImageFontXPM::getWidth(unsigned char glyph) const
    {
        if (mGlyph[glyph].width == 0)
        {
            return mGlyph[(int)(' ')].width + mGlyphSpacing;
        }

        return mGlyph[glyph].width + mGlyphSpacing;
    }

    int ImageFontXPM::getHeight() const
    {
        return mHeight + mRowSpacing;
    }

    int ImageFontXPM::drawGlyph(Graphics* graphics, unsigned char glyph,
                             int x, int y)
    {
        // This is needed for drawing the Glyph in the middle if we have spacing
        int yoffset = getRowSpacing() >> 1;

        if (mGlyph[glyph].width == 0)
        {
            graphics->drawRectangle(Rectangle(x, y + 1 + yoffset,
                                              mGlyph[(int)(' ')].width - 1,
                                              mGlyph[(int)(' ')].height - 2));

            return mGlyph[(int)(' ')].width + mGlyphSpacing;
        }

        graphics->drawImage(mImage, mGlyph[glyph].x, mGlyph[glyph].y, x,
                            y + yoffset, mGlyph[glyph].width,
                            mGlyph[glyph].height);

        return mGlyph[glyph].width + mGlyphSpacing;
    }

    void ImageFontXPM::drawString(Graphics* graphics, const std::string& text,
                               int x, int y)
    {
        unsigned int i;

        for (i = 0; i< text.size(); ++i)
        {
            drawGlyph(graphics, text.at(i), x, y);
            x += getWidth(text.at(i));
        }
    }

    void ImageFontXPM::setRowSpacing(int spacing)
    {
        mRowSpacing = spacing;
    }

    int ImageFontXPM::getRowSpacing()
    {
        return mRowSpacing;
    }

    void ImageFontXPM::setGlyphSpacing(int spacing)
    {
        mGlyphSpacing = spacing;
    }

    int ImageFontXPM::getGlyphSpacing()
    {
        return mGlyphSpacing;
    }

    void ImageFontXPM::addGlyph(unsigned char c, int &x,
                             int &y, const Color& separator)
    {
        Color color;
        do
        {
            ++x;

            if (x >= mImage->getWidth())
            {
                y += mHeight + 1;
                x = 0;

                if (y >= mImage->getHeight())
                {
                    std::string str;
                    std::ostringstream os(str);
                    os << "Image ";
                    os << mFilename;
                    os << " with font is corrupt near character '";
                    os << c;
                    os << "'";
                    throw GCN_EXCEPTION(os.str());
                }
            }

            color = mImage->getPixel(x, y);

        } while (color == separator);

        int w = 0;

        do
        {
            ++w;

            if (x+w >= mImage->getWidth())
            {
                std::string str;
                std::ostringstream os(str);
                os << "Image ";
                os << mFilename;
                os << " with font is corrupt near character '";
                os << c;
                os << "'";
                throw GCN_EXCEPTION(os.str());
            }

            color = mImage->getPixel(x + w, y);

        } while (color != separator);

        mGlyph[c] = Rectangle(x, y, w, mHeight);

        x += w;
    }

    int ImageFontXPM::getWidth(const std::string& text) const
    {
        unsigned int i;
        int size = 0;

        for (i = 0; i < text.size(); ++i)
        {
            size += getWidth(text.at(i));
        }

        return size;
    }

    int ImageFontXPM::getStringIndexAt(const std::string& text, int x)
    {
        unsigned int i;
        int size = 0;

        for (i = 0; i < text.size(); ++i)
        {
            size += getWidth(text.at(i));

            if (size > x)
            {
                return i;
            }
        }

        return text.size();
    }
}
