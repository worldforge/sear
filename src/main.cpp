// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2002 Simon Goodall, University of Southampton

#include <unistd.h>
#include <iostream.h>
#include <string>
#include "config.h"
#include "conf.h"
#include "debug.h"
#include "System.h"


int main(int argc, char** argv) {
  bool exit_program = false;
  Sear::System *sys = NULL;
  std::string install_dir = "";
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
/*      else if (arg == "-I" || arg == "--install-dir") {
	if (argc < 1) {
          std::cerr << "No path supllied!" << std::endl;
          exit_program = true;
	} else {
          install_dir = std::string((char *)argv[0]);
          argv++;
          argc--;
	}
      }
  */    
      else if (arg == "-h" || arg == "--help") {
        std::cout << invoked << " {options}" << std::endl;
	std::cout << "-h, --help    - display this message" << std::endl;
	std::cout << "-v, --version - display version info" << std::endl;
//	std::cout << "-I, --install-dir - directory where sear was installed. e.g. " << INSTALLDIR << "/share/sear" << std::endl;
	exit_program = true;
      }
      else {
        std::cout << "Unknown arument: " << arg << std::endl;
      }
    }
  }
  if (exit_program) exit(0);
/*
Sear version VERSION, Copyright (C) 2001 - 2002 Simon Goodall, University of Southampton
Sear comes with ABSOLUTELY NO WARRANTY; for details type `show w'.
This is free software, and you are welcome to redistribute it
under certain conditions; type `show c' for details.
*/
  
  sys = new Sear::System();
//  sys->setInstallDir(std::string(INSTALLDIR) + std::string("/share/sear"));
  if (!sys->init()) {
    std::cerr << "Error initialising Sear!" << std::endl;
    exit (1);
  }
  sys->createWindow(false);
  sys->setCaption(CLIENT_NAME, CLIENT_NAME);
//  sys->setInstallDir(install_dir);
  sys->mainLoop();
  sys->shutdown();
  delete sys;
  exit(0);
}

#if defined(_WIN32)

#include <windows.h>

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrev, LPSTR szCmdLine, int sw) {
  char *argvp[128];
  int i = 1;
  char *p = strtok(szCmdLine, " ");
  while (p) {
    argvp[i++] = p;
    p = strtok(NULL, " ");
  }

  argvp[0] = "sear.exe";

  main(i, argvp); 

  return 0;
}
#endif
