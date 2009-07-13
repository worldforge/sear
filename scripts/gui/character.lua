-- Implementation of the Character Window in LUA
--
function createButton(win, name, eventId, x_pos, y_pos, al)
  btn = Guichan.Button(name);
  btn:setFocusable(false);
  btn:setActionEventId(eventId);
  btn:setPosition(x_pos, y_pos);
  btn:addActionListener(al);
  win:add(btn);
  return btn;
end

system = Sear.System_instance()
workarea = system:getWorkarea()

win = Sear.LogicWindow("Character Selection");
col = win:getBaseColor();
col.a = 128;
win:setBaseColor(col)
win:setOpaque(true);


y_pos = 0;
l1 = Guichan.Label("Characters");
l1:setPosition(0, y_pos);
win:add(l1);
y_pos = y_pos + l1:getHeight() + 4;


al = Sear.ActionListenerSigC();

listModel = Sear.CharacterListModel();

characters = Sear.DblClkListBox(listModel, al, Guichan.ActionEvent(win, "take"));
characters:setWidth(200);
characters:setFocusable(false);

scrollArea = Guichan.ScrollArea(characters, Guichan.ScrollArea_SHOW_NEVER, Guichan.ScrollArea_SHOW_ALWAYS);

scrollArea:setWidth(200);
scrollArea:setHeight(200);
scrollArea:setFrameSize(1);
scrollArea:setPosition(0, y_pos);
win:add(scrollArea);
y_pos = y_pos + scrollArea:getHeight() + 4;

refreshButton = createButton(win, "Refresh", "refresh", 0, y_pos, al);
y_pos = y_pos + refreshButton:getHeight() + 4;

l2 = Guichan.Label("Name");
l2:setPosition(0, y_pos);
win:add(l2);

nameField = Guichan.TextField("");
nameField:setPosition(l2:getWidth() + 2, y_pos);
win:add(nameField);
y_pos = y_pos + nameField:getHeight() + 4;

l3 = Guichan.Label("Type");
l3:setPosition(0, y_pos);
win:add(l3);

typeModel = Sear.TypeListModel();
types = Guichan.DropDown(typeModel);
types:setPosition(l3:getWidth()  + 2, y_pos);
win:add(types);
y_pos = y_pos + types:getHeight() + 4;

charButton = createButton(win, "Create new character", "create", 0, y_pos, al);
y_pos = y_pos + charButton:getHeight() + 4;

closeButton = createButton(win, "Close", "close", 0, y_pos, al);
y_pos = y_pos + closeButton:getHeight() + 4;


win:resizeToContent();

function action_cb(str)
  if (str == "close") then 
    workarea:removeLater(win);
  elseif (str == "take") then
    id = "";

    account = system:getClient():getAccount();
    cm = account:getCharacters();
    -- Iterate over list and find character 
    

    system:runCommand("/take "..id);
    workarea:removeLater(win);
  elseif (str == "create") then
    
    --workarea:removeLater(win);
  elseif (str == "refresh") then
    account = system:getClient():getAccount();
    account:refreshCharacterInfo();
  end
end

al:ActionCB(action_cb);
action_cb("check");

typeSelected = -1;
currentSelected = -1;

function logic() 
  newSelected = characters:getSelected();
  if (newSelected ~= currentSelected) then
    currentSelected = newSelected;  
    if (newSelected >= 0) then
      types:setSelected(-1);
      nameField:setText("");
      account = system:getClient():getAccount();
      if (account ~= nil) then
        cm = account:getCharacters();
        name = listModel:getElementAt(newSelected);
        avatar = cm:get(name);
        nameField:setText(name);
        typeName = avatar:getParents():front();
        typeField:setText(typeName);
        charButton:setCaption("Take Character");
        charButton:setActionEventId("take");
        for i = 0, i < typeModel:getNumberOfElements() do
          if (typeModel:getElementAt(i) == typeName) then
            typeSelected = i;
            types:setSelected(i);
            break;
          end
        end
      end
    end
  end

  if (newSelected >= 0) then

  end
end

return win;

--workarea:registerWindow("test", win);

--s-ystem:runCommand("/workarea_open test");
