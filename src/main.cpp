// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2004 Simon Goodall, University of Southampton

// $Id: main.cpp,v 1.19 2004-04-29 10:33:26 simon Exp $
#ifdef HAVE_CONFIG_H
  #include "config.h"
#endif

#include <unistd.h>
#include <iostream>
#include <string>

#ifdef HAVE_CONFIG_H
    #include "config.h"
#endif

#include "conf.h"
#include "System.h"
#include "Exception.h"


#ifdef USE_MMGR
  #include "common/mmgr.h"
#endif

#ifdef DEBUG
  static const bool debug = true;
#else
  static const bool debug = false;
#endif
#if defined(_WIN32)
int SDL_main(int argc, char** argv) {
#else

int main(int argc, char** argv) {
#endif
  bool exit_program = false;
  Sear::System *sys = NULL;
  std::list<std::string> path_list;
  sys = NULL;
  
  if (argc > 1) {
    std::string invoked = std::string((char *)argv[0]);
    (argv)++;
    argc--;
    while (argc > 0)  {
      std::string arg = std::string((char *)argv[0]);
      argv++;
      argc--;
      if (arg == "-v" || arg == "--version") {
        std::cout << "Sear version: " << VERSION << std::endl;
	exit_program = true;
      }
      else if (arg == "-a" || arg == "--add-search-path") {
	if (argc < 1) {
          std::cerr << "No path supplied!" << std::endl;
          exit_program = true;
	} else {
	  path_list.push_back(std::string((char *)argv[0]));
          argv++;
          argc--;
	}
      }    
      else if (arg == "-h" || arg == "--help") {
        std::cout << invoked << " {options}" << std::endl;
	std::cout << "-h, --help    - display this message" << std::endl;
	std::cout << "-v, --version - display version info" << std::endl;
	std::cout << "-a, --add-search-path - Adds a search path" << std::endl;
	exit_program = true;
      }
      else {
        std::cout << "Unknown arument: " << arg << std::endl;
      }
    }
  }
  if (exit_program) exit(0);
/*
Sear version VERSION, Copyright (C) 2001 - 2004 Simon Goodall, University of Southampton
Sear comes with ABSOLUTELY NO WARRANTY; for details type `show w'.
This is free software, and you are welcome to redistribute it
under certain conditions; type `show c' for details.
*/
  
  sys = new Sear::System();
  sys->addSearchPaths(path_list);
  if (!sys->init()) {
    std::cerr << "Error initialising Sear!" << std::endl;
    exit (1);
  }
  try {
  //  sys->createWindow(false);
    sys->setCaption(CLIENT_NAME, CLIENT_NAME);
    sys->mainLoop();
  } catch (Sear::Exception e) {
    std::cerr << "Exception: " << e.getMessage() << std::endl;
  } catch (...) {
    std::cerr << "Unknown Exception" << std::endl;
  }
  sys->shutdown();
  delete sys;
  exit(0);
}

#if (0) // defined(_WIN32)
#include <windows.h>

// If we are compiling for Win32 we need this. 
int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrev, LPSTR szCmdLine, int sw) {
  // Convert szCmdLine into argc and argv format	
  char *argvp[128];
  int i = 1;
  char *p = strtok(szCmdLine, " ");
  while (p) {
    argvp[i++] = p;
    p = strtok(NULL, " ");
  }
  // Set first arg as execuatable name
  argvp[0] = "sear.exe";
  
  // Pass arguments to the main function
  main(i, argvp); 
  
  return 0;
}
#endif
