-- This file may be redistributed and modified only under the terms of
-- the GNU General Public License (See COPYING for details).
-- Copyright (C) 2005 Alistair Riddoch
-- Copyright (C) 2007 - 2009 Simon Goodall

-- Implementation of the Login Window in LUA
--
-- TODO:
-- More robust logic!
-- Fixed width text fields

-- TODO: This should just register the window!


-- Take refs to main objects
system = Sear.System_instance()
workarea = system:getWorkarea()

win = Sear.LogicWindow("Login Window");
col = win:getBaseColor();
col.a = 128;
win:setBaseColor(col);

al = Sear.ActionListenerSigC();

vbox = Sear.VBox(6);

hbox_user = Sear.HBox(6);
l1 = Guichan.Label("User Name");
hbox_user:pack(l1);
t1 = Guichan.TextField("          ");
hbox_user:pack(t1);
t1:setText("");
vbox:pack(hbox_user);

hbox_pwd = Sear.HBox(6);
l2 = Guichan.Label("Password");
hbox_pwd:pack(l2);
t2 = Sear.PasswordField("          ", al, Guichan.ActionEvent(win, "login"));
hbox_pwd:pack(t2);
t2:setText("");
vbox:pack(hbox_pwd);


hbox_pwd2 = Sear.HBox(6);
l3 = Guichan.Label("");
hbox_pwd2:pack(l3);
t3 = Sear.PasswordField("          ", al, Guichan.ActionEvent(win, "login"));
hbox_pwd2:pack(t3);
t3:setText("");
vbox:pack(hbox_pwd2);


hbox_name = Sear.HBox(6);
l4 = Guichan.Label("Name");
hbox_name:pack(l4);
t4 = Guichan.TextField("          ");
hbox_name:pack(t4);
t4:setText("");
vbox:pack(hbox_name);

check = Guichan.CheckBox("New Account");
check:setActionEventId("check");
check:addActionListener(al);
vbox:pack(check);

hbox3 = Sear.HBox(6)
btn = Guichan.Button("Login");
hbox3:pack(btn);
btn:setActionEventId("login");
btn:addActionListener(al);

btn_close = Guichan.Button("Close");
hbox3:pack(btn_close);
btn_close:setActionEventId("close");
btn_close:addActionListener(al);


vbox:pack(hbox3);

hbox_end = Sear.HBox();
hbox_end:pack(vbox, 6);
vbox_end = Sear.VBox();
vbox_end:pack(hbox_end, 6);

win:add(vbox_end);

win:resizeToContent();

function action_cb(str)
  if (str == "close") then 
    workarea:removeLater(win);
  elseif (str == "check") then
    visible = check:isSelected();
    l3:setVisible(visible);
    t3:setVisible(visible);
    l4:setVisible(visible);
    t4:setVisible(visible);
    t3:setEnabled(visible);
    t4:setEnabled(visible);
    if (check:isSelected()) then
      btn:setCaption("Create");
    else
      btn:setCaption("Login");
    end
  elseif (str == "login") then
    if (check:isSelected()) then

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
-- win:LogicCB(logic_cb);



-- Register callback
al:ActionCB(action_cb);

-- Trigger GUI updates
action_cb("check");


return win;

--workarea:registerWindow("login", win);
