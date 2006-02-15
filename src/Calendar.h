// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2006 Simon Goodall

// $Id: Calendar.h,v 1.11 2006-02-15 12:44:24 simon Exp $

#ifndef SEAR_CALENDAR_H
#define SEAR_CALENDAR_H 1

#include <string>

#include <sigc++/trackable.h>

#include <varconf/Config.h>

#include "interfaces/ConsoleObject.h"
#include "common/SPtr.h"

namespace Eris {
  class Avatar;
  class Calendar;
}

/**
 * Sear namespace
 */ 
namespace Sear {

// Forward Declarations
class Console;

/**
 * This class regulates the client-side game time.
 */ 
class Calendar : public sigc::trackable, public ConsoleObject {
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
  bool isInitialised() const { return m_initialised; }

  /**
   * Update calendar to server_time seconds.
   * @param server_time The current time from the server
   */
  void update();

  /**
   * Read Calendar config data
   */  
  void readConfig(varconf::Config &config);
  
  /**
   * Write Calendar config data
   */  
  void writeConfig(varconf::Config &config);
  
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
  int getSecondsPerMinute() const { return m_seconds_per_minute; }
  
  /**
   * Returns number of minutes per hour
   * @return Minutes per hour
   */
  int getMinutesPerHour() const { return m_minutes_per_hour; }
  
  /**
   * Returns number of hours per day
   * @return Hours per day
   */
  int getHoursPerDay() const { return m_hours_per_day; }
  
  /**
   * Returns number of days per month
   * @return Days per month
   */ 
  int getDaysPerMonth() const { return m_days_per_month; }
  
  /**
   * Returns number of months per year
   * @param Months per year
   */
  int getMonthsPerYear() const { return m_months_per_year; }
  
  /**
   * Return current seconds
   * @return Current seconds
   */
  double getSeconds() const { return m_seconds; }
  
  /**
   * Return minutes
   * @return Current minutes
   */
  int getMinutes() const { return m_minutes; }
  
  /**
   * Return hours
   * Current hours
   */ 
  int getHours() const { return m_hours; }
  
  /**
   * Return days
   * @return Current days
   */ 
  int getDays() const { return m_days; }
  
  /**
   * Return months
   * @return Current months
   */ 
  int getMonths() const { return m_months; }
  
  /*
   * Return years
   * @return Current years
   */ 
  int getYears() const { return m_years; }

  /**
   * Returns current Time area
   * @return Current time area
   */ 
  TimeArea getTimeArea() const { return m_time_area; }
  
  /**
   * Return start hour for dawn
   * @return Dawn start hour
   */ 
  int getDawnStart() const { return m_dawn_start; }

  /**
   * Return start hour for day
   * @return Day start hour
   */ 
  int getDayStart() const { return m_day_start; }

  /**
   * Return start hour for dusk
   * @return Dusk start hour
   */ 
  int getDuskStart() const { return m_dusk_start; }

  /**
   * Return start hour for night
   * @return Night start hour
   */ 
  int getNightStart() const { return m_night_start; }
  
  /**
   * Return number of seconds in current time area
   * @return Seconds in time area
   */ 
  double getTimeInArea() const { return m_time_in_area; }
  
  void reset() { m_firstUpdate = true; }
  
  void setAvatar(Eris::Avatar *avatar);

private:
  bool m_initialised; ///< Calendar initialisation state
  
  int m_seconds_per_minute; ///< Number of seconds in a minute
  int m_minutes_per_hour;   ///< Number of minutes in an hour
  int m_hours_per_day;      ///< Number of hours in a day
  int m_days_per_month;      ///< Number of days in a month
  int m_months_per_year;    //</ Number of months in a year

  // Current time and date values
  double m_seconds; ///< Current seconds
  double m_server_seconds; ///< Predicted server seconds
  double m_seconds_counter; ///< Number of seconds passed in current day
  int m_minutes; ///< Current minutes
  int m_hours; ///< Current hour
  int m_days; ///< Current day
  int m_months; ///< Current month
  int m_years; ///< Current years
  
  TimeArea m_time_area; ///< The current time area
  int m_dawn_start; ///< Hour in which dawn starts
  int m_day_start; ///< Hour in which day starts
  int m_dusk_start; ///< Hour in which dusk starts
  int m_night_start; ///< Hour in which night starts

  double m_time_in_area; ///< Time duration in current time area
  
  bool m_firstUpdate;

  SPtr<Eris::Calendar> m_cal;
};
	
} /* namespace Sear */

#endif /* SEAR_CALENDER_H */
