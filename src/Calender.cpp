// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2002 Simon Goodall

// $Id: Calender.cpp,v 1.2 2002-12-24 16:15:34 simon Exp $

#include "Calender.h"
#include "common/Utility.h"

#include "src/Console.h"
#include "src/System.h"


#ifdef DEBUG
  static const bool debug = true;
  #include "common/mmgr.h"
#else
  static const bool debug = false;
#endif

// Config section name
static const std::string CALENDER = "calender";

// Config Key names
static const std::string KEY_SECONDS_PER_MINUTE = "seconds_per_minute";
static const std::string KEY_MINUTES_PER_HOUR = "minutes_per_hour";
static const std::string KEY_HOURS_PER_DAY = "hours_per_day";
static const std::string KEY_DAYS_PER_WEEK = "days_per_week";
static const std::string KEY_WEEKS_PER_MONTH = "weeks_per_month";
static const std::string KEY_MONTHS_PER_YEAR = "months_per_year";

// Config Key prefix's for day and month names
static const std::string KEY_DAY_NAME = "day_name_";
static const std::string KEY_MONTH_NAME = "month_name_";

// Default config values
static const unsigned int DEFAULT_SECONDS_PER_MINUTE = 60;
static const unsigned int DEFAULT_MINUTES_PER_HOUR = 20;
static const unsigned int DEFAULT_HOURS_PER_DAY = 24;
static const unsigned int DEFAULT_DAYS_PER_WEEK = 7;
static const unsigned int DEFAULT_WEEKS_PER_MONTH = 4;
static const unsigned int DEFAULT_MONTHS_PER_YEAR = 12;

// Console Commands
static const std::string GET_TIME = "get_time";
static const std::string SET_SECONDS = "set_seconds";
static const std::string SET_MINUTES = "set_minutes";
static const std::string SET_HOURS = "set_hours";
static const std::string SET_DAYS = "set_days";
static const std::string SET_WEEKS = "set_weeks";
static const std::string SET_MONTHS = "set_months";
static const std::string SET_YEARS =  "set_years";

