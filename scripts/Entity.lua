-- Package provides a wrapper around exisiting entity functions.

local Public = {};
Entity = Public;

-- Create a copy of the basic entity structure
function Entity.new()
  new = {};
  new.getEntity = %Public.getEntity;
  new.getName = %Public.getName;
  new.getID = %Public.getID;
  new.getType = %Public.getType;
  new.getParent = %Public.getParent;
  new.getProperty = %Public.getProperty;
  new.onMove = %Public.onMove;
  new.onProperty = %Public.onProperty;
  new.onTalk = %Public.onTalk;
  new.onAppearance = %Public.onAppearance;
  new.onDisappearance = %Public.onDisappearance;
  new.onCreate = %Public.onCreate;
  new.onDelete = %Public.onDelete;
  new.printInfo = %Public.printInfo;
  return new;
end

-- Wrapper functions

function Entity.getEntity(entity_id)
  return entity_get_entity(entity_id);
end

function Entity.getName(entity)
  return entity_get_name(entity);
end

function Entity.getID(entity)
  return entity_get_id(entity);
end

function Entity.getType(entity)
  return entity_get_type(entity);
end

function Entity.getParent(entity)
  return entity_get_parent(entity);
end

function Entity.getProperty(entity, property)
  return entity_get_property(entity, property);
end

function Entity.onMove(entity, pos)
  print(Entity.getName(entity).." - Moved - "..pos);
end

function Entity.onProperty(entity, property)
  print (Entity.getName(entity).." - Property Changed - "..property);
  print (Entity.getProperty(entity, property));
end

function Entity.onTalk(entity, talk)
  print (Entity.getName(entity).." - Talk - "..talk);
end

function Entity.onAppearance(entity, args)
  print (Entity.getName(entity).." - Appearance");
end

function Entity.onDisappearance(entity, args)
  print (Entity.getName(entity).." - Disappearance");
end


function Entity.onCreate(entity, args)
  print (Entity.getName(entity).." - Created");
end

function Entity.onDelete(entity, args)
  print (Entity.getName(entity).." - Deleted");
end

function Entity.printInfo(entity)
  print("Name: "..%Public.getName(entity));
  print("ID: "..%Public.getID(entity));
  print("Type: "..%Public.getType(entity));
  print("Parent: "..%Public.getParent(entity));
  print("mode: "..%Public.getProperty(entity, "mode"));
  print("action: "..%Public.getProperty(entity, "action"));
end

function test()
  simon = Entity.getEntity("simon_34");
  world = Entity.getEntity("world_0");
  Entity.printInfo(simon);
  Entity.printInfo(world);
end

