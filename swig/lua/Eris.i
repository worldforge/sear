%module Eris

%{

#include <sigc++/signal.h>


#include <Atlas/Objects/Operation.h>
#include <Atlas/Objects/ObjectsFwd.h>
#include <Atlas/Message/Element.h>

#include <Eris/Types.h>
#include <Eris/EntityRef.h>
#include <Eris/ServerInfo.h>

#include <Eris/Account.h>
#include <Eris/Alarm.h>
#include <Eris/Avatar.h>
#include <Eris/BaseConnection.h>
#include <Eris/Calendar.h>
#include <Eris/Connection.h>
#include <Eris/DeleteLater.h>
#include <Eris/Entity.h>
#include <Eris/EntityRouter.h>
#include <Eris/Exceptions.h>
#include <Eris/Factory.h>
#include <Eris/IGRouter.h>
#include <Eris/Room.h>
#include <Eris/Lobby.h>
#include <Eris/Log.h>
#include <Eris/LogStream.h>
#include <Eris/MetaQuery.h>
#include <Eris/Metaserver.h>
#include <Eris/Operations.h>
#include <Eris/Person.h>
#include <Eris/Poll.h>
#include <Eris/PollDefault.h>
//#include <Eris/PollGlibFD.h>
//#include <Eris/PollGlib.h>
//#include <Eris/PollGlibSource.h>
#include <Eris/Redispatch.h>
#include <Eris/Response.h>
#include <Eris/Router.h>
#include <Eris/Task.h>
#include <Eris/TerrainModObserver.h>
#include <Eris/TimedEventService.h>
#include <Eris/Timeout.h>
#include <Eris/TypeBoundRedispatch.h>
#include <Eris/TypeInfo.h>
//#include <Eris/TypeService.h>
//#include <Eris/UIFactory.h>
#include <Eris/View.h>
%}


%include <std_string.i>
%include <std_map.i>
%include <std_vector.i>

%include <Eris/Types.h>
%include <Eris/EntityRef.h>
%include <Eris/ServerInfo.h>

%include <Eris/Account.h>
%include <Eris/Alarm.h>
%include <Eris/Avatar.h>
%include <Eris/BaseConnection.h>
%include <Eris/Calendar.h>
%include <Eris/Connection.h>
%include <Eris/DeleteLater.h>
%include <Eris/Entity.h>
%include <Eris/EntityRouter.h>
%include <Eris/Exceptions.h>
%include <Eris/Factory.h>
%include <Eris/IGRouter.h>
%include <Eris/Room.h>
%include <Eris/Lobby.h>
%include <Eris/Log.h>
%include <Eris/LogStream.h>
%include <Eris/MetaQuery.h>
%include <Eris/Metaserver.h>
%include <Eris/Operations.h>
%include <Eris/Person.h>
%include <Eris/Poll.h>
%include <Eris/PollDefault.h>
#%include <Eris/PollGlibFD.h>
#%include <Eris/PollGlib.h>
#%include <Eris/PollGlibSource.h>
%include <Eris/Redispatch.h>
%include <Eris/Response.h>
%include <Eris/Router.h>
%include <Eris/Task.h>
%include <Eris/TerrainModObserver.h>
%include <Eris/TimedEventService.h>
%include <Eris/Timeout.h>
%include <Eris/TypeBoundRedispatch.h>
%include <Eris/TypeInfo.h>
##%include <Eris/TypeService.h>
##%include <Eris/UIFactory.h>
%include <Eris/View.h>

