// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2002 Simon Goodall

// $Id: Calender.h,v 1.1 2002-12-24 15:12:37 simon Exp $

#ifndef SEAR_CALENDER_H
#define SEAR_CALENDER_H 1

#include <string>

#include <sigc++/object_slot.h>
#include <varconf/Config.h>
#include "src/ConsoleObject.h"

namespace Sear {

class Console;
	
class Calender : public SigC::Object, public ConsoleObject {
public:
  Calender();
  ~Calender();
 
  void init();
  void shutdown();

  void update(unsigned int time_elapsed) {
    update ((float)time_elapsed / 1000.0f);
  }

  void update(float time_elapsed);

  void readConfig();
  void writeConfig();
  
  void config_update(const std::string &section, const std::string &key, varconf::Config &config);
 
  void registerCommands(Console *console);
  void runCommand(const std::string &command, const std::string &args);
  
  float getSeconds() { return _seconds; }
  unsigned int getMinutes() { return _minutes; }
  unsigned int getHours() { return _hours; }
  unsigned int getDays() { return _days; }
  unsigned int getWeeks() { return _weeks; }
  unsigned int getMonths() { return _months; }
  unsigned int getYears() { return _years; }
	 
  std::string getDayName() { return _current_day_name; }  
  std::string getMonthName() { return _current_month_name; }  
  
private:
  bool _initialised;
  
  unsigned int _seconds_per_minute;
  unsigned int _minutes_per_hour;
  unsigned int _hours_per_day;
  unsigned int _days_per_week;
  unsigned int _weeks_per_month;
  unsigned int _months_per_year;

  float _seconds; // Updates are done in milliseconds
  unsigned int _minutes;
  unsigned int _hours;
  unsigned int _days;
  unsigned int _weeks;
  unsigned int _months;
  unsigned int _years;
  
  typedef std::map<unsigned int, std::string> NameMap;
  
  NameMap _day_names;
  NameMap _month_names;

  std::string _current_day_name;
  std::string _current_month_name;

  SigC::Connection _config_connection;
};
	
} /* namespace Sear */

#endif /* SEAR_CALENDER_H */
