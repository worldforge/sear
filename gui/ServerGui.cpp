#ifdef HAVE_CONFIG_H
#include "config.h"
#endif 

#ifdef HAVE_GLGOOEY
#include "glgooey/WindowManager.h"
#include "glgooey/Rectangle.h"
#include "glgooey/FrameWindow.h"
#include "glgooey/CheckBox.h"
#include "glgooey/CheckBoxGroup.h"
#include "glgooey/Button.h"
#include "glgooey/Panel.h"
#include "glgooey/EditField.h"
#include "glgooey/MultiTextButton.h"
#include "glgooey/Font.h"
#include "glgooey/ScrollBar.h"
#include "glgooey/ListBox.h"
#include "glgooey/ListControl.h"
#include "glgooey/StaticText.h"
#include "glgooey/ProgressBar.h"
#include "glgooey/ComplexGridLayouter.h"
#include "glgooey/StaticBitmap.h"
#include "glgooey/TimeManager.h"
#endif

#include "ServerGui.h"

#include "common/Utility.h"

namespace Sear {

ServerGui::ServerGui() {
#ifdef HAVE_GLGOOEY
  Gooey::Font::initializeSystem("./samples/data/");

  server_window = new Gooey::FrameWindow(Gooey::Rectangle(0,0, 400, 200), 0, "Server");
  panel = new Gooey::Panel(Gooey::Rectangle(10, 10, 20, 20), server_window);
  button_connect = new Gooey::Button(panel,"Connect");
  button_disconnect = new Gooey::Button(panel,"Disconnect");
  button_reconnect = new Gooey::Button(panel,"Reconnect");

  textbox_hostname = new Gooey::EditField(panel, "localhost");
  textbox_port = new Gooey::EditField(panel, "6767");

  label_hostname = new Gooey::StaticText(panel, "Hostname");
  label_port = new Gooey::StaticText(panel, "Port");
 
  // Add action handler 
  sbl = new ServerButtonListener(*textbox_hostname, *textbox_port);
  button_connect->addMessageListener(sbl);
  button_disconnect->addMessageListener(sbl);
  button_reconnect->addMessageListener(sbl);

  Gooey::ComplexGridLayouter* layouter = new Gooey::ComplexGridLayouter;
  panel->setLayouter(layouter);
  std::vector<unsigned int> widths;
  widths.push_back(64);
  widths.push_back(100);
  std::vector<unsigned int> heights;
  heights.push_back(30);
  heights.push_back(30);
  heights.push_back(30);
  heights.push_back(30);
  heights.push_back(30);
  layouter->setColumnWidths(widths);
  layouter->setRowHeights(heights);
  Gooey::ComplexGridCellInfo info;

  ///info.horizontalSpan = 2;
  //    info.verticalSpan = 1;
  info.columnIndex = 0; info.rowIndex = 2;

  panel->addChildWindow(button_connect, info);
  info.columnIndex = 1; info.rowIndex = 2;  
  panel->addChildWindow(button_reconnect, info);  
  info.columnIndex = 2; info.rowIndex = 2;
  panel->addChildWindow(button_disconnect, info);
  //    info.horizontalSpan = 1;
  info.columnIndex = 0; info.rowIndex = 0;
  panel->addChildWindow(label_hostname, info);
  info.columnIndex = 0; info.rowIndex = 1;
  panel->addChildWindow(label_port, info);

  info.columnIndex = 1; info.rowIndex = 0;
  panel->addChildWindow(textbox_hostname, info);  
  info.columnIndex = 1; info.rowIndex = 1;
  panel->addChildWindow(textbox_port, info);
  
  server_window->setClientWindow(panel);
  // Create server list
  server_list_window = new Gooey::FrameWindow(Gooey::Rectangle(0, 300, 640, 480),0, "Servers");
//  server_list_panel = new Gooey::Panel(Gooey::Rectangle(10, 10, 20, 20), server_list_window);
  
  button_refresh = new Gooey::Button(panel,"Refresh");
  button_cancel = new Gooey::Button(panel,"Cancel");
  server_list = new Gooey::ListControl(server_list_window, Gooey::Rectangle(), num_server_list_columns);
  mbl = new MetaButtonListener(*server_list);
  button_refresh->addMessageListener(mbl);
  button_cancel->addMessageListener(mbl);
  // Add column heading to list control
  std::vector<std::string> header;
  header.push_back("Host Name");
  header.push_back("Server Name");
  header.push_back("Rule Set");
  header.push_back("Server Type");
  header.push_back("Clients");
  header.push_back("Ping");
  header.push_back("Uptime");
  server_list->addRow(header);

  panel->addChildWindow(button_refresh);
  panel->addChildWindow(button_cancel);
//  server_list_panel->addChildWindow(server_list);
  
  lcl = new ListControlListener(*textbox_hostname, *server_list);
  server_list->addMessageListener(lcl);
 
  server_list_window->setClientWindow(server_list);
  
  panel->arrangeChildren();
//  server_list_panel->arrangeChildren();
	
//  server_list_window->setClientWindow(server_list_panel);
  Gooey::WindowManager::instance().addWindow(server_window).addWindow(server_list_window);
#endif
}

ServerGui::~ServerGui() {

}
	
void ServerGui::addServer(Eris::ServerInfo si) {
#ifdef HAVE_GLGOOEY
  std::vector<std::string> server;
  std::string num_clients, ping, uptima;
  server.push_back(si.getHostname());
  server.push_back(si.getRuleset());
  server.push_back(si.getServer());
  server.push_back(string_fmt(si.getNumClients()));
  server.push_back(string_fmt(si.getUptime()));

  server_list->addRow(server);
#endif
}

}
