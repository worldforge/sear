// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2005 Alistair Riddoch

#include "guichan/VideoOptions.h"

#include "guichan/ActionListenerSigC.h"
#include "guichan/RootWidget.h"
#include "guichan/box.hpp"

#include "renderers/GL.h"
#include "renderers/RenderSystem.h"

#include "src/System.h"

#include "common/compose.hpp"

#include <guichan.hpp>

#include <sigc++/bind.h>
#include <sigc++/hide.h>
#include <sigc++/object_slot.h>

#include <SDL/SDL.h>

#include <iostream>

namespace Sear {

class ResolutionListModel : public gcn::ListModel
{
protected:
  SDL_Rect ** videoModes;
public:
  ResolutionListModel() : videoModes(0)
  {
    Render * render = RenderSystem::getInstance().getRenderer();
    GL * gl_render = dynamic_cast<GL *>(render);
    if (gl_render == 0) {
      return;
    }
    videoModes = SDL_ListModes(NULL, SDL_FULLSCREEN|SDL_HWSURFACE);
  }

  virtual ~ResolutionListModel() {
  }

  virtual int getNumberOfElements()
  {
    if (videoModes == 0) {
      return 0;
    }
    int i;
    for (i = 0; videoModes[i]; ++i);
    return i;
  }

  virtual std::string getElementAt(int n)
  {
    if (videoModes == 0) {
      return "NONE";
    }
    int i;
    for (i = 0; videoModes[i]; ++i) {
      if (i == n) {
        return String::compose("%1x%2", videoModes[i]->w, videoModes[i]->h);
      }
    }
    return "ERROR";
  }

  int setVideoMode(int n)
  {
    if (videoModes == 0) {
      return -1;
    }
    int i;
    for (i = 0; videoModes[i]; ++i) {
      if (i == n) {
        System::instance()->resizeScreen(videoModes[i]->w, videoModes[i]->h);
      }
    }
    return -1;
  }
};

VideoOptions::VideoOptions(RootWidget * top) : gcn::Window("video"), m_top(top),
                                               m_checkFullChanged(false)
{
  gcn::Color base = getBaseColor();
  base.a = 128;
  setBaseColor(base);

  setOpaque(true);

  m_buttonListener = new ActionListenerSigC;
  m_buttonListener->Action.connect(sigc::mem_fun(*this, &VideoOptions::actionPressed));

  gcn::Box * vbox = new gcn::VBox(6);
  m_widgets.push_back(SPtr<gcn::Widget>(vbox));

  gcn::Label *l1 = new gcn::Label("Video Modes");
  m_widgets.push_back(SPtr<gcn::Widget>(l1));
  vbox->pack(l1);

  m_resolutionList = new ResolutionListModel;

  m_resolutions = new gcn::ListBox(m_resolutionList);
  m_widgets.push_back(SPtr<gcn::Widget>(m_resolutions));
  
  m_resolutions->setWidth(100);
  m_resolutions->setFocusable(false);
  gcn::ScrollArea * scroll_area = new gcn::ScrollArea(m_resolutions,
                                      gcn::ScrollArea::SHOW_NEVER,
                                      gcn::ScrollArea::SHOW_ALWAYS);
  m_widgets.push_back(SPtr<gcn::Widget>(scroll_area));
  scroll_area->setWidth(100);
  scroll_area->setHeight(100);
  scroll_area->setFrameSize(1);
  vbox->pack(scroll_area);

  m_fullCheck = new gcn::CheckBox("Full screen");
  m_widgets.push_back(SPtr<gcn::Widget>(m_fullCheck));
  m_fullCheck->setFocusable(false);
  m_fullCheck->addActionListener(this);
  vbox->pack(m_fullCheck);

  gcn::Box * hbox = new gcn::HBox(6);
  m_widgets.push_back(SPtr<gcn::Widget>(hbox));

  gcn::Button * b = new gcn::Button("Apply");
  m_widgets.push_back(SPtr<gcn::Widget>(b));
  b->setActionEventId("apply");
  b->setFocusable(false);
  b->addActionListener(m_buttonListener);
  hbox->pack(b);

  b = new gcn::Button("Close");
  m_widgets.push_back(SPtr<gcn::Widget>(b));
  b->setActionEventId("close");
  b->setFocusable(false);
  b->addActionListener(m_buttonListener);
  hbox->pack(b);

  vbox->pack(hbox);

  add(vbox);

  resizeToContent();
}

VideoOptions::~VideoOptions()
{
  delete m_buttonListener;
  delete m_resolutionList;
}

void VideoOptions::actionPressed(std::string event)
{
  if (event == "apply") {
    Render * render = RenderSystem::getInstance().getRenderer();
    GL * gl_render = dynamic_cast<GL *>(render);

    if (gl_render != 0) {
      if (m_fullCheck->isSelected() != gl_render->isFullScreen()) {
        gl_render->toggleFullscreen();
      }

      int sel = m_resolutions->getSelected();
      std::cout << sel << std::endl << std::flush;
      m_resolutionList->setVideoMode(sel);
    }

    m_checkFullChanged = false;
    std::cout << "Apply changes" << std::endl << std::flush;
  } else if (event == "close") {
    m_top->closeWindow(this);
  } else {
    std::cout << "Say what?" << std::endl << std::flush;
  }
}

void VideoOptions::logic()
{
  Render * render = RenderSystem::getInstance().getRenderer();
  GL * gl_render = dynamic_cast<GL *>(render);

  if (gl_render != 0 && !m_checkFullChanged) {
    m_fullCheck->setSelected(gl_render->isFullScreen());
  }
  
  gcn::Window::logic();
}

void VideoOptions::action(const gcn::ActionEvent &actionEvent)
{
  m_checkFullChanged = true;
}

} // namespace Sear
