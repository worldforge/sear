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

#ifndef _LUA_VALUE_H_
#define _LUA_VALUE_H_

#ifdef HAVE_CONFIG_H
#include "../config.h"
#endif

extern "C"
{
#ifdef HAVE_LUA
  #include <lua40/lua.h>
#else
  #include "lua/lua.h"
#endif
}

#include <string>
#include <vector>
#include <stdexcept>

namespace Lua {

class StackUnderrunException : public std::underflow_error {
 public: 
  StackUnderrunException() : 
    std::underflow_error("LuaScript stack underrun") {};
};

class StackOverflowException  : public std::overflow_error {
 public: 
  StackOverflowException() : 
    std::overflow_error("LuaScript stack overflow") {};
};
      
class NoCastException : public std::exception { };  
class NoTableException : public std::exception { };
class NoSuchElementException : public std::exception { };

class Value
{
 public:
  typedef double NumberType;
  typedef std::string StringType;
  typedef std::vector< std::pair<Value, Value> > TableType;
  typedef int TagType;
  typedef void* UserdataType;
  typedef int(*FunctionType)(lua_State*);
  enum type_t { NIL, NUMBER, STRING, TABLE, USERDATA, FUNCTION };

  Value() : type_(NIL) {};
  Value(NumberType val):type_(NUMBER), tag_(LUA_ANYTAG) 
    { n = new NumberType(val); };
  Value(int val):type_(NUMBER), tag_(LUA_ANYTAG) 
    { n = new NumberType(val); }; 
  Value(const char* val):type_(STRING), tag_(LUA_ANYTAG)  
    { s = new StringType(val); };
  Value(const StringType& val):type_(STRING), tag_(LUA_ANYTAG)  
    { s = new StringType(val); };
  Value(const TableType& val):type_(TABLE), tag_(LUA_ANYTAG)  
    { t = new TableType(val); };
  Value(const UserdataType& val):type_(USERDATA), tag_(LUA_ANYTAG)  
    { u = val; };
  Value(FunctionType val):type_(FUNCTION), tag_(LUA_ANYTAG)  
    { f = val; };
  
  Value(const Value& val);

  Value& operator=(const Value& val);

  Value& operator[](const char* id)
	throw(NoSuchElementException, NoTableException);
  const Value& operator[](const char* id) const
	throw(NoSuchElementException, NoTableException);

  bool has(const char* id) const throw(NoTableException);

  Value& operator[](int _n) throw(NoTableException);
  const Value& operator[](int _n) const throw(NoTableException);

  Value(TagType tag, void* val) : type_(USERDATA), tag_(tag) 
    { u = val; }; 
  Value(TagType tag, const TableType& val) : type_(TABLE), tag_(tag) 
    { t = new TableType(val); };
  
  void setTag(TagType tag) { tag_ = tag; };

  virtual ~Value() { clear(); }

  static Value peek(lua_State*, int index = -1)
	throw (StackOverflowException, StackUnderrunException);
  static Value pop(lua_State*)
	throw (StackOverflowException, StackUnderrunException);
  static void push(lua_State*, const Value&)
	throw (StackOverflowException);

  bool isNil() const { return type_ == NIL; };
  bool isNumber() const { return type_ == NUMBER; };
  bool isString() const { return type_ == STRING; };
  bool isTable() const { return type_ == TABLE; };
  bool isUserdata() const { return type_ == USERDATA; };
  bool isFunction() const { return type_ == FUNCTION; };

  type_t type() const { return type_; };
  TagType tag() const { return tag_; };

  NumberType asNumber() const  throw(NoCastException)
    {
      if(type_ != NUMBER) throw NoCastException();
      return *n;
    }
  NumberType& asNumber()  throw(NoCastException)
    {
      if(type_ != NUMBER) throw NoCastException();
      return *n;
    }

  const StringType& asString() const throw(NoCastException)
    {
      if(type_ != STRING) throw NoCastException();
      return *s;
    }
  StringType& asString() throw(NoCastException)
    {
      if(type_ != STRING) throw NoCastException();
      return *s;
    }
  
  const TableType& asTable() const  throw(NoCastException)
    {
      if(type_ != TABLE) throw NoCastException();
      return *t;
    }
  TableType& asTable()  throw(NoCastException)
    {
      if(type_ != TABLE) throw NoCastException();
      return *t;
    }

  const UserdataType& asUserdata() const  throw(NoCastException)
    {
      if(type_ != USERDATA) throw NoCastException();
      return u;
    }
  UserdataType& asUserdata()  throw(NoCastException)
    {
      if(type_ != USERDATA) throw NoCastException();
      return u;
    }

  const FunctionType& asFunction() const  throw(NoCastException)
    {
      if(type_ != FUNCTION) throw NoCastException();
      return f;
    }

  FunctionType& asFunction()  throw(NoCastException)
    {
      if(type_ != FUNCTION) throw NoCastException();
      return f;
    }
  
 protected:

  type_t type_;
  TagType tag_;

  void clear();

  union 
  {
    NumberType* n;
    StringType* s;
    TableType* t;
    UserdataType u;
    FunctionType f;
  };

};

} // namespace

#endif



  
