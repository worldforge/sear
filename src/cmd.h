// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2002 Simon Goodall, University of Southampton

#ifndef _CMD_H_
#define _CMD_H_ 1

//Constants used in commands
#define CMD_CONNECT "connect"
#define CMD_DISCONNECT "disconnect"
#define CMD_RECONNECT "reconnect"
#define CMD_ACCOUNT_CREATE "create"
#define CMD_ACCOUNT_LOGIN "login"
#define CMD_ACCOUNT_LOGOUT "logout"
#define CMD_CHARACTER_CREATE "add"
#define CMD_CHARACTER_TAKE "take"
#define CMD_CHARACTER_LIST "get"

#define CMD_ROOM_LIST "rooms"

#define CMD_WORLD_LIST "list"

#define CMD_STATUS "status"

#define CMD_CHAR_MOVE_FORW   "+charforward"
#define CMD_CHAR_MOVE_BACK   "+charback"
#define CMD_CHAR_STOP_MOVE_FORW   "-charforward"
#define CMD_CHAR_STOP_MOVE_BACK   "-charback"

#define CMD_CHAR_ROT_LEFT    "+charlookleft"
#define CMD_CHAR_ROT_RIGHT   "+charlookright"
#define CMD_CHAR_STOP_ROT_LEFT    "-charlookleft"
#define CMD_CHAR_STOP_ROT_RIGHT   "-charlookright"

#define CMD_CHAR_STRAFE_LEFT    "+charstepleft"
#define CMD_CHAR_STRAFE_RIGHT   "+charstepright"
#define CMD_CHAR_STOP_STRAFE_LEFT    "-charstepleft"
#define CMD_CHAR_STOP_STRAFE_RIGHT   "-charstepright"

#define CMD_CAMERA_ZOOM_IN       "+camera_zoom_in"
#define CMD_CAMERA_ZOOM_OUT      "+camera_zoom_out"
#define CMD_CAMERA_STOP_ZOOM_IN  "-camera_zoom_in"
#define CMD_CAMERA_STOP_ZOOM_OUT "-camera_zoom_out"

#define CMD_CAMERA_LOOK_LEFT       "+camera_look_left"
#define CMD_CAMERA_LOOK_RIGHT      "+camera_look_right"
#define CMD_CAMERA_STOP_LOOK_LEFT  "-camera_look_left"
#define CMD_CAMERA_STOP_LOOK_RIGHT "-camera_look_right"

#define CMD_CAMERA_LOOK_UP        "+camera_look_up"
#define CMD_CAMERA_LOOK_DOWN      "+camera_look_down"
#define CMD_CAMERA_STOP_LOOK_UP   "-camera_look_up"
#define CMD_CAMERA_STOP_LOOK_DOWN "-camera_look_down"

#define CMD_BIND "bind"
#define CMD_GET_ATTRIBUTE "getat"
#define CMD_SET_ATTRIBUTE "setat"
#define CMD_QUIT "quit"
#define CMD_LOAD_CONFIG "load_config"
#define CMD_SAVE_CONFIG "save_config"

#define CMD_SAY "say"
#define CMD_TOUCH "touch"
#define CMD_INVENTORY "invent"
#define CMD_PICKUP "pickup"
#define CMD_PICKUP2 "pickup2"
#define CMD_DROP "drop"
#define CMD_GIVE "give"
#define CMD_IDENTIFY "ident"
#define CMD_READ_CONFIG "read_config"


#define CMD_CALLY_SET_STATE "set_state"
#define CMD_CALLY_SET_MOTION "set_motion"
#define CMD_CALLY_EXECUTE "execute"

#define CMD_KEY_PRESS "keypress"
#define CMD_EVENT "event"
#define CMD_RUN_SCRIPT "run_script"
#define CMD_LOAD_OBJECT_FILE "load_object_file"
#define CMD_LOAD_STATE_FILE "load_state_file"
#define CMD_LOAD_GENERAL "load_general"
#define CMD_SAVE_GENERAL "save_general"
#define CMD_LOAD_BINDING "load_bindings"
#define CMD_SAVE_BINDING "save_bindings"
#define CMD_LOAD_TEXTURE "load_textures"
#define CMD_LOAD_MODEL "load_models"
#define CMD_TOGGLE_CONSOLE "toggle_console"
#define CMD_TOGGLE_FULLSCREEN "toggle_fullscreen"

#define CMD_START_RUN "+run"
#define CMD_STOP_RUN "-run"
#define CMD_TOGGLE_ALWAYS_RUN "toggle_run"

#endif
