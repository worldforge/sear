// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2004 Simon Goodall

// $Id: Calendar.cpp,v 1.10 2004-05-17 10:39:28 simon Exp $

// TODO
// * Check all values are correctly updated on SET_ commands

#ifdef HAVE_CONFIG_H
  #include "config.h"
#endif

#include "Calendar.h"
#include "common/Utility.h"

#include "src/ActionHandler.h"
#include "src/Console.h"
#include "src/System.h"


#ifdef USE_MMGR
  #include "common/mmgr.h"
#endif

#ifdef DEBUG
  static const bool debug = true;
#else
  static const bool debug = false;
#endif

// Config section name
static const std::string CALENDER = "calendar";

// Config Key names
static const std::string KEY_SECONDS_PER_MINUTE = "seconds_per_minute";
static const std::string KEY_MINUTES_PER_HOUR = "minutes_per_hour";
static const std::string KEY_HOURS_PER_DAY = "hours_per_day";
static const std::string KEY_DAYS_PER_WEEK = "days_per_week";
static const std::string KEY_WEEKS_PER_MONTH = "weeks_per_month";
static const std::string KEY_MONTHS_PER_YEAR = "months_per_year";

static const std::string KEY_DAWN_START = "dawn_starts";
static const std::string KEY_DAY_START = "day_starts";
static const std::string KEY_DUSK_START = "dusk_starts";
static const std::string KEY_NIGHT_START = "night_starts";

// Config Key prefix's for day and month names
static const std::string KEY_DAY_NAME = "day_name_";
static const std::string KEY_MONTH_NAME = "month_name_";

// Default config values
static const unsigned int DEFAULT_SECONDS_PER_MINUTE = 20;
static const unsigned int DEFAULT_MINUTES_PER_HOUR = 60;
static const unsigned int DEFAULT_HOURS_PER_DAY = 24;
static const unsigned int DEFAULT_DAYS_PER_WEEK = 7;
static const unsigned int DEFAULT_WEEKS_PER_MONTH = 4;
static const unsigned int DEFAULT_MONTHS_PER_YEAR = 12;

static const unsigned int DEFAULT_DAWN_START = 6;
static const unsigned int DEFAULT_DAY_START = 9;
static const unsigned int DEFAULT_DUSK_START = 18;
static const unsigned int DEFAULT_NIGHT_START = 21;

// Console Commands
static const std::string GET_TIME = "get_time";
static const std::string SET_SECONDS = "set_seconds";
static const std::string SET_MINUTES = "set_minutes";
static const std::string SET_HOURS = "set_hours";
static const std::string SET_DAYS = "set_days";
static const std::string SET_WEEKS = "set_weeks";
static const std::string SET_MONTHS = "set_months";
static const std::string SET_YEARS =  "set_years";

// Action handler strings
static const std::string ACTION_DAWN  = "dawn";
static const std::string ACTION_DAY   = "day";
static const std::string ACTION_DUSK  = "dusk";
static const std::string ACTION_NIGHT = "night";

