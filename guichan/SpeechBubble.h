// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2005 Alistair Riddoch

#ifndef SEAR_GUICHAN_SPEECH_BUBBLE_H
#define SEAR_GUICHAN_SPEECH_BUBBLE_H

#include <guichan/widget.hpp>

#include <vector>

namespace Sear {

class SpeechBubble: public gcn::Widget
{
public:
    SpeechBubble();

    virtual void logic();

    virtual void draw(gcn::Graphics * graphics);

    virtual void drawBorder(gcn::Graphics * graphics);

    void addLine(const std::string &);

    int loadImages(const std::vector<std::string> &);
private:
    gcn::Image * m_n;
    gcn::Image * m_s;
    gcn::Image * m_e;
    gcn::Image * m_w;
    gcn::Image * m_ne;
    gcn::Image * m_se;
    gcn::Image * m_sw;
    gcn::Image * m_nw;
    gcn::Image * m_mid;

    std::list<std::string> m_lines;

    bool m_overflow;
    float m_offset;
};

} // namespace Sear

#endif // SEAR_GUICHAN_SPEECH_BUBBLE_H
