//  The Worldforge Project
//  Copyright (C) 1998, 1999,2000,2001 The Worldforge Project
//
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation; either version 2 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
//
//  For information about Worldforge and its authors, please contact
//  the Worldforge Web Site at http://www.worldforge.org.
//
// created by Karsten Laux 03/2001

#include "lua_value.h"

#ifdef HAVE_CONFIG_H
#include "../config.h"
#endif

Lua::Value::Value(const Value& val)
{
  //actually do the assignment
  type_ = val.type_;
  tag_ = val.tag_;

  switch(type_) 
    {
    case NIL:
    case FUNCTION:
      f = val.f;
      break;
    case NUMBER: 
      n = new NumberType(*val.n);
      break;
    case STRING: 
      s = new StringType(*val.s);
      break;
    case TABLE: 
      t = new TableType(*val.t);
      break;
    case USERDATA: 
      u = val.u;
      break;
    };    
}

Lua::Value&
Lua::Value::operator=(const Value& val)
{
  //check for self assignment
  if(&val == this)
    return *this; 
  //clear existing data
  clear();
  //actually do the assignment
  type_ = val.type_;
  tag_ = val.tag_;

  switch(type_) 
    {
    case NIL:
    case FUNCTION:
      f = val.f;
      break;
    case NUMBER: 
      n = new NumberType(*val.n);
      break;
    case STRING: 
      s = new StringType(*val.s);
      break;
    case TABLE: 
      t = new TableType(*val.t);
      break;
    case USERDATA: 
      u = val.u;
      break;
    };    
  return *this;
}
  
Lua::Value&
Lua::Value::operator[](const char* id)
	throw(NoSuchElementException, NoTableException)
{
  if(type_ != TABLE) throw NoTableException();
  TableType::iterator itr = t->begin();
  std::string _id = std::string(id);

  while(itr != t->end())
    {
      if(itr->first.isString() && itr->first.asString() == _id)
        break;
      else
        itr++;
    }

  if(itr == t->end())
    {
      //create new NIL entry
       std::pair<Value, Value> entry;
      entry.first = _id;
      t->push_back(entry);
    };
  return itr->second;
}

const Lua::Value&
Lua::Value::operator[](const char* id) const
	throw(NoSuchElementException, NoTableException)
{
  if(type_ != TABLE) throw NoTableException();
  TableType::const_iterator itr = t->begin();
  std::string _id = std::string(id);

  while(itr != t->end())
    {
      if(itr->first.isString() && itr->first.asString() == _id)
        break;
      else
        itr++;
    }
  if(itr == t->end()) throw NoSuchElementException();
  return itr->second;
}    
  
bool
Lua::Value::has(const char* id) const throw(NoTableException)
{
  if(type_ != TABLE) throw NoTableException();
  TableType::const_iterator itr = t->begin();
  std::string _id = std::string(id);

  while(itr != t->end())
    {
      if(itr->first.isString() && itr->first.asString() == _id)
        break;
      else
        itr++;
    }

  return (itr != t->end());
}

Lua::Value&
Lua::Value::operator[](int _n) throw(NoTableException)
{
  unsigned n = (unsigned) _n;
  if(type_ != TABLE) throw NoTableException();
  if(n >= t->size())
    {
      t->resize(n+1);
      (*t)[n].first = (int)(n+1);
    }
  return (*t)[n].second;
}
  
const Lua::Value&
Lua::Value::operator[](int _n) const throw(NoTableException)
{
  unsigned n = (unsigned) _n;
  if(type_ != TABLE) throw NoTableException();
  if(n >= t->size()) throw NoSuchElementException();
  return (*t)[n].second;
}
  
Lua::Value 
Lua::Value::peek(lua_State *handle_, int index)
	throw (StackOverflowException, StackUnderrunException)
{
  // check if stack is not empty
  if(!lua_gettop(handle_))
    throw StackUnderrunException();

  Value result;
  switch(lua_type(handle_, index))
    {
    case LUA_TNONE:
      throw StackUnderrunException();
      break;
    case LUA_TNIL:
      break;
    case LUA_TNUMBER:
      result = lua_tonumber(handle_, index);
      break;
    case LUA_TSTRING:
      result = std::string(lua_tostring(handle_, index), lua_strlen(handle_, index));
      break;
    case LUA_TTABLE:
      {
	int table_index = index;
	Value::TableType table;
	//get tag of the lua object
	Value::TagType tag = lua_tag(handle_, index);
	/* check if there is at least 2 entries free on the stack,
	 * which are needed for table traversal 
	 */
	if(lua_stackspace(handle_) < 2)
	  throw StackOverflowException();
	
	lua_pushnil(handle_);  /* push first 'dummy' key */
	Value key, value;
	while (lua_next(handle_, table_index) != 0) 
	  {
	    /* `key' is at index -2 and `value' at index -1 */
	    value = pop(handle_); // note the recursion ! (nested tables ...)
	    key = peek(handle_); // keep key on the stack for next iteration !
	    table.push_back(std::pair<Value, Value>(key, value));
	  }
	result = table;
	if(tag != LUA_TTABLE)
	  result.setTag(tag);
      }
      break;
    case LUA_TFUNCTION:
      result = lua_tocfunction(handle_, index);
      break;
    case LUA_TUSERDATA:
      {
	//get tag of the lua object
	Value::TagType tag = lua_tag(handle_, index);
	result = lua_touserdata(handle_, index);
	if(tag != LUA_TUSERDATA)
	  result.setTag(tag);
      }
      break;
    default:
      break;
    }

  return result;
}

Lua::Value 
Lua::Value::pop(lua_State *handle_) throw (StackOverflowException, StackUnderrunException)
{
  // read value at top of stack
  Value result = peek(handle_);

  // actually pop the element from the stack
  lua_pop(handle_,1);
  
  return result;
}  
  
void 
Lua::Value::push(lua_State *handle_, const Value& val) throw (StackOverflowException)
{
  if(!lua_stackspace(handle_))
    throw StackOverflowException();

  switch(val.type())
    {
    case Value::NIL:
      lua_pushnil(handle_);
      break;
    case Value::NUMBER:
      lua_pushnumber(handle_, val.asNumber());
      break;
    case Value::STRING:
      lua_pushlstring(handle_, val.asString().c_str(), val.asString().size());
      break;
    case Value::USERDATA:
      {      
	lua_pushusertag(handle_, val.asUserdata(), val.tag());
      }
      break;
    case Value::FUNCTION:
      lua_pushcfunction(handle_, val.asFunction());
      break;
    case Value::TABLE:
      {
	//create a new empty table and push it onto the stack
	lua_newtable(handle_);
       
	//if table owns special tag, tag the lua object appropriately
	if(val.tag() != LUA_ANYTAG)
	  lua_settag(handle_, val.tag());

	//remember the stack index of the table
	int table_index = lua_gettop(handle_);
	Value::TableType::const_iterator itr = val.asTable().begin();
	while(itr != val.asTable().end())
	  {
	    /* push key and value onto the stack ... */
	    push(handle_, itr->first);
	    push(handle_, itr->second); // note the recursion ! (nested tables ...)
	    /* and set the table's entry: */
	    lua_settable(handle_, table_index);
	    itr++;
	  }
      }
      break;
      
    }
}

void
Lua::Value::clear()
{
  switch(type_) 
    {
    case NIL:
    case FUNCTION:
      break;
    case NUMBER: 
      delete n;
      break;
    case STRING: 
      delete s;
      break;
    case TABLE: 
      delete t;
      break;
    case USERDATA: 
      break;
    };
}
