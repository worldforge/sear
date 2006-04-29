// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2006 Simon Goodall, University of Southampton

//$Id: Model.h,v 1.12 2006-04-29 13:11:29 alriddoch Exp $

#ifndef SEAR_MODEL_H
#define SEAR_MODEL_H 1

#include <map>
#include <string>

#include <Atlas/Message/Element.h>

#include <wfmath/vector.h>
#include <wfmath/quaternion.h>

#include "renderers/RenderTypes.h"

namespace Sear {

typedef struct {
    WFMath::Vector<3> pos;
    WFMath::Quaternion orient;
} PosAndOrient;

class Model {
public:
  Model() : m_last_time(0) {}
  virtual ~Model() {}
//  virtual int init() = 0;
  virtual int shutdown() = 0;
  virtual bool isInitialised() const = 0;

  /** The contextCreated function is called whenever the graphics context is
   * created. Typically this function will do nothing as lazy resource
   * allocation is prefered.
   */
  virtual void contextCreated() = 0;

  /** The contextDestroyed function is called whenever the graphics context is
   * destroyed or the object needs to clean up its context handles (e.g. when 
   * the model is destroyed). The parameter check is used to determine whether
   * to release the context resources or just reset the internal pointers. When
   * the context is destroyed there is no need to try and free the data.
   * @param check Flag indicating to relase GL resources before resetting them.
   */
  virtual void contextDestroyed(bool check) = 0;

  /** This function tells the model that XX seconds have passed since the last
   * frame. It is primarilty meant to be used to update animations. 
   * @param time_elapsed The time in seconds since the last frame.
   */
  virtual void update (float time_elapsed) {}

  /** The render function is called to render the object. Geometric transforms
   * from the server and model_record will already have been performed by this
   * stage. The OpenGL states will also have been set. Any state changes 
   * performed in this function need to be reset before returning. If the 
   * select_mode parameter is set, then objects need to be rendered a solid 
   * colour that is set before the render function is called.
   * @param select_mode Flag indicating that objects are to be rendered for 
   * selection
   */
  virtual void render(bool select_mode) {}

  /** The setDetailLevel function is intended to tell the model to adjust it's
   * detail level (e.g. number of polygons/ fancy effects) and will be based on
   * current frame rate and distance from camera. 
   * Currently not in use.
   * @param f Floating point number in the range [0.0:1.0] with 1.0 indicating
   * maximum detail.
   */
  virtual void setDetailLevel(float f) {}

  /** The action method is used to specify a single shot animation. The values
   * for action are the names of Action operations sent by the server. Typically
   * these could be "pickup", "drop", "touch", "attack" and "collapse".
   * @param action The name of the animation to perform
   */
  virtual void action(const std::string &action) {}

  /** The animate method is used to specify the name of a cyclic animation. The
   * values for animate are those specified in the mode attriute of an entity.
   * Typical values are "standing", "idle", "walking" and "running".
   * @param action The name of the animation to perform.
   */
  virtual void animate(const std::string &action) {}

  /** The setAppearance method is used to pass an Atlas Map object containing
   * data about the appearance of the object. This should be used to select
   * appropriate meshes and textures for the model.
   * @param mt The map containing appearance data.
   */
  virtual void setAppearance(const Atlas::Message::MapType &mt) {}

  /** This function returns a value to tell the renderer how the model should
   * be rotated. The main point of this function is for billboard style models
   * that need to be rotated to face the camera.
   * @return The rotation style.
   */  
  virtual RotationStyle rotationStyle() { return ROS_NONE; }

  /** The getLastTime function returns the last time value set by the 
   * setLastTime function. This is the time the model was last rendered.
   * @return The last time the model was rendered.
   */
  float getLastTime() const { return m_last_time; }

  /** The setLastTime function is used to set the time that the model was last 
   * rendered. This is used to unload a model if it has not been rendered for a
   * certain amount of time.
   * @param t The time in seconds.
   */
  void setLastTime(float t) { m_last_time = t; }
  
  /** The getPositionForSubmodel function is used to return the position and
   * orientation of a point in space for the given name. E.g. "right hand" 
   * should return the position and orientation of the right hand in relation
   * to the rest of the model. This information is used for attaching other
   * models to this one (e.g. tools).
   * @param submodelName Name of the sub section to get position of.
   * @return A struct containing position and orientation.
   */
  virtual PosAndOrient getPositionForSubmodel(const std::string& submodelName);
  
protected: 
  float m_last_time;
  
};

} /* namespace Sear */

#endif /* SEAR_MODEL_H */
