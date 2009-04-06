// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2009 Simon Goodall

#ifndef SEAR_GUICHAN_IMAGEBOX_H
#define SEAR_GUICHAN_IMAGEBOX_H

#include "renderers/RenderTypes.h"

#include <guichan.hpp>

namespace Sear {

class ImageBox : public gcn::Widget {
public:
  ImageBox(const std::string &texture_name);
    
  virtual ~ImageBox();

  virtual void logic();
  
  virtual void draw(gcn::Graphics *);

  virtual void setRotation(float angle) { m_angle = angle; }
  virtual void setTextureName(const std::string &texture_name);
  virtual void setText(const std::string &text) { m_text = text; }
protected:
  std::string m_texture_name;
  std::string m_text;
  TextureID m_texture_id;
  float m_angle;
};

} // of namespace Sear

#endif // SEAR_GUICHAN_IMAGEBOX_H
