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

