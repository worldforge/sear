// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2006 Simon Goodall

#ifdef WIN32
#include <windows.h>
#elif defined(__APPLE__)
    #include <Carbon/Carbon.h>
#else
// Linunx Headers
#include <stdio.h>
#endif

#include "error.h"

namespace Sear {

void ErrorDialog(const std::string &msg) {
#ifdef WIN32
  MessageBox(NULL, msg.c_str(), "Error", MB_ICONERROR | MB_OK);
#elif defined(__APPLE__)
    
  DialogRef theAlert;
  DialogItemIndex itemIndex;
  CFStringRef cfmsg = CFStringCreateWithCString(kCFAllocatorDefault, 
    msg.c_str(), kCFStringEncodingASCII);
  
  ::CreateStandardAlert(kAlertStopAlert,
                CFSTR("Sear has experienced a serious error."),
                cfmsg,
                NULL, &theAlert);
 
  ::RunStandardAlert (theAlert, NULL, &itemIndex); 
  
  CFRelease(cfmsg);
#else
  printf("Error: %s\n", msg.c_str());
#endif
}

} // namespace Sear
