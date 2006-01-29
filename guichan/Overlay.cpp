// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2006 Alistair Riddoch

#include "Overlay.h"
#include "SpeechBubble.h"
#include "StatusWindow.h"
#include "RootWidget.h"

#include "renderers/Render.h"
#include "renderers/RenderSystem.h"

#include "src/System.h"
#include "src/client.h"

#include <Eris/Avatar.h>
#include <Eris/View.h>

#include <sigc++/object_slot.h>

namespace Sear {

Overlay * Overlay::m_instance = 0;

Overlay::Overlay() : m_top(0), m_selfStatus(0), m_selectionStatus(0)
{

}

Overlay::~Overlay()
{
}

void Overlay::logic(RootWidget * rw)
{
  Eris::Avatar * avatar = System::instance()->getClient()->getAvatar();
  Render * render = RenderSystem::getInstance().getRenderer();
  if (avatar == 0 || avatar->getEntity() == 0) {
    if (m_top != 0) {
      // If this overlay is initialised, clean it up.
      m_top->remove(m_selfStatus);
      delete m_selfStatus;
      if (m_selectionStatus != 0) {
        m_top->remove(m_selectionStatus);
        delete m_selectionStatus;
        m_selectionStatus = 0;
      }
      // Clean up any bubbles.
      m_top = 0;
    }
    return;
  }

  if (m_top == 0) {
    avatar->Hear.connect(SigC::slot(*this, &Overlay::heard));
    m_top = rw;
    std::cout << "Overlay init" << std::endl << std::flush;

    m_selfStatus = new StatusWindow(avatar->getEntity());
    m_top->setWindowCoords(m_selfStatus, std::make_pair(render->getWindowWidth() - m_selfStatus->getWidth(), 0));
    m_top->openWindow(m_selfStatus);
  }

  std::string selection_id = render->getActiveID();
  Eris::Entity * selection_ent = avatar->getView()->getEntity(selection_id);

  if (selection_ent != m_selection.get() || !m_selection) {
    if (m_selectionStatus != 0) {
      m_top->remove(m_selectionStatus);
      delete m_selectionStatus;
      m_selectionStatus = 0;
    }
    m_selection = selection_ent;
    if (selection_ent != 0) {
      m_selectionStatus = new StatusWindow(selection_ent);
      m_top->add(m_selectionStatus, render->getWindowWidth() - m_selfStatus->getWidth() - 4 - m_selectionStatus->getWidth(), 0);
    }
  }

  double elapsed = System::instance()->getTimeElapsed();

  BubbleMap::const_iterator I = m_bubbles.begin();
  BubbleMap::const_iterator Iend = m_bubbles.end();
  for (; I != Iend; ++I) {
    int x = I->second->getX();
    int y = I->second->getY();
    BubbleMap::const_iterator J = m_bubbles.begin();
    for (; J != Iend; ++J) {
      if (I->second == J->second) { continue; }
      int ox = J->second->getX();
      int oy = J->second->getY();
      if (abs(x - ox) < I->second->getWidth()) {
        if (x >= ox) {
          x = std::min(int(x + 64 * elapsed), m_top->getWidth() - I->second->getWidth());
        } else {
          x = std::max(int(x - 64 * elapsed), 0);
        }
        I->second->setX(x);
      }
    }
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
