// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2002 Simon Goodall, University of Southampton

#ifndef _EXCEPTION_H_
#define _EXCEPTION_H_ 1

#include <string>

namespace Sear {

class Exception {
public:
  Exception() {}
  Exception(const std::string &msg) : _msg(msg) {}
  virtual ~Exception() {}

  virtual const std::string getMessage() const { return _msg; }

protected:
  std::string _msg;

};


class ClientException : public Sear::Exception {
public:
typedef enum {
  DEFAULT = 0,
  NO_CONNECTION_OBJECT,
  NOT_CONNECTED,
  ALREADY_CONNECTED,
  NO_HOSTNAME,
  ERROR_CONNECTING,
  ERROR_DISCONNECTING,
  BAD_STATE,
  ACTION_IN_PROGRESS,
  
  BAD_CONNECTION,
  BAD_PASSWORD,
  BAD_USERNAME,
  BAD_FULLANME,
  BAD_ACCOUNT_CREATION,
  BAD_CHARACTER_CREATION,
  BAD_LOGIN,
  BAD_CHARACTER
} Type;


/*
 * TODO
 * Make msg optional -> base it upon type, but allow overriding
 */

//  ClientException() {}
//  ClientException(const std::string &msg) : _msg(msg) {}
  ClientException(const std::string &msg, const Type type) : _type(type) {
    _msg = msg;
  }
  virtual ~ClientException() {}

  virtual const Type getType() const { return _type; }
protected:
  Type _type;

};

} /* namespace Sear */

#endif /* _EXCEPTION_H_ */
