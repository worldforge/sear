-- Register GUI classes and action handlers
--

system = Sear.System_instance();
workarea = system:getWorkarea();

-- TODO: Obtain from sys call?
scriptPath = "/home/sg/src/sear-tmp/sear/scripts/gui/";

-- Create and register windows
workarea:registerWindow("connect", dofile(scriptPath.."connect.lua"));
workarea:registerWindow("login", dofile(scriptPath.."login.lua"));
workarea:registerWindow("character", dofile(scriptPath.."character.lua"));

-- TODO: De-register windows on cleanup?





ac = system:getActionHandler();

ac:addHandler("connected", "/workarea_close connect");
ac:addHandler("connected", "/workarea_open login");

ac:addHandler("disconnected", "/workarea_alert Connection to server failed");
ac:addHandler("disconnected", "/workarea_open connect");

ac:addHandler("logged_in", "/workarea_close login");
ac:addHandler("logged_in", "/workarea_open character");

ac:addHandler("world_entered", "/workarea_close character");
ac:addHandler("world_entered", "/workarea_open panel");

ac:addHandler("avatar_failed", "/workarea_alert Unable to get character");

ac:addHandler("inventory_open", "/panel_toggle Inventory");
