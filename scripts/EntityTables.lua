-- LUA copy of entity information

-- If a requested entity does not exist in EntityTable, then it will be created
-- from data pulled from the EntityTypeTable.

-- When calling LUA functions from C, we lookup the entity, then run the 
-- functions associated with it. This means we can specify functions on a per-
-- type basis or even per-entity.

-- List of all entities
EntityTable = {};
-- Lists of all entity types
EntityTypeTable = {};

function entityEvent(id, func, arglist)
  print (id.."-"..func.."-"..arglist);
  if EntityTable[id] then
  else 
    createEntity(id);
  end
  if EntityTable[id] then
    entity = EntityTable[id];
    if entity then
      if entity[func] then
        entity[func](entity.entity, arglist);
      else
        print ("no func defined");
      end
    else
      print ("no entity created");
    end 
  else
    print ("No id in table");
  end
end

function createEntity(id)
  print("Creating Entity Record - "..id);
  entity = Entity.getEntity(id);
  EntityTable[id] = Entity.new();
  EntityTable[id].entity = entity;
end

function copyEntity(entity)
  copy = {};
  copy.getName = entity.getName;
  copy.getID = entity.getID;
  copy.getParent = entity.getParent;
  copy.getType = entity.getType;
  copy.getProperty = entity.getProperty;
  -- Callback stuff
  copy.onMove = entity.onMove;
  copy.onMode = entity.onMode;
  copy.onAction = entity.onAction;
  copy.onTalk = entity.onTalk;
  copy.onAppearance = entity.onAppearance;
  copy.onDisappearance = entity.onDisappearance;
  copy.onCreate = entity.onCreate;
  copy.onDelete = entity.onDelete;
end

EntityTypeTable["default"] = Entity.new();
EntityTypeTable["default"].mode = "";
