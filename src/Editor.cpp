// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2004 - 2009 Simon Goodall

#include "Editor.h"
#include <Eris/Entity.h>
#include <Eris/View.h>
#include <Eris/Connection.h>
#include <Eris/Avatar.h>

#include <Atlas/Message/Element.h>
#include <wfmath/atlasconv.h>
#include <Atlas/Objects/Operation.h>
#include "Character.h"
#include "CharacterManager.h"
#include "client.h"
#include "System.h"
#include "renderers/RenderSystem.h"
#include "renderers/Render.h"


namespace Sear {

static const std::string EDIT_MOVE_X = "edit_move_local_x";
static const std::string EDIT_MOVE_Y = "edit_move_local_y";
static const std::string EDIT_MOVE_Z = "edit_move_local_z";

static const std::string EDIT_MOVE_LOC_X = "edit_move_x";
static const std::string EDIT_MOVE_LOC_Y = "edit_move_y";
static const std::string EDIT_MOVE_LOC_Z = "edit_move_z";

static const std::string EDIT_ROTATE_Z = "edit_rotate_z";

static const std::string EDIT_DELETE = "edit_delete";

void Editor::registerCommands(Console *console)
{
    console->registerCommand(EDIT_MOVE_X, this);
    console->registerCommand(EDIT_MOVE_Y, this);
    console->registerCommand(EDIT_MOVE_Z, this);
    
    console->registerCommand(EDIT_MOVE_LOC_X, this);
    console->registerCommand(EDIT_MOVE_LOC_Y, this);
    console->registerCommand(EDIT_MOVE_LOC_Z, this);
    
    console->registerCommand(EDIT_ROTATE_Z, this);
    console->registerCommand(EDIT_DELETE, this);
}

void Editor::runCommand(const std::string &command, const std::string &args)
{
    std::string target = RenderSystem::getInstance().getRenderer()->getActiveID();
  
    if (command == EDIT_MOVE_X) {
        WFMath::Vector<3> delta(::strtod(args.c_str(), NULL),0,0);
        translate(target, delta, true);
    }
    
    if (command == EDIT_MOVE_Y) {
        WFMath::Vector<3> delta(0,::strtod(args.c_str(), NULL),0);
        translate(target, delta, true);
    }
    
    if (command == EDIT_MOVE_Z) {
        WFMath::Vector<3> delta(0,0,::strtod(args.c_str(), NULL));
        translate(target, delta, true);
    }

    if (command == EDIT_MOVE_LOC_X) {
        WFMath::Vector<3> delta(::strtod(args.c_str(), NULL),0,0);
        translate(target, delta, false);
    }
    
    if (command == EDIT_MOVE_LOC_Y) {
        WFMath::Vector<3> delta(0,::strtod(args.c_str(), NULL),0);
        translate(target, delta, false);
    }
    
    if (command == EDIT_MOVE_LOC_Z) {
        WFMath::Vector<3> delta(0,0,::strtod(args.c_str(), NULL));
        translate(target, delta, false);
    }
    
    if (command == EDIT_ROTATE_Z) {
        rotateZ(target, ::strtod(args.c_str(), NULL));
    }
    
    if (command == EDIT_DELETE)
        deleteEntity(target);
}

void Editor::translate(const std::string& target, WFMath::Vector<3> delta, bool local)
{

    Eris::EntityPtr te = getAvatar()->getView()->getEntity(target);
    if (!te) return; // odd
    Eris::EntityPtr loc = te->getLocation();
    if (!loc) return;
    
    Atlas::Objects::Operation::Move move;
    move->setFrom(getAvatar()->getId());
    move->setTo(te->getId());
    
    if (local) {
        delta = te->toLocationCoords(delta);
        std::cout << "making vector local" << delta << std::endl;
    }
    
    Atlas::Message::MapType msg;
    WFMath::Point<3> pos = te->getPosition() + delta;
    msg["pos"] = pos.toAtlas();
    msg["id"] = te->getId();
    msg["loc"] = loc->getId();
    move->setArgsAsList(Atlas::Message::ListType(1, msg));
      
    getAvatar()->getConnection()->send(move);
}

void Editor::rotateZ(const std::string& target, double delta)
{
    Eris::EntityPtr te = getAvatar()->getView()->getEntity(target);
    if (!te) return; // odd
    Eris::EntityPtr loc = te->getLocation();
    if (!loc) return;
    
    Atlas::Objects::Operation::Move move;
    move->setFrom(getAvatar()->getId());
    move->setTo(te->getId());
    
    Atlas::Message::MapType msg;
    WFMath::Quaternion q = te->getOrientation() * WFMath::Quaternion(WFMath::Vector<3>(0.0, 0.0, 1.0), delta);
    
    msg["orientation"] = q.toAtlas();
    msg["pos"] = te->getPosition().toAtlas();    
    msg["loc"] = loc->getId();
    msg["id"] = te->getId();
    move->setArgsAsList(Atlas::Message::ListType(1, msg));
      
    getAvatar()->getConnection()->send(move);
}

void Editor::deleteEntity(const std::string& target)
{
    Eris::EntityPtr te = getAvatar()->getView()->getEntity(target);
    if (!te) return; // odd
    
    Atlas::Objects::Operation::Delete del;
    del->setFrom(getAvatar()->getId());
    del->setTo(te->getId());
    
    Atlas::Message::MapType msg;
    msg["id"] = te->getId();
    del->setArgsAsList(Atlas::Message::ListType(1, msg));
      
    getAvatar()->getConnection()->send(del);
}

Eris::Avatar* Editor::getAvatar() const
{
    return System::instance()->getCharacterManager()->getActiveCharacter()->getAvatar();
}

} // of namespace Sear

