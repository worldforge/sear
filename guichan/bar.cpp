/*      _______   __   __   __   ______   __   __   _______   __   __                 
 *     / _____/\ / /\ / /\ / /\ / ____/\ / /\ / /\ / ___  /\ /  |\/ /\                
 *    / /\____\// / // / // / // /\___\// /_// / // /\_/ / // , |/ / /                 
 *   / / /__   / / // / // / // / /    / ___  / // ___  / // /| ' / /                  
 *  / /_// /\ / /_// / // / // /_/_   / / // / // /\_/ / // / |  / /                   
 * /______/ //______/ //_/ //_____/\ /_/ //_/ //_/ //_/ //_/ /|_/ /                    
 * \______\/ \______\/ \_\/ \_____\/ \_\/ \_\/ \_\/ \_\/ \_\/ \_\/                      
 *
 * Copyright (c) 2004, 2005 darkbits                        Js_./
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

#include "guichan/graphics.hpp"
#include "guichan/bar.hpp"

namespace gcn
{
    Bar::Bar(double scaleEnd)
    {
        mScaleStart = 0;
        mScaleEnd = scaleEnd;
        
        setFrameSize(1);
        setOrientation(HORIZONTAL);
        setValue(0);
    }

    Bar::Bar(double scaleStart, double scaleEnd)
    {
        mScaleStart = scaleStart;
        mScaleEnd = scaleEnd;
        
        setFrameSize(1);
        setOrientation(HORIZONTAL);
        setValue(scaleStart);
    }

    void Bar::setScale(double scaleStart, double scaleEnd)
    {
        mScaleStart = scaleStart;
        mScaleEnd = scaleEnd;
    }

    double Bar::getScaleStart() const
    {
        return mScaleStart;
    }

    void Bar::setScaleStart(double scaleStart)
    {
        mScaleStart = scaleStart;
    }

    double Bar::getScaleEnd() const
    {
        return mScaleEnd;
    }

    void Bar::setScaleEnd(double scaleEnd)
    {
        mScaleEnd = scaleEnd;
    }

    void Bar::draw(gcn::Graphics* graphics)
    {
        Color shadowColor = getBaseColor() - 0x101010;
        int alpha = getBaseColor().a;        
        shadowColor.a = alpha;
                
        graphics->setColor(shadowColor);
        graphics->fillRectangle(gcn::Rectangle(0,0,getWidth(),getHeight()));
    
        drawMarker(graphics);
    }

    void Bar::drawBorder(gcn::Graphics* graphics)
    {
        Color faceColor = getBaseColor();
        Color highlightColor, shadowColor;
        int alpha = getBaseColor().a;
        int width = getWidth() + getFrameSize() * 2 - 1;
        int height = getHeight() + getFrameSize() * 2 - 1;
        highlightColor = faceColor + 0x303030;
        highlightColor.a = alpha;
        shadowColor = faceColor - 0x303030;
        shadowColor.a = alpha;
        
        unsigned int i;
        for (i = 0; i < getFrameSize(); ++i)
        {
            graphics->setColor(shadowColor);
            graphics->drawLine(i,i, width - i, i);
            graphics->drawLine(i,i + 1, i, height - i - 1);
            graphics->setColor(highlightColor);
            graphics->drawLine(width - i,i + 1, width - i, height - i); 
            graphics->drawLine(i,height - i, width - i - 1, height - i);
        }
    }
    
    void Bar::drawMarker(gcn::Graphics* graphics)
    {
        gcn::Color faceColor = getForegroundColor();
        Color highlightColor, shadowColor;
        int alpha = getForegroundColor().a;
        highlightColor = faceColor + 0x303030;
        highlightColor.a = alpha;
        shadowColor = faceColor - 0x303030;
        shadowColor.a = alpha;        
        
        graphics->setColor(faceColor);    

        if (getOrientation() == HORIZONTAL)
        {
            graphics->fillRectangle(gcn::Rectangle(1, 1, getPosition() - 2, getHeight() - 2));
            graphics->setColor(highlightColor);
            graphics->drawLine(0, 0, getPosition() - 1,0);
            graphics->drawLine(0, 0, 0, getHeight() - 1);
            graphics->setColor(shadowColor);
            graphics->drawLine(0 + getPosition() - 1, 1, getPosition() - 1, getHeight() - 1);
            graphics->drawLine(1, getHeight() - 1, getPosition() - 1, getHeight() - 1);

            if (isFocused())
            {
                graphics->setColor(getForegroundColor());
                graphics->drawRectangle(Rectangle(2, 2, getPosition() - 4, getHeight() - 4));
            }    
        }
        else
        {
            graphics->fillRectangle(gcn::Rectangle(1, 1, getWidth() - 2, getPosition() - 2));
            graphics->setColor(highlightColor);
            graphics->drawLine(0, 0, 0, getPosition() - 1);
            graphics->drawLine(0, 0, getWidth() - 1, 0);
            graphics->setColor(shadowColor);
            graphics->drawLine(1, getPosition() - 1, getWidth() - 1, getPosition() - 1);
            graphics->drawLine(getWidth() - 1, 1, getWidth() - 1, getPosition() - 1);

            if (isFocused())
            {
                graphics->setColor(getForegroundColor());
                graphics->drawRectangle(Rectangle(2, 2, getWidth() - 4, getPosition() - 4));
            }    
        }
    }
    
    void Bar::setValue(double value)
    {
        if (value > getScaleEnd())
        {
            mValue = getScaleEnd();
            return;
        }

        if (value < getScaleStart())
        {
            mValue = getScaleStart();
            return;
        }

        mValue = value;
    }
    
    double Bar::getValue() const
    {
        return mValue;
    }

    void Bar::setOrientation(unsigned int orientation)
    {
        mOrientation = orientation;    
    }

    unsigned int Bar::getOrientation() const
    {
        return mOrientation;
    }

#if 0
    double Bar::markerPositionToValue(int v) const
    {
        int w;
        if (getOrientation() == HORIZONTAL)
        {
            w = getWidth();
        }
        else
        {
            w = getHeight();
        }
    
        double pos = v / ((double)w - getMarkerLength());
        return (1.0 - pos) * getScaleStart() + pos * getScaleEnd();
    
    }
  
#endif
    int Bar::valueToPosition(double value) const
    {
        int v;
        if (getOrientation() == HORIZONTAL)
        {
            v = getWidth();
        }
        else
        {
            v = getHeight();
        }

        int w =  (int)(v * (value  - getScaleStart()) / (getScaleEnd() - getScaleStart()));
    
        if (w < 0)
        {
            return 0;
        }
      
        if (w > v)
        {
            return v;
        }
      
        return w;
    }

    int Bar::getPosition() const
    {
        return valueToPosition(getValue());
    }    
}
