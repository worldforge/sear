// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2002 Simon Goodall

// $Id: Calender.h,v 1.2 2002-12-24 16:15:39 simon Exp $

#ifndef SEAR_CALENDER_H
#define SEAR_CALENDER_H 1

#include <string>

#include <sigc++/object_slot.h>
#include <varconf/Config.h>
#include "src/ConsoleObject.h"

namespace Sear {

// Forward Declarations
class Console;
	
class Calender : public SigC::Object, public ConsoleObject {
public:
  Calender();
  ~Calender();

  // Initialise Calender
  void init();

  // Shutdown Calender
  void shutdown();

  // Update calender by time_elapsed milliseconds.
  void update(unsigned int time_elapsed) {
    update ((float)time_elapsed / 1000.0f);
  }

  // Update calender by time_elapsed seconds.
  void update(float time_elapsed);

  // Read Calender config data
  void readConfig();
  // Write Calender config data
  void writeConfig();
  
  // Callback for config updates
  void config_update(const std::string &section, const std::string &key, varconf::Config &config);
 
  // Registers Calender console commands
  void registerCommands(Console *console);

  // Function called to process Calender console commands;
  void runCommand(const std::string &command, const std::string &args);
  
  // Return seconds
  float getSeconds() { return _seconds; }
  // Return minutes
  unsigned int getMinutes() { return _minutes; }
  // Return hours
  unsigned int getHours() { return _hours; }
  // Return days
  unsigned int getDays() { return _days; }
  // Return weeks
  unsigned int getWeeks() { return _weeks; }
  // Return months
  unsigned int getMonths() { return _months; }
  // Return years
  unsigned int getYears() { return _years; }
	 
  // Return name of day
  std::string getDayName() { return _current_day_name; }  
  // Return name of month
  std::string getMonthName() { return _current_month_name; }  
  
private:
  bool _initialised; // Calender initialisation state
  
  unsigned int _seconds_per_minute; // Number of seconds in a minute
  unsigned int _minutes_per_hour;   // Number of minutes in an hour
  unsigned int _hours_per_day;      // Number of hours in a day
  unsigned int _days_per_week;      // Number of days in a week
  unsigned int _weeks_per_month;    // Number of weeks in a month
  unsigned int _months_per_year;    // Number of months in a year

  // Current time and date values
  float _seconds; // Updates are done in milliseconds
  unsigned int _minutes;
  unsigned int _hours;
  unsigned int _days;
  unsigned int _weeks;
  unsigned int _months;
  unsigned int _years;
  
  // Mapping between number and name
  typedef std::map<unsigned int, std::string> NameMap;
  
  NameMap _day_names;   // Mapping for day names
  NameMap _month_names; // Mapping for month names

  std::string _current_day_name;   // Name of current day
  std::string _current_month_name; // Name of current month

  SigC::Connection _config_connection; // Connection object for signal
};
	
} /* namespace Sear */

#endif /* SEAR_CALENDER_H */
