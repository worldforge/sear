// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2002 Simon Goodall, University of Southampton

#include "conf.h"
#include "Error.h"
#include "System.h"
void traceError(int err) {
  std::string str;
  switch (err) {
    case CLIENT_ERROR: break;
    case CLIENT_ERROR_HOSTNAME: str = "No hostname given"; break;
    case CLIENT_ERROR_PASSWORD: str = "No password given"; break;
    case CLIENT_ERROR_USERNAME: str = "No username given"; break;
    case CLIENT_ERROR_FULLNAME: str = "No username given"; break;
    case CLIENT_ERROR_CONNECTION: str = "No Connection Object has been created"; break;
    case CLIENT_ERROR_NOT_CONNECTED: str = "Need to connect to server first"; break;
    case CLIENT_ERROR_ALREADY_CONNECTED: str = "Connection already established"; break;
    case CLIENT_ERROR_CHAR_NAME: str = "No character name given"; break;
    case CLIENT_ERROR_CHAR_DESC: str = "No chracter description given"; break;
    case CLIENT_ERROR_CHAR_CREATE: str = "The character could not be created"; break;
    
    case CLIENT_ERROR_CHAR_ID: str ="No id given"; break;
    case CLIENT_ERROR_CHAR_TAKE: str = "The character could not be taken"; break;
    case CLIENT_ERROR_LOBBY: str = "Error no lobby created"; break;
    case CLIENT_ERROR_PLAYER: str = "Error no player created"; break;
    case CLIENT_ERROR_LOGIN: str = "There was an error logging in"; break;

    case CLIENT_ERROR_CONNECTING: str = "There was an error during request to connect"; break;

    case CLIENT_ERROR_DISCONNECTING: str = "There was an error disconnecting"; break;
    default:  str = "Unknown Error"; break;
  }
  System::instance()->pushMessage("Error: "  + str, 3);
}
