-- This file may be redistributed and modified only under the terms of
-- the GNU General Public License (See COPYING for details).
-- Copyright (C) 2005 Alistair Riddoch
-- Copyright (C) 2007 - 2009 Simon Goodall

-- Implementation of the Connect Window in LUA
--
-- TODO:
-- More robust logic!
-- Fixed width text fields

-- TODO: This should just register the window!

-- Create table to store all reference in.
-- This avoids copying when referenced within functions
-- Required for win var. Unsure about rest
self = {}

-- Take refs to main objects
self.system = Sear.System_instance()
self.workarea = self.system:getWorkarea()


self.win = Sear.LogicWindow("Login Window");
col = self.win:getBaseColor();
col.a = 128;
self.win:setBaseColor(col);

self.win:setOpaque(true);

self.serverListModel = Sear.ServerListModel();

self.al = Sear.ActionListenerSigC();

self.vbox1 = Sear.VBox(6);

self.servers = Sear.DblClkListBox(self.serverListModel, self.al, Guichan.ActionEvent(self.win, "connect"));

self.servers:setWidth(200);
self.servers:setFocusable(false);
self.scrollArea1 = Guichan.ScrollArea(self.servers, Guichan.ScrollArea_SHOW_NEVER, Guichan.ScrollArea_SHOW_ALWAYS);

self.scrollArea1:setWidth(200);
self.scrollArea1:setHeight(200);

self.vbox1:pack(self.scrollArea1);

self.serverField = Guichan.TextField("                ");
self.vbox1:pack(self.serverField);

-- Little function to create a button
function createButton(hbox, name, eventId, al)
  btn = Guichan.Button(name);
  btn:setFocusable(false);
  btn:setActionEventId(eventId);
  btn:addActionListener(al);
  hbox:pack(btn);
  return btn;
end

self.hbox1 = Sear.HBox(6);
self.connectBtn = createButton(self.hbox1, "Connect", "connect", self.al);
self.refreshBtn = createButton(self.hbox1, "Refresh", "refresh", self.al);
self.closeBtn   = createButton(self.hbox1, "Close", "close", self.al);

self.vbox1:pack(self.hbox1);

self.win:add(self.vbox1);

self.win:resizeToContent();

self.selected = -1;

function logic_cb()
  newSelected = self.servers:getSelected();
  if (newSelected ~= self.selected) then

    self.selected = newSelected;

    sl = self.serverListModel.m_meta:getServerList();
    if (self.selected >= 0) then 
      if (self.selected < self.serverListModel:getNumberOfElements()) then
        e = self.serverListModel:getElementDataAt(self.selected);
        str = e.hostname.." "..e.port;
        self.serverField:setText(str);
      end
    end
  end
end

self.win:LogicCB(logic_cb);
function action_cb(event)
  close = false;
  if (event == "connect") then 
    cmd = "/connect "..self.serverField:getText();
    self.system:runCommand(cmd);
    close = true;
  elseif (event == "refresh") then
    self.serverListModel.m_meta:runCommand("refresh_server_list", "");
  elseif (event == "close") then
    close = true;
  end

  -- Close window if required
  if (close == true) then
    self.workarea:removeLater(self.win);
  end
end

-- Register callback
self.al:ActionCB(action_cb);

return self.win;
