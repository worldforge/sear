%module Sear 

%{

extern "C" {
  #include "lua.h"
  #include "lualib.h"
  #include "lauxlib.h"
}

#include <sigc++/sigc++.h>
#include <sigc++/connection.h>
#include <sigc++/trackable.h>
#include <guichan/actionlistener.hpp>
#include "Eris/Localserver.h"

#include "guichan/ActionImageBox.h"
#include "guichan/ActionListenerSigC.h"
#include "guichan/actiontextfield.h"
#include "guichan/adjustingcontainer.hpp"
#include "guichan/Alert.h"
#include "guichan/bar.hpp"
#include "guichan/box.hpp"
#include "guichan/CharacterListModel.h"
#include "guichan/CommandLine.h"
#include "guichan/DblClkListBox.h"
#include "guichan/grid.hpp"
#include "guichan/ImageBox.h"
#include "guichan/LogicWindow.h"
#include "guichan/passwordfield.h"
#include "guichan/ServerListModel.h"
#include "guichan/TypeListModel.h"
#include "guichan/Workarea.h"

#include "interfaces/ConsoleObject.h"
#include "src/ActionHandler.h"
#include "src/client.h"
#include "src/System.h"
#include "src/FileHandler.h"

%}

%include <lua_fnptr.i>
%include <std_string.i>


namespace sigc {

struct connection 
{
bool empty() const;
bool connected() const;
void disconnect();
};

}

%import "Guichan.i"
%import "Eris.i"

%{
  #include "luasigc.hpp"
%}



%include "interfaces/ConsoleObject.h"
%include "src/ActionHandler.h"
%include "src/client.h"

# Allow string refs to be used
%apply std::string& INOUT {std::string& str};
%include "src/FileHandler.h"

%include "src/System.h"
%include "Eris/Localserver.h"

%include "guichan/ActionImageBox.h"
%include "guichan/ActionListenerSigC.h"
%include "guichan/actiontextfield.h"
%include "guichan/adjustingcontainer.hpp"
%include "guichan/Alert.h"
%include "guichan/bar.hpp"
%include "guichan/box.hpp"
%include "guichan/CharacterListModel.h"
%include "guichan/CommandLine.h"
%include "guichan/DblClkListBox.h"
%include "guichan/grid.hpp"
%include "guichan/ImageBox.h"
%include "guichan/LogicWindow.h"
%include "guichan/passwordfield.h"
%include "guichan/ServerListModel.h"
%include "guichan/TypeListModel.h"
%include "guichan/Workarea.h"


%extend Sear::LogicWindow {
  sigc::connection LogicCB(lua_State *L, SWIGLUA_FN fn) {
    SWIGLUA_REF the_func={0,0};
    swiglua_ref_set(&the_func, L, lua_gettop(L));

    sigc::connection c( self->Logic.connect(sigc::bind(sigc::ptr_fun(Sear::callback0), L, the_func)));

    return c;
  }
}

%extend Sear::ActionListenerSigC {
  sigc::connection ActionCB(lua_State *L, SWIGLUA_FN fn) {
    SWIGLUA_REF the_func={0,0};
    swiglua_ref_set(&the_func, L, lua_gettop(L));

    sigc::connection c(self->Action.connect(sigc::bind(sigc::ptr_fun(Sear::callback1<std::string>), L, the_func)));

    return c;
  }
}
