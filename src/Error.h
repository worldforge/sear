// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2002 Simon Goodall, University of Southampton

#include <stdio.h>

//Error Codes
#define CLIENT_ERROR 0x0
#define CLIENT_ERROR_HOSTNAME 0xd
#define CLIENT_ERROR_PASSWORD 0x1
#define CLIENT_ERROR_USERNAME 0x2
#define CLIENT_ERROR_FULLNAME 0x3
#define CLIENT_ERROR_CONNECTION 0x4
#define CLIENT_ERROR_NOT_CONNECTED 0x5
#define CLIENT_ERROR_ALREADY_CONNECTED 0x6
#define CLIENT_ERROR_CHAR_NAME 0x7
#define CLIENT_ERROR_CHAR_DESC 0x8
#define CLIENT_ERROR_CHAR_ID 0x9
#define CLIENT_ERROR_PLAYER 0xa
#define CLIENT_ERROR_LOGIN 0xb
#define CLIENT_ERROR_LOBBY 0xc
#define CLIENT_ERROR_CHAR_CREATE 0xe
#define CLIENT_ERROR_CHAR_TAKE 0xf
#define CLIENT_ERROR_CONNECTING 0x10
#define CLIENT_ERROR_DISCONNECTING 0x11

namespace Sear {
void traceError(int);
} /* namespace Sear */
