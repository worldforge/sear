-- Code for player character
-- Wraps Character functions
-- Extends Entity

local Public = Entity.new();
Character = Public;

function Character.new()
  new = Entity.new();
  new.pickup = %Public.pickup;
  new.drop = %Public.drop;
  new.dropTo = %Public.dropTo;
  new.inventory = %Public.inventory;
  new.moveTo = %Public.moveTo;
  new.setVel = %Public.setVel;
  return new;
end


-- Wrapper funcitons
function Character.pickup(entity, target)

end

function Character.drop(entity, target)

end

function Character.dropTo(entity, target, pos)

end

function Character.inventory(entity)

end

function Character.moveTo(entity, pos)

end

function Character.setVel(entity, vel)

end
