// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2003 Simon Goodall

// $Id: Calendar.h,v 1.3 2003-03-23 19:51:49 simon Exp $

#ifndef SEAR_CALENDAR_H
#define SEAR_CALENDAR_H 1

#include <string>

#include <sigc++/object_slot.h>
#include <varconf/Config.h>
#include "interfaces/ConsoleObject.h"

/**
 * Sear namespace
 */ 
namespace Sear {

// Forward Declarations
class Console;

/**
 * This class regulates the client-side game time.
 * \todo Link this with server time
 */ 
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

  /**
   * Default constructor
   */   
  Calendar();

  /**
   * Destructor
   */ 
  ~Calendar();

  /**
   * Initialise Calendar
   */ 
  void init();

  /**
   *  Shutdown Calendar
   */  
  void shutdown();

  /** 
   * Update calendar by time_elapsed
   * @param time_elapsed Time elapsed in milliseconds 
   */
  void update(unsigned int time_elapsed) {
    update ((float)time_elapsed / 1000.0f);
  }

  /**
   * Update calendar by time_elapsed seconds.
   * @param time_elapsed Time elapsed in seconds
   */
  void update(float time_elapsed);

  /**
   * Read Calendar config data
   */  
  void readConfig();
  
  /**
   * Write Calendar config data
   */  
  void writeConfig();
  
  /**
   * Callback for config updates
   * @param section The config section the key is in
   * @param key The updated key
   * @param config The config object that was updated
   */
  void config_update(const std::string &section, const std::string &key, varconf::Config &config);
 
  /**
   * Registers Calendar console commands
   * @param console The console object to register with
   */ 
  void registerCommands(Console *console);

  /**
   * Run a console command
   * @param command Console command
   * @param args Console command arguments
   */  
  void runCommand(const std::string &command, const std::string &args);
 
  /**
   * Returns number of seconds per minute
   * @return Seconds per minute
   */ 
  unsigned int getSecondsPerMinute() const { return _seconds_per_minute; }
  
  /**
   * Returns number of minutes per hour
   * @return Minutes per hour
   */
  unsigned int getMinutesPerHour() const { return _minutes_per_hour; }
  
  /**
   * Returns number of hours per day
   * @return Hours per day
   */
  unsigned int getHoursPerDay() const { return _hours_per_day; }
  
  /**
   * Returns number of days per week
   * @return Days per week
   */ 
  unsigned int getDaysPerWeek() const { return _days_per_week; }
  
  /**
   * Returns number of weeks per month
   * @return Weeks per month
   */
  unsigned int getWeeksPerMonth() const { return _weeks_per_month; }
  
  /**
   * Returns number of months per year
   * @param Months per year
   */
  unsigned int getMonthsPerYear() const { return _months_per_year; }
  
  /**
   * Return current seconds
   * @return Current seconds
   */
  float getSeconds() const { return _seconds; }
  
  /**
   * Return minutes
   * @return Current minutes
   */
  unsigned int getMinutes() const { return _minutes; }
  
  /**
   * Return hours
   * Current hours
   */ 
  unsigned int getHours() const { return _hours; }
  
  /**
   * Return days
   * @return Current days
   */ 
  unsigned int getDays() const { return _days; }
  
  /**
   * Return weeks
   * @return Current weeks
   */ 
  unsigned int getWeeks() const { return _weeks; }
  
  /**
   * Return months
   * @return Current months
   */ 
  unsigned int getMonths() const { return _months; }
  
  /*
   * Return years
   * @return Current years
   */ 
  unsigned int getYears() const { return _years; }

  /**
   * Returns current Time area
   * @return Current time area
   */ 
  TimeArea getTimeArea() const { return _time_area; }
  
  /**
   * Return start hour for dawn
   * @return Dawn start hour
   */ 
  unsigned int getDawnStart() const { return _dawn_start; }

  /**
   * Return start hour for day
   * @return Day start hour
   */ 
  unsigned int getDayStart() const { return _day_start; }

  /**
   * Return start hour for dusk
   * @return Dusk start hour
   */ 
  unsigned int getDuskStart() const { return _dusk_start; }

  /**
   * Return start hour for night
   * @return Night start hour
   */ 
  unsigned int getNightStart() const { return _night_start; }
  
  /**
   * Return number of seconds in current time area
   * @return Seconds in time area
   */ 
  float getTimeInArea() const { return _time_in_area; }
  
  /**
   * Return name of day
   * @return Day name
   */
  std::string getDayName() const { return _current_day_name; }
  
  /**
   * Return name of month
   * @return Month name
   */
  std::string getMonthName() const { return _current_month_name; }  

  
private:
  bool _initialised; ///< Calendar initialisation state
  
  unsigned int _seconds_per_minute; ///< Number of seconds in a minute
  unsigned int _minutes_per_hour;   ///< Number of minutes in an hour
  unsigned int _hours_per_day;      ///< Number of hours in a day
  unsigned int _days_per_week;      ///< Number of days in a week
  unsigned int _weeks_per_month;    ///< Number of weeks in a month
  unsigned int _months_per_year;    //</ Number of months in a year

  // Current time and date values
  float _seconds; ///< Current seconds
  float _seconds_counter; ///< Number of seconds passed in current day
  unsigned int _minutes; ///< Current minutes
  unsigned int _hours; ///< Current hour
  unsigned int _days; ///< Current day
  unsigned int _weeks; ///< Current week
  unsigned int _months; ///< Current month
  unsigned int _years; ///< Current years
  
  TimeArea _time_area; ///< The current time area
  unsigned int _dawn_start; ///< Hour in which dawn starts
  unsigned int _day_start; ///< Hour in which day starts
  unsigned int _dusk_start; ///< Hour in which dusk starts
  unsigned int _night_start; ///< Hour in which night starts

  float _time_in_area; ///< Time duration in current time area
  
  /**
   * Mapping between number and name
   */  
  typedef std::map<unsigned int, std::string> NameMap;
  
  NameMap _day_names;   ///< Mapping for day names
  NameMap _month_names; ///< Mapping for month names

  std::string _current_day_name;   ///< Name of current day
  std::string _current_month_name; ///< Name of current month

  SigC::Connection _config_connection; ///< Connection object for signal
};
	
} /* namespace Sear */

#endif /* SEAR_CALENDER_H */
