dofile("scripts/bindings.lua");
dofile("scripts/Entity.lua");
dofile("scripts/Character.lua");
dofile("scripts/EntityTables.lua");

function onNetLoginSuccess()
  get_characters();
end
