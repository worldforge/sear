-- This file may be redistributed and modified only under the terms of
-- the GNU General Public License (See COPYING for details).
-- Copyright (C) 2005 Alistair Riddoch
-- Copyright (C) 2007 - 2009 Simon Goodall

-- Implementation of the Login Window in LUA

local self = {};

-- Take refs to main objects
local system = Sear.System_instance()
local workarea = system:getWorkarea()

local textSize = 10;

self.win = Sear.LogicWindow("Login Window");
col = self.win:getBaseColor();
col.a = 128;
self.win:setBaseColor(col);

self.al = Sear.ActionListenerSigC();

self.vbox = Sear.VBox(6);

self.hbox_user = Sear.HBox(6);
self.l1 = Guichan.Label(pad("User Name", textSize, " "));
self.hbox_user:pack(self.l1);
self.t1 = Guichan.TextField(pad("", textSize, " "));
self.hbox_user:pack(self.t1);
self.t1:setText("");
self.vbox:pack(self.hbox_user);

self.hbox_pwd = Sear.HBox(6);
self.l2 = Guichan.Label(pad("Password", textSize, " "));
self.hbox_pwd:pack(self.l2);
self.t2 = Sear.PasswordField(pad("", textSize, " "), al, Guichan.ActionEvent(win, "login"));
self.hbox_pwd:pack(self.t2);
self.t2:setText("");
self.vbox:pack(self.hbox_pwd);


self.hbox_pwd2 = Sear.HBox(6);
self.l3 = Guichan.Label(pad("", textSize, " "));
self.hbox_pwd2:pack(self.l3);
self.t3 = Sear.PasswordField(pad("", textSize, " "), al, Guichan.ActionEvent(win, "login"));
self.hbox_pwd2:pack(self.t3);
self.t3:setText("");
self.vbox:pack(self.hbox_pwd2);


self.hbox_name = Sear.HBox(6);
self.l4 = Guichan.Label(pad("Name", textSize, " "));
self.hbox_name:pack(self.l4);
self.t4 = Guichan.TextField(pad("", textSize, " "));
self.hbox_name:pack(self.t4);
self.t4:setText("");
self.vbox:pack(self.hbox_name);

self.check = Guichan.CheckBox("New Account");
self.check:setActionEventId("check");
self.check:addActionListener(self.al);
self.vbox:pack(self.check);

self.hbox3 = Sear.HBox(6)
self.btn = Guichan.Button("Login");
self.hbox3:pack(self.btn);
self.btn:setActionEventId("login");
self.btn:addActionListener(self.al);

self.btn_close = Guichan.Button("Close");
self.hbox3:pack(self.btn_close);
self.btn_close:setActionEventId("close");
self.btn_close:addActionListener(self.al);


self.vbox:pack(self.hbox3);

self.hbox_end = Sear.HBox();
self.hbox_end:pack(self.vbox, 6);
self.vbox_end = Sear.VBox();
self.vbox_end:pack(self.hbox_end, 6);

self.win:add(self.vbox_end);

self.win:resizeToContent();

function action_cb(str)
  if (str == "close") then 
    workarea:removeLater(self.win);
  elseif (str == "check") then
    local visible = self.check:isSelected();
    self.l3:setVisible(visible);
    self.t3:setVisible(visible);
    self.l4:setVisible(visible);
    self.t4:setVisible(visible);
    self.t3:setEnabled(visible);
    self.t4:setEnabled(visible);
    if (self.check:isSelected()) then
      self.btn:setCaption("Create");
    else
      self.btn:setCaption("Login");
    end
  elseif (str == "login") then
    if (self.check:isSelected()) then

    -- TODO: Check passwords match
    -- Quote strings
    -- Check for empty values
    -- Disable login button otherwise

      system:runCommand("/create " .. t1:getText() .. " " .. t2:getText() .. " " ..t4:getText());
    else
      system:runCommand("/login " ..t1:getText() .. " ".. t2:getText());
    end
  end
end

-- Logic function called each frame
function logic_cb()
 -- Anything to do here? 
end

-- Register logic function
-- (Only do if required)
 self.logicSlot = self.win:LogicCB(logic_cb);

-- Register callback
self.actionSlot = self.al:ActionCB(action_cb);

-- Trigger GUI updates
action_cb("check");

return self.win;

