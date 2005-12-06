// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2005 Alistair Riddoch

#include "guichan/SpeechBubble.h"

#include <iostream>

SpeechBubble::SpeechBubble() : m_overflow(false), m_offset(0.f)
{
    setWidth(200);
    setHeight(200);

    m_lines.push_back("The cat sat on the matte is a very very gelical cat.");
    m_lines.push_back("The boy stood on the burning deck when all but he had fled.");
    m_lines.push_back("The quick brown fox jumped over the lazy dog.");
    m_lines.push_back("All work and no play make jack a dull boy.");
    m_lines.push_back("Richard of York gave battle in vain.");
    m_lines.push_back("To be or not to be, that is the question.");
    m_lines.push_back("Now is the winter of our discontent.");
    m_lines.push_back("There would have been a time for such a deed, tomorrow and tomorrow and tomorrow.");
}

void SpeechBubble::logic()
{
    if (m_overflow) {
        m_offset += 0.1f;
        m_overflow = false;
    }
}

void SpeechBubble::draw(gcn::Graphics * graphics)
{
    graphics->drawImage(m_nw, 0, 0);
    graphics->drawImage(m_ne, getWidth() - m_ne->getWidth(), 0);
    graphics->drawImage(m_se, getWidth() - m_se->getWidth(), getHeight() - m_se->getHeight());
    graphics->drawImage(m_sw, 0, getHeight() - m_se->getHeight());

    // FIXME Need to deal if its bigger than the graphics can handle
    graphics->drawImage(m_n, 0, 0, m_nw->getWidth(), 0, getWidth() - m_nw->getWidth() - m_ne->getWidth(), m_n->getHeight());
    graphics->drawImage(m_s, 0, 0, m_sw->getWidth(), getHeight() - m_s->getHeight(), getWidth() - m_sw->getWidth() - m_se->getWidth(), m_s->getHeight());
    graphics->drawImage(m_w, 0, 0, 0, m_nw->getHeight(), m_w->getWidth(), getHeight() - m_nw->getHeight() - m_sw->getHeight());
    graphics->drawImage(m_e, 0, 0, getWidth() - m_e->getWidth(), m_ne->getHeight(), m_e->getWidth(), getHeight() - m_ne->getHeight() - m_se->getHeight());
    graphics->drawImage(m_mid, 0, 0, m_w->getWidth(), m_n->getHeight(), getWidth() - m_w->getWidth() - m_e->getWidth(), getHeight() - m_n->getHeight() - m_s->getHeight());


    graphics->setColor(getForegroundColor());
    graphics->setFont(getFont());

    int line_no = 0;
    int obsolete_lines = 0;
    int obsolete_offset = 0;
    std::list<std::string>::const_iterator I = m_lines.begin();
    std::list<std::string>::const_iterator Iend = m_lines.end();
    for (; I != Iend; ++I) {
        const std::string & text = *I;
        for (unsigned i = 1, s = 0; i <= text.size(); ++i) {
            if ((line_no + 1) * getFont()->getHeight() - m_offset> (getHeight() - m_n->getHeight() - m_n->getHeight())) {
                m_overflow = true;
                break;
            }
            int line_offset = getFont()->getHeight() * line_no - m_offset;
            if (getFont()->getWidth(text.substr(s, (i - s))) > (getWidth() - m_w->getWidth() - m_e->getWidth())) {
                std::string line;
                std::string::size_type space = text.find_last_of(" ", i, 1);
                if (space > s) {
                    line = text.substr(s, space - s);
                    s = space + 1;
                } else {
                    line = text.substr(s, i - s - 1);
                    s = i - 1;
                }
                if (line_offset >= 0) {
                    graphics->drawText(line, m_w->getWidth(), m_n->getHeight() + line_offset);
                }
                ++line_no;
                line_offset = getFont()->getHeight() * line_no - m_offset;
            }
            if (i == text.size()) {
                if (line_offset >= 0) {
                    graphics->drawText(text.substr(s, i - s), m_w->getWidth(), m_n->getHeight() + getFont()->getHeight() * line_no - m_offset);
                } else {
                    ++obsolete_lines;
                    obsolete_offset = getFont()->getHeight() * (line_no + 1);
                }
                ++line_no;
            }
        }
    }
    for (int i = 0; i < obsolete_lines; ++i) {
        m_lines.pop_front();
    }
    m_offset -= obsolete_offset;
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
    return 0;
}
