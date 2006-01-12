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

  ActionListenerSigC * buttonListener = new ActionListenerSigC;
  buttonListener->Action.connect(SigC::slot(*this, &VideoOptions::actionPressed));

  gcn::Box * vbox = new gcn::VBox(6);

  vbox->pack(new gcn::Label("Video Modes"));

  m_resolutionList = new ResolutionListModel;

  m_resolutions = new gcn::ListBox(m_resolutionList);
  m_resolutions->setWidth(100);
  m_resolutions->setFocusable(false);
  gcn::ScrollArea * scroll_area = new gcn::ScrollArea(m_resolutions,
                                      gcn::ScrollArea::SHOW_NEVER,
                                      gcn::ScrollArea::SHOW_ALWAYS);
  scroll_area->setWidth(100);
  scroll_area->setHeight(100);
  scroll_area->setBorderSize(1);
  vbox->pack(scroll_area);

  m_fullCheck = new gcn::CheckBox("Full screen");
  m_fullCheck->setFocusable(false);
  m_fullCheck->addActionListener(this);
  vbox->pack(m_fullCheck);

  gcn::Box * hbox = new gcn::HBox(6);

  gcn::Button * b = new gcn::Button("Apply");
  b->setEventId("apply");
  b->setFocusable(false);
  b->addActionListener(buttonListener);
  hbox->pack(b);

  b = new gcn::Button("Close");
  b->setEventId("close");
  b->setFocusable(false);
  b->addActionListener(buttonListener);
  hbox->pack(b);

  vbox->pack(hbox);

  setContent(vbox);

  resizeToContent();
}

VideoOptions::~VideoOptions()
{
}

void VideoOptions::actionPressed(std::string event)
{
  if (event == "apply") {
    Render * render = RenderSystem::getInstance().getRenderer();
    GL * gl_render = dynamic_cast<GL *>(render);

    if (gl_render != 0) {
      if (m_fullCheck->isMarked() != gl_render->isFullScreen()) {
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
    m_fullCheck->setMarked(gl_render->isFullScreen());
  }
  
  gcn::Window::logic();
}

void VideoOptions::action(const std::string &)
{
  m_checkFullChanged = true;
}

} // namespace Sear
