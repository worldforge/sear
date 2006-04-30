// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2006 Alistair Riddoch

#include "Overlay.h"
#include "SpeechBubble.h"
#include "StatusWindow.h"
#include "TaskWindow.h"
#include "RootWidget.h"

#include "renderers/Render.h"
#include "renderers/RenderSystem.h"

#include "src/System.h"
#include "src/WorldEntity.h"
#include "src/client.h"

#include <Eris/Avatar.h>
#include <Eris/View.h>

#include <sigc++/object_slot.h>

namespace Sear {

SPtr<Overlay> Overlay::m_instance;

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

    m_selfStatus = new StatusWindow(avatar->getEntity());
    m_top->setWindowCoords(m_selfStatus, std::make_pair(render->getWindowWidth() - m_selfStatus->getWidth(), 0));
    m_top->openWindow(m_selfStatus);

    m_selfTask = new TaskWindow(avatar->getEntity());
    m_top->setWindowCoords(m_selfTask, std::make_pair(render->getWindowWidth() - m_selfTask->getWidth(), m_selfStatus->getHeight()));
    m_top->openWindow(m_selfTask);
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

  std::set<WorldEntity *> obsoletes;

  BubbleMap::const_iterator I = m_bubbles.begin();
  BubbleMap::const_iterator Iend = m_bubbles.end();
  for (; I != Iend; ++I) {
    WorldEntity * we = dynamic_cast<WorldEntity *>(I->first);
    SpeechBubble * sb = I->second;
    if (we == NULL) {
      continue;
    }
    int ex = we->screenX();
    int ey = we->screenY();
    if (ey < 0 || ey >= m_top->getHeight() ||
        ex < 0 || ex >= m_top->getWidth() ||
        !we->isVisible()) {
      obsoletes.insert(we);
      // Obsolete speech bubble?
      continue;
    }
    ex = (ex - sb->getWidth() / 2);
    ey = (m_top->getHeight() - ey) - sb->getHeight();
    // sb->setX(ex + sb->m_xoff);
    sb->setY(ey);

    const int x = ex + (int)sb->m_xoff; // sb->getX();
    const int y = sb->getY();
    float xoff = sb->m_xoff;
    BubbleMap::const_iterator J = m_bubbles.begin();
    for (; J != Iend; ++J) {
      if (sb == J->second) { continue; }
      const int ox = J->second->getX();
      const int oy = J->second->getY();
      if (abs(y - oy) < sb->getHeight() && abs(x - ox) < sb->getWidth()) {
        if (x >= ox) {
          xoff += std::min(64.f * elapsed, (double)x - ox);
          // , (float)(m_top->getWidth() - sb->getWidth()));
        } else {
          xoff -= std::min(64.f * elapsed, (double)ox - x);
        }
      }
    }
    // Ensure the xoffset does not move the bubbles off the screen
    // Right hand side
    xoff = std::min(xoff, (float)(m_top->getWidth() - (ex + sb->getWidth())));
    // Left hand side
    xoff = std::max(xoff, (float)-ex);
    // Limit new xoffset to half bubble width
    xoff = std::min(xoff, sb->getWidth() / 2.f);
    xoff = std::max(xoff, -(sb->getWidth() / 2.f));
    sb->m_xoff = xoff;
    sb->setX(ex + (int)sb->m_xoff);
  }
  std::set<WorldEntity *>::const_iterator K = obsoletes.begin();
  std::set<WorldEntity *>::const_iterator Kend = obsoletes.end();
  for (; K != Kend; ++K) {
    BubbleMap::iterator J = m_bubbles.find(*K);
    if (J != m_bubbles.end()) {
      J->first->releaseScreenCoords();
      m_top->remove(J->second);
      delete J->second;
      m_bubbles.erase(J);
    }
  }
}

void Overlay::heard(Eris::Entity * e,
                    const Atlas::Objects::Operation::RootOperation & talk)
{
  assert(m_top != 0);

  Eris::Avatar * avatar = System::instance()->getClient()->getAvatar();
  assert(avatar != 0);
  if (e == avatar->getEntity()) {
    return;
  }

  WorldEntity * we = dynamic_cast<WorldEntity *>(e);
  if (we == NULL) {
    return;
  }
  SpeechBubble * bubble;
  BubbleMap::const_iterator I = m_bubbles.find(we);
  if (I == m_bubbles.end()) {
    bubble = new SpeechBubble;
    bubble->loadImages(std::vector<std::string>());
    m_top->add(bubble, 50, 50);
    m_bubbles[we] = bubble;
    we->requestScreenCoords();
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