namespace Sear {

Calender::Calender() :
  _initialised(false),
  _seconds_per_minute(DEFAULT_SECONDS_PER_MINUTE),
  _minutes_per_hour(DEFAULT_MINUTES_PER_HOUR),
  _hours_per_day(DEFAULT_HOURS_PER_DAY),
  _days_per_week(DEFAULT_DAYS_PER_WEEK),
  _weeks_per_month(DEFAULT_WEEKS_PER_MONTH),
  _months_per_year(DEFAULT_MONTHS_PER_YEAR),
  _seconds(0.0f),
  _minutes(0),
  _hours(12), // Default start time is noon
  _days(0),
  _months(0),
  _years(0)
{}
	
Calender::~Calender() {
  if (_initialised) shutdown();
}

void Calender::init() {
  if (_initialised) shutdown(); // shutdown first if we are already initialised
  // Read in initial config
  readConfig();
  // Bind signal to config for further updates
  _config_connection = System::instance()->getGeneral().sigsv.connect(SigC::slot(*this, &Calender::config_update));
  
  _initialised = true;
}

void Calender::shutdown() {
  // Save config
  writeConfig();
  // Remove update signal
  _config_connection.disconnect();
  _initialised = false;
}

void Calender::update(float time_elapsed) {
  _seconds += time_elapsed;
  // Check for seconds overflow  
  if (_seconds >= _seconds_per_minute) {
    ++_minutes;
    _seconds -= _seconds_per_minute;
  } 
  // Check for minutes overflow
  if (_minutes >= _minutes_per_hour) {
    ++_hours;
    _minutes -= _minutes_per_hour;
  }
  // Check for hours overflow
  if (_hours >= _hours_per_day) {
    ++_days;
    // Update day name
    _current_day_name = _day_names[_days];
    _hours -= _hours_per_day;
  }
  // Check for days overflow
  if (_days >= _days_per_week) {
    ++_weeks;
    _days -= _days_per_week;
    // Update day name
    _current_day_name = _day_names[_days];
  }
  // Check for weeks overflow
  if (_weeks >= _weeks_per_month) {
    ++_months;
    // Update month name
    _current_month_name = _month_names[_months];
    _weeks -= _weeks_per_month;
  }
  // Check for months overflow
  if (_months >= _months_per_year) {
    ++_years;
    _months -= _months_per_year;
    // Update month name
    _current_month_name = _month_names[_months];
  }
}

void Calender::readConfig() {
  varconf::Config &config = System::instance()->getGeneral();
  varconf::Variable temp;

  temp = config.getItem(CALENDER, KEY_SECONDS_PER_MINUTE);
  _seconds_per_minute = (!temp.is_int()) ? (DEFAULT_SECONDS_PER_MINUTE) : ((int)temp);
  temp = config.getItem(CALENDER, KEY_MINUTES_PER_HOUR);
  _minutes_per_hour = (!temp.is_int()) ? (DEFAULT_MINUTES_PER_HOUR) : ((int)temp);
  temp = config.getItem(CALENDER, KEY_HOURS_PER_DAY);
  _hours_per_day = (!temp.is_int()) ? (DEFAULT_HOURS_PER_DAY) : ((int)temp);
  temp = config.getItem(CALENDER, KEY_DAYS_PER_WEEK);
  _days_per_week = (!temp.is_int()) ? (DEFAULT_DAYS_PER_WEEK) : ((int)temp);
  temp = config.getItem(CALENDER, KEY_WEEKS_PER_MONTH);
  _weeks_per_month = (!temp.is_int()) ? (DEFAULT_WEEKS_PER_MONTH) : ((int)temp);
  temp = config.getItem(CALENDER, KEY_MONTHS_PER_YEAR);
  _months_per_year = (!temp.is_int()) ? (DEFAULT_MONTHS_PER_YEAR) : ((int)temp);

  for (unsigned int i = 0; i < _days_per_week; ++i) {
    std::string key = KEY_DAY_NAME + string_fmt(i);
    temp = config.getItem(CALENDER, key);
    _day_names[i] = (!temp.is_string()) ? (key) : ((std::string)temp);
  }
  
  for (unsigned int i = 0; i < _months_per_year; ++i) {
    std::string key = KEY_MONTH_NAME + string_fmt(i);
    temp = config.getItem(CALENDER, key);
    _month_names[i] = (!temp.is_string()) ? (key) : ((std::string)temp);
  }
  _current_day_name = _day_names[_days];
  _current_month_name = _month_names[_months];
}

void Calender::writeConfig() {
  varconf::Config &config = System::instance()->getGeneral();
  config.setItem(CALENDER, KEY_SECONDS_PER_MINUTE, (int)_seconds_per_minute);
  config.setItem(CALENDER, KEY_MINUTES_PER_HOUR, (int)_minutes_per_hour);
  config.setItem(CALENDER, KEY_HOURS_PER_DAY, (int)_hours_per_day);
  config.setItem(CALENDER, KEY_DAYS_PER_WEEK, (int)_days_per_week);
  config.setItem(CALENDER, KEY_WEEKS_PER_MONTH, (int)_weeks_per_month);
  config.setItem(CALENDER, KEY_MONTHS_PER_YEAR, (int)_months_per_year);

  for (unsigned int i = 0; i < _days_per_week; ++i) {
    std::string key = KEY_DAY_NAME + string_fmt(i);
    config.setItem(CALENDER, key, _day_names[i]);
  }
  
  for (unsigned int i = 0; i < _months_per_year; ++i) {
    std::string key = KEY_MONTH_NAME + string_fmt(i);
    config.setItem(CALENDER, key, _month_names[i]);
  }

}

void Calender::config_update(const std::string &section, const std::string &key, varconf::Config &config) {
  if (section == CALENDER) {
    varconf::Variable temp;
    if (key == KEY_SECONDS_PER_MINUTE) {
      temp = config.getItem(CALENDER, KEY_SECONDS_PER_MINUTE);
      if (temp.is_int()) _seconds_per_minute = ((int)temp);
    }
    else if (key == KEY_MINUTES_PER_HOUR) {
      temp = config.getItem(CALENDER, KEY_MINUTES_PER_HOUR);
      if (temp.is_int()) _minutes_per_hour = ((int)temp);
    }
    else if (key == KEY_HOURS_PER_DAY) {
      temp = config.getItem(CALENDER, KEY_HOURS_PER_DAY);
      if (temp.is_int()) _hours_per_day = ((int)temp);
    }
    else if (key == KEY_DAYS_PER_WEEK) {
      temp = config.getItem(CALENDER, KEY_DAYS_PER_WEEK);
      if (temp.is_int()) _days_per_week = ((int)temp);
    }
    else if (key == KEY_WEEKS_PER_MONTH) {
      temp = config.getItem(CALENDER, KEY_WEEKS_PER_MONTH);
      if (temp.is_int()) _weeks_per_month = ((int)temp);
    }
    else if (key == KEY_MONTHS_PER_YEAR) {
      temp = config.getItem(CALENDER, KEY_MONTHS_PER_YEAR);
      if (temp.is_int()) _months_per_year = ((int)temp);
    }
    else if (key.substr(0, KEY_DAY_NAME.length()) == KEY_DAY_NAME) {
      temp = config.getItem(CALENDER, key);
      unsigned int index;
      cast_stream(key.substr(KEY_DAY_NAME.length()), index);
      if (temp.is_string()) _day_names[index] = ((std::string)temp);
    }
     else if (key.substr(0, KEY_MONTH_NAME.length()) == KEY_MONTH_NAME) {
      temp = config.getItem(CALENDER, key);
      unsigned int index;
      cast_stream(key.substr(KEY_MONTH_NAME.length()), index);
      if (temp.is_string()) _month_names[index] = ((std::string)temp);
    }
  }
  _current_day_name = _day_names[_days];
  _current_month_name = _month_names[_months];
}
 
void Calender::registerCommands(Console *console) {
  console->registerCommand(GET_TIME, this);
  console->registerCommand(SET_SECONDS, this);
  console->registerCommand(SET_MINUTES, this);
  console->registerCommand(SET_HOURS, this);
  console->registerCommand(SET_DAYS, this);
  console->registerCommand(SET_WEEKS, this);
  console->registerCommand(SET_MONTHS, this);
  console->registerCommand(SET_YEARS, this);
}

void Calender::runCommand(const std::string &command, const std::string &args) {
  if (command == GET_TIME) {
    std::string message = string_fmt(_hours) + ":" + string_fmt(_minutes) + ":" + string_fmt((int)_seconds) + " " + getDayName() + " " + string_fmt(_days + _weeks * _days_per_week + 1) + " of " + getMonthName() + " " + string_fmt(_years);
    System::instance()->pushMessage(message, 0x1);
  }
  else if (command == SET_SECONDS) {
    cast_stream(args, _seconds);
  }
  else if (command == SET_MINUTES) {
    cast_stream(args, _minutes);
  }
  else if (command == SET_HOURS) {
    cast_stream(args, _hours);
  }
  else if (command == SET_DAYS) {
    cast_stream(args, _days);
    // Update day name as well
    _current_day_name = _day_names[_days];
  }
  else if (command == SET_WEEKS) {
    cast_stream(args, _weeks);
  }
  else if (command == SET_MONTHS) {
    cast_stream(args, _months);
    // Update Month name as well
    _current_month_name = _month_names[_months];
  }
  else if (command == SET_YEARS) {
    cast_stream(args, _years);
  }
}

} /* namespace Sear */
