-- This file may be redistributed and modified only under the terms of
-- the GNU General Public License (See COPYING for details).
-- Copyright (C) 2005 Alistair Riddoch
-- Copyright (C) 2007 - 2009 Simon Goodall

-- Implementation of the Character Window in LUA

local self = {};

--
function createButton(win, name, eventId, x_pos, y_pos, al)
  local btn = Guichan.Button(name);
  btn:setFocusable(false);
  btn:setActionEventId(eventId);
  btn:setPosition(x_pos, y_pos);
  btn:addActionListener(al);
  win:add(btn);
  return btn;
end

local system = Sear.System_instance()
local workarea = system:getWorkarea()

self.win = Sear.LogicWindow("Character Selection");
local col = self.win:getBaseColor();
col.a = 128;
self.win:setBaseColor(col)
self.win:setOpaque(true);

local textSize = 15;
local lTextSize = 5;

local y_pos = 0;
self.l1 = Guichan.Label("Characters");
self.l1:setPosition(0, y_pos);
self.win:add(self.l1);
y_pos = y_pos + self.l1:getHeight() + 4;


self.al = Sear.ActionListenerSigC();

self.listModel = Sear.CharacterListModel();

self.characters = Sear.DblClkListBox(self.listModel, self.al, Guichan.ActionEvent(win, "take"));
self.characters:setWidth(200);
self.characters:setFocusable(false);

self.scrollArea = Guichan.ScrollArea(self.characters, Guichan.ScrollArea_SHOW_NEVER, Guichan.ScrollArea_SHOW_ALWAYS);

self.scrollArea:setWidth(200);
self.scrollArea:setHeight(200);
self.scrollArea:setFrameSize(1);
self.scrollArea:setPosition(0, y_pos);
self.win:add(self.scrollArea);
y_pos = y_pos + self.scrollArea:getHeight() + 4;

self.refreshButton = createButton(self.win, "Refresh", "refresh", 0, y_pos, self.al);
y_pos = y_pos + self.refreshButton:getHeight() + 4;

self.l2 = Guichan.Label(pad("Name", lTextSize, " "));
self.l2:setPosition(0, y_pos);
self.win:add(self.l2);

self.nameField = Guichan.TextField(pad("", textSize, " "));
self.nameField:setPosition(self.l2:getWidth() + 2, y_pos);
self.win:add(self.nameField);
self.nameField:setText("");
y_pos = y_pos + self.nameField:getHeight() + 4;

self.l3 = Guichan.Label(pad("Type", lTextSize, " "));
self.l3:setPosition(0, y_pos);
self.win:add(self.l3);

self.typeModel = Sear.TypeListModel();
self.typesBox = Guichan.DropDown(self.typeModel);
self.typesBox:setPosition(self.l3:getWidth()  + 2, y_pos);
self.win:add(self.typesBox);
y_pos = y_pos + self.typesBox:getHeight() + 4;

self.charButton = createButton(self.win, "Create new character", "create", 0, y_pos, self.al);
y_pos = y_pos + self.charButton:getHeight() + 4;

self.closeButton = createButton(self.win, "Close", "close", 0, y_pos, self.al);
y_pos = y_pos + self.closeButton:getHeight() + 4;


self.win:resizeToContent();

function action_cb(str)
  if (str == "close") then 
    workarea:removeLater(self.win);
  elseif (str == "take") then
    local id = "";

    local account = system:getClient():getAccount();
    local cm = account:getCharacters();
    -- Iterate over list and find character 
    

    system:runCommand("/take "..id);
    workarea:removeLater(self.win);
  elseif (str == "create") then
    
    --workarea:removeLater(win);
  elseif (str == "refresh") then
    local account = system:getClient():getAccount();
    account:refreshCharacterInfo();
  end
end

self.actionSlot = self.al:ActionCB(action_cb);
action_cb("check");

local typeSelected = -1;
local currentSelected = -1;

function logic() 
  -- The logic function fills in the details on list box change.
  -- It also de-selects upon user edit.
  -- We also check to see if the user has changed a character selection detail
  -- and so make it a character creation

  local newSelected = self.characters:getSelected();

  -- Has selection changed?
  if (newSelected ~= currentSelected) then

    currentSelected = newSelected;  

    -- Is selection valid?
    if (newSelected >= 0) then
      -- Reset fields
      self.typesBox:setSelected(-1);
      self.nameField:setText("");
      local account = system:getClient():getAccount();
      if (account ~= nil) then
        -- Get selected character details
        local cm = account:getCharacters();
        local name = self.listModel:getElementAt(newSelected);
        local avatar = cm:get(name);
        local typeName = avatar:getParents():front();

        -- Update fields
        self.nameField:setText(name);
        self.typeField = typeName;
        self.charButton:setCaption("Take Character");
        self.charButton:setActionEventId("take");

	-- Update type list box
        for i = 0, i < self.typeModel:getNumberOfElements() do
          if (self.typeModel:getElementAt(i) == typeName) then
            typeSelected = i;
            self.typesBox:setSelected(i);
            break;
          end
        end
      end
    end
  end


  -- Check details, if they have been edited since we selected a character, then
  -- we are creating a new character rather than taking an existing one.
  if (newSelected >= 0) then
    local account = system:getClient():getAccount();
    if (account ~= nil) then
      -- Get selected character details
      local cm = account:getCharacters();
      local name = self.listModel:getElementAt(newSelected);
      local avatar = cm:get(name);
      local typeName = avatar:getParents():front();
   
      if (self.nameField:getText() ~= name or self.typeNameField:getText() ~= typeName) then
        self.characters:setSelected(-1);
	self.charButton:setCaption("Create new character");
	self.charButton:setEventId("create");
      end
    end
  end

  -- If type selection changes, create new character
  local new_type_sel = self.typesBox:getSelected();
  if (new_type_sel ~= typeSelected) then
    typeSelected = new_type_sel;
    if (new_type_sel >= 0) then
      self.typeField = "";
      local account = system:getClient():getAccount();
      if (account ~= nil) then
        local cm = account:getCharacters();
        local name = self.listModel:getElementAt(newSelected);
        local avatar = cm:get(name);
        local typeName = avatar:getParents():front();
        local types = account:getCharacterTypes();

        -- Set type string to selected option 
        if (typeSelected < types:size()) then
          self.typeField = types:get(typeSelected);
        end
      end
      self.characters:setSelected(-1);
      self.charButton:setCaption("Create new character");
      self.charButton:setActionEventId("create");
    end
  end

  -- If typeField differs from selection, reset selection
  if (new_type_sel >= 0) then
    local account = system:getClient():getAccount();
    if (account ~= nil) then
      local types = account:getCharacterTypes();
      if (new_type_sel >= 0 and typeSelected < types:size()) then
        if (self.typeField ~= types:get(typeSelected)) then
          self.typesBox:setSelected(-1);
        end
      end
    end
  end
end

return self.win;
