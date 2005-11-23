// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2005 Alistair Riddoch

#include "guichan/SpeechBubble.h"

#include <iostream>

SpeechBubble::SpeechBubble()
{
    setWidth(200);
    setHeight(200);
}

void SpeechBubble::draw(gcn::Graphics * graphics)
{
    graphics->drawImage(m_nw, 0, 0);
    graphics->drawImage(m_ne, getWidth() - m_ne->getWidth(), 0);
    graphics->drawImage(m_se, getWidth() - m_se->getWidth(), getHeight() - m_se->getHeight());
    graphics->drawImage(m_sw, 0, getHeight() - m_se->getHeight());

    graphics->drawImage(m_n, 0, 0, m_nw->getWidth(), 0, getWidth() - m_nw->getWidth() - m_ne->getWidth(), m_n->getHeight());
    graphics->drawImage(m_s, 0, 0, m_sw->getWidth(), getHeight() - m_s->getHeight(), getWidth() - m_sw->getWidth() - m_se->getWidth(), m_s->getHeight());
    graphics->drawImage(m_w, 0, 0, 0, m_nw->getHeight(), m_w->getWidth(), getHeight() - m_nw->getHeight() - m_sw->getHeight());
    graphics->drawImage(m_e, 0, 0, getWidth() - m_e->getWidth(), m_ne->getHeight(), m_e->getWidth(), getHeight() - m_ne->getHeight() - m_se->getHeight());
    graphics->drawImage(m_mid, 0, 0, m_w->getWidth(), m_n->getHeight(), getWidth() - m_w->getWidth() - m_e->getWidth(), getHeight() - m_n->getHeight() - m_s->getHeight());
}

void SpeechBubble::drawBorder(gcn::Graphics * graphics)
{
    // Do we have a border for this? Why abstract?
}

int SpeechBubble::loadImages(const std::vector<std::string> & filenames)
{
#if 0
    if (filenames.size() != 9) {
        std::cerr << "Not enough filenames" << std::endl << std::flush;
        return -1;
    }
    m_n = new gcn::Image(filenames[0]);
    m_s = new gcn::Image(filenames[1]);
    m_e = new gcn::Image(filenames[2]);
    m_w = new gcn::Image(filenames[3]);
    m_ne = new gcn::Image(filenames[4]);
    m_se = new gcn::Image(filenames[5]);
    m_sw = new gcn::Image(filenames[6]);
    m_nw = new gcn::Image(filenames[7]);
    m_mid = new gcn::Image(filenames[8]);
#else 
    std::cout << "Loading images" << std::endl << std::flush;

    m_n = new gcn::Image("/home/ajr/sear/bubble_n.png");
    m_s = new gcn::Image("/home/ajr/sear/bubble_s.png");
    m_e = new gcn::Image("/home/ajr/sear/bubble_e.png");
    m_w = new gcn::Image("/home/ajr/sear/bubble_w.png");
    m_ne = new gcn::Image("/home/ajr/sear/bubble_ne.png");
    m_se = new gcn::Image("/home/ajr/sear/bubble_se.png");
    m_sw = new gcn::Image("/home/ajr/sear/bubble_sw.png");
    m_nw = new gcn::Image("/home/ajr/sear/bubble_nw.png");
    m_mid = new gcn::Image("/home/ajr/sear/bubble_mid.png");
#endif
}
