// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2006 Alistair Riddoch

#include "Overlay.h"
#include "SpeechBubble.h"
#include "RootWidget.h"

#include "src/System.h"
#include "src/client.h"

#include <Eris/Avatar.h>

#include <sigc++/object_slot.h>

namespace Sear {

Overlay * Overlay::m_instance = 0;

Overlay::Overlay() : m_top(0)
{

}

Overlay::~Overlay()
{
}

void Overlay::logic(RootWidget * rw)
{
  if (m_top == 0) {
    Eris::Avatar * avatar = System::instance()->getClient()->getAvatar();
    if (avatar == 0) {
      return;
    }
    avatar->Hear.connect(SigC::slot(*this, &Overlay::heard));
    m_top = rw;
    std::cout << "Overlay init" << std::endl << std::flush;
  }
  
}

void Overlay::heard(Eris::Entity * e,
                    const Atlas::Objects::Operation::RootOperation & talk)
{
  assert(m_top != 0);
    std::cout << "Overlay talk" << std::endl << std::flush;

  SpeechBubble * bubble;
  BubbleMap::const_iterator I = m_bubbles.find(e);
  if (I == m_bubbles.end()) {
    bubble = new SpeechBubble;
    bubble->loadImages(std::vector<std::string>());
    m_top->add(bubble, 50, 50);
    m_bubbles[e] = bubble;
  } else {
    bubble = I->second;
  }
  const std::vector<Atlas::Objects::Root> & talkArgs = talk->getArgs();
  if (talkArgs.empty())
  {
    return;
  }
  const Atlas::Objects::Root & talkArg = talkArgs.front();
  if (!talkArg->hasAttr("say")) {
    printf("Error: Talk but no 'say'\n");
    return;
  }
  std::string msg = talkArg->getAttr("say").asString();
  bubble->addLine(msg);
}

} // namespace Sear
