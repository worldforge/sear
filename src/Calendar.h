// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2002 Simon Goodall

// $Id: Calendar.h,v 1.1 2002-12-24 18:08:17 simon Exp $

#ifndef SEAR_CALENDAR_H
#define SEAR_CALENDAR_H 1

#include <string>

#include <sigc++/object_slot.h>
#include <varconf/Config.h>
#include "src/ConsoleObject.h"

namespace Sear {

// Forward Declarations
class Console;
	
class Calendar : public SigC::Object, public ConsoleObject {
public:
  // List of time areas
  typedef enum {
    INVALID = 0,
    DAWN,
    DAY,
    DUSK,
    NIGHT
  } TimeArea;
  
  Calendar();
  ~Calendar();

  // Initialise Calendar
  void init();

  // Shutdown Calendar
  void shutdown();

  // Update calendar by time_elapsed milliseconds.
  void update(unsigned int time_elapsed) {
    update ((float)time_elapsed / 1000.0f);
  }

  // Update calendar by time_elapsed seconds.
  void update(float time_elapsed);

  // Read Calendar config data
  void readConfig();
  // Write Calendar config data
  void writeConfig();
  
  // Callback for config updates
  void config_update(const std::string &section, const std::string &key, varconf::Config &config);
 
  // Registers Calendar console commands
  void registerCommands(Console *console);

  // Function called to process Calendar console commands;
  void runCommand(const std::string &command, const std::string &args);
 
  // Returns number of seconds per minute
  unsigned int getSecondsPerMinute() const { return _seconds_per_minute; }
  // Returns number of minutes per hour
  unsigned int getMinutesPerHour() const { return _minutes_per_hour; }
  // Returns number of hours per day
  unsigned int getHoursPerDay() const { return _hours_per_day; }
  // Returns number of days per week
  unsigned int getDaysPerWeek() const { return _days_per_week; }
  // Returns number of weeks per month
  unsigned int getWeeksPerMonth() const { return _weeks_per_month; }
  // Returns number of months per year
  unsigned int getMonthsPerYear() const { return _months_per_year; }
  
  // Return seconds
  float getSeconds() const { return _seconds; }
  // Return minutes
  unsigned int getMinutes() const { return _minutes; }
  // Return hours
  unsigned int getHours() const { return _hours; }
  // Return days
  unsigned int getDays() const { return _days; }
  // Return weeks
  unsigned int getWeeks() const { return _weeks; }
  // Return months
  unsigned int getMonths() const { return _months; }
  // Return years
  unsigned int getYears() const { return _years; }

  // Returns current Time area
  TimeArea getTimeArea() const { return _time_area; }
  // Return start hour for time area
  unsigned int getDawnStart() const { return _dawn_start; }
  unsigned int getDayStart() const { return _day_start; }
  unsigned int getDuskStart() const { return _dusk_start; }
  unsigned int getNightStart() const { return _night_start; }
  
  float getTimeInArea() const { return _time_in_area; }
  
  // Return name of day
  std::string getDayName() const { return _current_day_name; }  
  // Return name of month
  std::string getMonthName() const { return _current_month_name; }  

  
private:
  bool _initialised; // Calendar initialisation state
  
  unsigned int _seconds_per_minute; // Number of seconds in a minute
  unsigned int _minutes_per_hour;   // Number of minutes in an hour
  unsigned int _hours_per_day;      // Number of hours in a day
  unsigned int _days_per_week;      // Number of days in a week
  unsigned int _weeks_per_month;    // Number of weeks in a month
  unsigned int _months_per_year;    // Number of months in a year

  // Current time and date values
  float _seconds; // Updates are done in milliseconds
  float _seconds_counter; // Number of seconds passed in current day
  unsigned int _minutes;
  unsigned int _hours;
  unsigned int _days;
  unsigned int _weeks;
  unsigned int _months;
  unsigned int _years;
  
  TimeArea _time_area;
  unsigned int _dawn_start;
  unsigned int _day_start;
  unsigned int _dusk_start;
  unsigned int _night_start;

  float _time_in_area;
  
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