namespace Sear {

Calendar::Calendar() :
  _initialised(false),
  _seconds_per_minute(DEFAULT_SECONDS_PER_MINUTE),
  _minutes_per_hour(DEFAULT_MINUTES_PER_HOUR),
  _hours_per_day(DEFAULT_HOURS_PER_DAY),
  _days_per_week(DEFAULT_DAYS_PER_WEEK),
  _weeks_per_month(DEFAULT_WEEKS_PER_MONTH),
  _months_per_year(DEFAULT_MONTHS_PER_YEAR),
  _seconds(0.0f),
  _seconds_counter(0.0f),
  _minutes(0),
  _hours(12), // Default start time is noon
  _days(0),
  _weeks(0),
  _months(0),
  _years(0),
  _time_area(DAY),
  _dawn_start(DEFAULT_DAWN_START),
  _day_start(DEFAULT_DAY_START),
  _dusk_start(DEFAULT_DUSK_START),
  _night_start(DEFAULT_NIGHT_START),
  _time_in_area(0.0f)
{}
	
Calendar::~Calendar() {
  if (_initialised) shutdown();
}

void Calendar::init() {
  if (_initialised) shutdown(); // shutdown first if we are already initialised
  // Read in initial config
  readConfig();
  // Bind signal to config for further updates
  _config_connection = System::instance()->getGeneral().sigsv.connect(SigC::slot(*this, &Calendar::config_update));
  
  _initialised = true;
}

void Calendar::shutdown() {
  // Save config
  writeConfig();
  // Remove update signal
  _config_connection.disconnect();
  _initialised = false;
}

void Calendar::serverUpdate(double time) {
//  if (debug)
//  std::cout << "Current: " << _seconds << " - Server: " << time << std::endl;
  double diff = time - m_server_seconds;
  // TODO make this change gradual if too large
  // Could perhaps use the event manager to throw a series of events?
  /*
  if (abs(diff) < 60.0) {
    _seconds = time;
  } else {
    _seconds += (diff > 0.0) ? (60.0) : (-60.0);
  }
  */
//  _seconds = time;
  // Sync calendar values
  update(diff);
}

void Calendar::update(double time_elapsed) {
  assert ((_initialised == true) && "Calender not initialised");

  m_server_seconds += time_elapsed;

  _seconds += time_elapsed;
  _seconds_counter += time_elapsed;
  // Check for seconds overflow  
  while (_seconds >= _seconds_per_minute) {
    ++_minutes;
    _seconds -= _seconds_per_minute;
  } 
  // Check for minutes overflow
  while (_minutes >= _minutes_per_hour) {
    ++_hours;
    _minutes -= _minutes_per_hour;
  }
  
  // Check for hours overflow
  while (_hours >= _hours_per_day) {
    ++_days;
    // Update day name
    _current_day_name = _day_names[_days];
    _hours -= _hours_per_day;
  }
  // Check for days overflow
  while (_days >= _days_per_week) {
    ++_weeks;
    _days -= _days_per_week;
    _seconds_counter -= _seconds_per_minute * _minutes_per_hour * _hours_per_day;
    // Update day name
    _current_day_name = _day_names[_days];
  }
  // Check for weeks overflow
  while (_weeks >= _weeks_per_month) {
    ++_months;
    // Update month name
    _current_month_name = _month_names[_months];
    _weeks -= _weeks_per_month;
  }
  // Check for months overflow
  while (_months >= _months_per_year) {
    ++_years;
    _months -= _months_per_year;
    // Update month name
    _current_month_name = _month_names[_months];
  }
  // Update Time Area
  TimeArea ta = _time_area; // Store current time area
  // Set new time area
  if (_hours < _dawn_start) _time_area = NIGHT;
  else if (_hours < _day_start) _time_area = DAWN;
  else if (_hours < _dusk_start) _time_area = DAY;
  else if (_hours < _night_start) _time_area = DUSK;
  else if (_hours < _hours_per_day) _time_area = NIGHT;

  if (ta != _time_area) {
    unsigned int time_1 = 0;
    switch(_time_area) {
      case INVALID:
      case NIGHT: break;
      case DAWN: time_1 = _dawn_start; break;
      case DAY: time_1 = _day_start; break;
      case DUSK: time_1 = _dusk_start; break;
    }
    _time_in_area = _seconds_counter - (time_1 * _seconds_per_minute * _minutes_per_hour);
    // Emit an action event
    ActionHandler *action_handler = System::instance()->getActionHandler();
    switch(_time_area) {
      case INVALID: break;
      case NIGHT: action_handler->handleAction(ACTION_NIGHT, NULL); break;
      case DAWN:  action_handler->handleAction(ACTION_DAWN,  NULL); break;
      case DAY:   action_handler->handleAction(ACTION_DAY,   NULL); break;
      case DUSK:  action_handler->handleAction(ACTION_DUSK,  NULL); break;
    }
  } else {
    // Update _time_in_area
    _time_in_area += time_elapsed;
  }


}

void Calendar::readConfig() {
  varconf::Config &config = System::instance()->getGeneral();
  varconf::Variable temp;

  if (config.findItem(CALENDER, KEY_SECONDS_PER_MINUTE)) {
    temp = config.getItem(CALENDER, KEY_SECONDS_PER_MINUTE);
    _seconds_per_minute = (!temp.is_int()) ? (DEFAULT_SECONDS_PER_MINUTE) : ((int)temp);
  } else {
    _seconds_per_minute = DEFAULT_SECONDS_PER_MINUTE;
  }
  if (config.findItem(CALENDER, KEY_MINUTES_PER_HOUR)) {
    temp = config.getItem(CALENDER, KEY_MINUTES_PER_HOUR);
    _minutes_per_hour = (!temp.is_int()) ? (DEFAULT_MINUTES_PER_HOUR) : ((int)temp);
  } else {
    _minutes_per_hour = DEFAULT_MINUTES_PER_HOUR;
  }
  if (config.findItem(CALENDER, KEY_HOURS_PER_DAY)) {
    temp = config.getItem(CALENDER, KEY_HOURS_PER_DAY);
    _hours_per_day = (!temp.is_int()) ? (DEFAULT_HOURS_PER_DAY) : ((int)temp);
  } else {
    _hours_per_day = DEFAULT_HOURS_PER_DAY;
  }
  if (config.findItem(CALENDER, KEY_DAYS_PER_WEEK)) {
    temp = config.getItem(CALENDER, KEY_DAYS_PER_WEEK);
    _days_per_week = (!temp.is_int()) ? (DEFAULT_DAYS_PER_WEEK) : ((int)temp);
  } else {
    _days_per_week = DEFAULT_DAYS_PER_WEEK;
  }
  if (config.findItem(CALENDER, KEY_WEEKS_PER_MONTH)) {
    temp = config.getItem(CALENDER, KEY_WEEKS_PER_MONTH);
    _weeks_per_month = (!temp.is_int()) ? (DEFAULT_WEEKS_PER_MONTH) : ((int)temp);
  } else {
    _weeks_per_month = DEFAULT_WEEKS_PER_MONTH;
  }
  if (config.findItem(CALENDER, KEY_MONTHS_PER_YEAR)) {
    temp = config.getItem(CALENDER, KEY_MONTHS_PER_YEAR);
    _months_per_year = (!temp.is_int()) ? (DEFAULT_MONTHS_PER_YEAR) : ((int)temp);
  } else {
    _months_per_year = DEFAULT_MONTHS_PER_YEAR;
  }

  if (config.findItem(CALENDER, KEY_DAWN_START)) {
    temp = config.getItem(CALENDER, KEY_DAWN_START);
    _dawn_start = (!temp.is_int()) ? (DEFAULT_DAWN_START) : ((int)temp);
  } else {
    _dawn_start = DEFAULT_DAWN_START;
  }
  if (config.findItem(CALENDER, KEY_DAY_START)) {
    temp = config.getItem(CALENDER, KEY_DAY_START);
    _day_start = (!temp.is_int()) ? (DEFAULT_DAY_START) : ((int)temp);
  } else {
    _day_start = DEFAULT_DAY_START;
  }
  if (config.findItem(CALENDER, KEY_DUSK_START)) {
    temp = config.getItem(CALENDER, KEY_DUSK_START);
    _dusk_start = (!temp.is_int()) ? (DEFAULT_DUSK_START) : ((int)temp);
  } else {
    _dusk_start = DEFAULT_DUSK_START;
  }
  if (config.findItem(CALENDER, KEY_NIGHT_START)) {
    temp = config.getItem(CALENDER, KEY_NIGHT_START);
    _night_start = (!temp.is_int()) ? (DEFAULT_NIGHT_START) : ((int)temp);
  } else {
    _night_start = DEFAULT_NIGHT_START;
  }
  
  for (unsigned int i = 0; i < _days_per_week; ++i) {
    std::string key = KEY_DAY_NAME + string_fmt(i);
    if (config.findItem(CALENDER, key)) {
      temp = config.getItem(CALENDER, key);
      _day_names[i] = (!temp.is_string()) ? (key) : ((std::string)temp);
    } else {
      _day_names[i] = key;
    }
  }
  
  for (unsigned int i = 0; i < _months_per_year; ++i) {
    std::string key = KEY_MONTH_NAME + string_fmt(i);
    if (config.findItem(CALENDER, key)) {
      temp = config.getItem(CALENDER, key);
      _month_names[i] = (!temp.is_string()) ? (key) : ((std::string)temp);
    } else {
      _month_names[i] = key;
    }
  }
  _current_day_name = _day_names[_days];
  _current_month_name = _month_names[_months];
}

void Calendar::writeConfig() {
  assert ((_initialised == true) && "Calender not initialised");
  varconf::Config &config = System::instance()->getGeneral();
  config.setItem(CALENDER, KEY_SECONDS_PER_MINUTE, (int)_seconds_per_minute);
  config.setItem(CALENDER, KEY_MINUTES_PER_HOUR, (int)_minutes_per_hour);
  config.setItem(CALENDER, KEY_HOURS_PER_DAY, (int)_hours_per_day);
  config.setItem(CALENDER, KEY_DAYS_PER_WEEK, (int)_days_per_week);
  config.setItem(CALENDER, KEY_WEEKS_PER_MONTH, (int)_weeks_per_month);
  config.setItem(CALENDER, KEY_MONTHS_PER_YEAR, (int)_months_per_year);

  config.setItem(CALENDER, KEY_DAWN_START, (int)_dawn_start);
  config.setItem(CALENDER, KEY_DAY_START, (int)_day_start);
  config.setItem(CALENDER, KEY_DUSK_START, (int)_dusk_start);
  config.setItem(CALENDER, KEY_NIGHT_START, (int)_night_start);
  
  for (unsigned int i = 0; i < _days_per_week; ++i) {
    std::string key = KEY_DAY_NAME + string_fmt(i);
    config.setItem(CALENDER, key, _day_names[i]);
  }
  
  for (unsigned int i = 0; i < _months_per_year; ++i) {
    std::string key = KEY_MONTH_NAME + string_fmt(i);
    config.setItem(CALENDER, key, _month_names[i]);
  }

}

void Calendar::config_update(const std::string &section, const std::string &key, varconf::Config &config) {
  assert ((_initialised == true) && "Calender not initialised");
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
    else if (key == KEY_DAWN_START) {
      temp = config.getItem(CALENDER, KEY_DAWN_START);
      if (temp.is_int()) _dawn_start = ((int)temp);
    }
    else if (key == KEY_DAY_START) {
      temp = config.getItem(CALENDER, KEY_DAY_START);
      if (temp.is_int()) _day_start = ((int)temp);
    }
    else if (key == KEY_DUSK_START) {
      temp = config.getItem(CALENDER, KEY_DUSK_START);
      if (temp.is_int()) _dusk_start = ((int)temp);
    }
    else if (key == KEY_NIGHT_START) {
      temp = config.getItem(CALENDER, KEY_NIGHT_START);
      if (temp.is_int()) _night_start = ((int)temp);
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
 
void Calendar::registerCommands(Console *console) {
  assert ((_initialised == true) && "Calender not initialised");
  assert ((console != NULL) && "Console is NULL");
  console->registerCommand(GET_TIME, this);
  console->registerCommand(SET_SECONDS, this);
  console->registerCommand(SET_MINUTES, this);
  console->registerCommand(SET_HOURS, this);
  console->registerCommand(SET_DAYS, this);
  console->registerCommand(SET_WEEKS, this);
  console->registerCommand(SET_MONTHS, this);
  console->registerCommand(SET_YEARS, this);
}

void Calendar::runCommand(const std::string &command, const std::string &args) {
  assert ((_initialised == true) && "Calender not initialised");
  if (command == GET_TIME) {
    std::string message = string_fmt(_hours) + ":" + string_fmt(_minutes) + ":" + string_fmt((int)_seconds) + " " + getDayName() + " " + string_fmt(_days + _weeks * _days_per_week + 1) + " of " + getMonthName() + " " + string_fmt(_years);
    System::instance()->pushMessage(message, 0x1);
  }
  else if (command == SET_SECONDS) {
    cast_stream(args, _seconds);
    // Calculate new seconds_counter value
    _seconds_counter = (_minutes_per_hour * _hours + _minutes) * _seconds_per_minute + _seconds;
    // Force update of time area
    _time_area = INVALID;
    
  }
  else if (command == SET_MINUTES) {
    cast_stream(args, _minutes);
    // Calculate new seconds_counter value
    _seconds_counter = (_minutes_per_hour * _hours + _minutes) * _seconds_per_minute + _seconds;
    // Force update of time area
    _time_area = INVALID;
  }
  else if (command == SET_HOURS) {
    cast_stream(args, _hours);
    // Calculate new seconds_counter value
    _seconds_counter = (_minutes_per_hour * _hours + _minutes) * _seconds_per_minute + _seconds;
    // Force update of time area
    _time_area = INVALID;
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
