// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2004 Simon Goodall

// $Id: Calendar.cpp,v 1.17 2005-04-28 20:31:37 simon Exp $

// TODO
// * Check all values are correctly updated on SET_ commands


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
  m_initialised(false),
  m_seconds_per_minute(DEFAULT_SECONDS_PER_MINUTE),
  m_minutes_per_hour(DEFAULT_MINUTES_PER_HOUR),
  m_hours_per_day(DEFAULT_HOURS_PER_DAY),
  m_days_per_week(DEFAULT_DAYS_PER_WEEK),
  m_weeks_per_month(DEFAULT_WEEKS_PER_MONTH),
  m_months_per_year(DEFAULT_MONTHS_PER_YEAR),
  m_seconds(0.0f),
  m_server_seconds(0.0),
  m_seconds_counter(0.0f),
  m_minutes(0),
  m_hours(0), // Default start time is midnight
  m_days(0),
  m_weeks(0),
  m_months(0),
  m_years(0),
  m_time_area(DAY),
  m_dawn_start(DEFAULT_DAWN_START),
  m_day_start(DEFAULT_DAY_START),
  m_dusk_start(DEFAULT_DUSK_START),
  m_night_start(DEFAULT_NIGHT_START),
  m_time_in_area(0.0f)
{}
	
Calendar::~Calendar() {
  assert(m_initialised == false);
}

void Calendar::init() {
  assert(m_initialised == false);
  // Bind signal to config for further updates
  m_config_connection = System::instance()->getGeneral().sigsv.connect(SigC::slot(*this, &Calendar::config_update));
  
  m_ts = WFMath::TimeStamp::epochStart();
  m_initialised = true;
}

void Calendar::shutdown() {
  assert(m_initialised == true);
  // Remove update signal
  m_config_connection.disconnect();
  m_initialised = false;
}

void Calendar::serverUpdate(double time) {
  double diff = time - m_server_seconds;
  // Sync calendar values
  update(diff);
}

void Calendar::setWorldTime(const WFMath::TimeStamp &ts) {
  assert ((m_initialised == true) && "Calender not initialised");
  assert(ts.isValid());
  assert(m_ts.isValid());

  WFMath::TimeDiff df = ts - m_ts;
  m_ts = ts;
  double time = (double)df.milliseconds() / 1000.0;

  update(time);
}

void Calendar::update(double time_elapsed) {
  assert ((m_initialised == true) && "Calender not initialised");

  m_server_seconds += time_elapsed;

  m_seconds += time_elapsed;
  m_seconds_counter += time_elapsed;
  // Check for seconds overflow  
  while (m_seconds >= m_seconds_per_minute) {
    ++m_minutes;
    m_seconds -= m_seconds_per_minute;
  } 
  // Check for minutes overflow
  while (m_minutes >= m_minutes_per_hour) {
    ++m_hours;
    m_minutes -= m_minutes_per_hour;
  }
  
  // Check for hours overflow
  while (m_hours >= m_hours_per_day) {
    ++m_days;
    // Update day name
    m_current_day_name = m_day_names[m_days];
    m_hours -= m_hours_per_day;
  }
  // Check for days overflow
  while (m_days >= m_days_per_week) {
    ++m_weeks;
    m_days -= m_days_per_week;
    m_seconds_counter -= m_seconds_per_minute * m_minutes_per_hour * m_hours_per_day;
    // Update day name
    m_current_day_name = m_day_names[m_days];
  }
  // Check for weeks overflow
  while (m_weeks >= m_weeks_per_month) {
    ++m_months;
    // Update month name
    m_current_month_name = m_month_names[m_months];
    m_weeks -= m_weeks_per_month;
  }
  // Check for months overflow
  while (m_months >= m_months_per_year) {
    ++m_years;
    m_months -= m_months_per_year;
    // Update month name
    m_current_month_name = m_month_names[m_months];
  }
  // Update Time Area
  TimeArea ta = m_time_area; // Store current time area
  // Set new time area
  if (m_hours < m_dawn_start) m_time_area = NIGHT;
  else if (m_hours < m_day_start) m_time_area = DAWN;
  else if (m_hours < m_dusk_start) m_time_area = DAY;
  else if (m_hours < m_night_start) m_time_area = DUSK;
  else if (m_hours < m_hours_per_day) m_time_area = NIGHT;

  if (ta != m_time_area) {
    unsigned int time_1 = 0;
    switch(m_time_area) {
      case INVALID:
      case NIGHT: break;
      case DAWN: time_1 = m_dawn_start; break;
      case DAY: time_1 = m_day_start; break;
      case DUSK: time_1 = m_dusk_start; break;
    }
    m_time_in_area = m_seconds_counter - (time_1 * m_seconds_per_minute * m_minutes_per_hour);
    // Emit an action event
    ActionHandler *action_handler = System::instance()->getActionHandler();
    switch(m_time_area) {
      case INVALID: break;
      case NIGHT: action_handler->handleAction(ACTION_NIGHT, NULL); break;
      case DAWN:  action_handler->handleAction(ACTION_DAWN,  NULL); break;
      case DAY:   action_handler->handleAction(ACTION_DAY,   NULL); break;
      case DUSK:  action_handler->handleAction(ACTION_DUSK,  NULL); break;
    }
  } else {
    // Update m_time_in_area
    m_time_in_area += time_elapsed;
  }


}

void Calendar::readConfig(varconf::Config &config) {

  varconf::Variable temp;

  if (config.findItem(CALENDER, KEY_SECONDS_PER_MINUTE)) {
    temp = config.getItem(CALENDER, KEY_SECONDS_PER_MINUTE);
    m_seconds_per_minute = (!temp.is_int()) ? (DEFAULT_SECONDS_PER_MINUTE) : ((int)temp);
  } else {
    m_seconds_per_minute = DEFAULT_SECONDS_PER_MINUTE;
  }
  if (config.findItem(CALENDER, KEY_MINUTES_PER_HOUR)) {
    temp = config.getItem(CALENDER, KEY_MINUTES_PER_HOUR);
    m_minutes_per_hour = (!temp.is_int()) ? (DEFAULT_MINUTES_PER_HOUR) : ((int)temp);
  } else {
    m_minutes_per_hour = DEFAULT_MINUTES_PER_HOUR;
  }
  if (config.findItem(CALENDER, KEY_HOURS_PER_DAY)) {
    temp = config.getItem(CALENDER, KEY_HOURS_PER_DAY);
    m_hours_per_day = (!temp.is_int()) ? (DEFAULT_HOURS_PER_DAY) : ((int)temp);
  } else {
    m_hours_per_day = DEFAULT_HOURS_PER_DAY;
  }
  if (config.findItem(CALENDER, KEY_DAYS_PER_WEEK)) {
    temp = config.getItem(CALENDER, KEY_DAYS_PER_WEEK);
    m_days_per_week = (!temp.is_int()) ? (DEFAULT_DAYS_PER_WEEK) : ((int)temp);
  } else {
    m_days_per_week = DEFAULT_DAYS_PER_WEEK;
  }
  if (config.findItem(CALENDER, KEY_WEEKS_PER_MONTH)) {
    temp = config.getItem(CALENDER, KEY_WEEKS_PER_MONTH);
    m_weeks_per_month = (!temp.is_int()) ? (DEFAULT_WEEKS_PER_MONTH) : ((int)temp);
  } else {
    m_weeks_per_month = DEFAULT_WEEKS_PER_MONTH;
  }
  if (config.findItem(CALENDER, KEY_MONTHS_PER_YEAR)) {
    temp = config.getItem(CALENDER, KEY_MONTHS_PER_YEAR);
    m_months_per_year = (!temp.is_int()) ? (DEFAULT_MONTHS_PER_YEAR) : ((int)temp);
  } else {
    m_months_per_year = DEFAULT_MONTHS_PER_YEAR;
  }

  if (config.findItem(CALENDER, KEY_DAWN_START)) {
    temp = config.getItem(CALENDER, KEY_DAWN_START);
    m_dawn_start = (!temp.is_int()) ? (DEFAULT_DAWN_START) : ((int)temp);
  } else {
    m_dawn_start = DEFAULT_DAWN_START;
  }
  if (config.findItem(CALENDER, KEY_DAY_START)) {
    temp = config.getItem(CALENDER, KEY_DAY_START);
    m_day_start = (!temp.is_int()) ? (DEFAULT_DAY_START) : ((int)temp);
  } else {
    m_day_start = DEFAULT_DAY_START;
  }
  if (config.findItem(CALENDER, KEY_DUSK_START)) {
    temp = config.getItem(CALENDER, KEY_DUSK_START);
    m_dusk_start = (!temp.is_int()) ? (DEFAULT_DUSK_START) : ((int)temp);
  } else {
    m_dusk_start = DEFAULT_DUSK_START;
  }
  if (config.findItem(CALENDER, KEY_NIGHT_START)) {
    temp = config.getItem(CALENDER, KEY_NIGHT_START);
    m_night_start = (!temp.is_int()) ? (DEFAULT_NIGHT_START) : ((int)temp);
  } else {
    m_night_start = DEFAULT_NIGHT_START;
  }
  
  for (unsigned int i = 0; i < m_days_per_week; ++i) {
    std::string key = KEY_DAY_NAME + string_fmt(i);
    if (config.findItem(CALENDER, key)) {
      temp = config.getItem(CALENDER, key);
      m_day_names[i] = (!temp.is_string()) ? (key) : ((std::string)temp);
    } else {
      m_day_names[i] = key;
    }
  }
  
  for (unsigned int i = 0; i < m_months_per_year; ++i) {
    std::string key = KEY_MONTH_NAME + string_fmt(i);
    if (config.findItem(CALENDER, key)) {
      temp = config.getItem(CALENDER, key);
      m_month_names[i] = (!temp.is_string()) ? (key) : ((std::string)temp);
    } else {
      m_month_names[i] = key;
    }
  }
  m_current_day_name = m_day_names[m_days];
  m_current_month_name = m_month_names[m_months];
}

void Calendar::writeConfig(varconf::Config &config) {
  assert ((m_initialised == true) && "Calender not initialised");

  config.setItem(CALENDER, KEY_SECONDS_PER_MINUTE, (int)m_seconds_per_minute);
  config.setItem(CALENDER, KEY_MINUTES_PER_HOUR, (int)m_minutes_per_hour);
  config.setItem(CALENDER, KEY_HOURS_PER_DAY, (int)m_hours_per_day);
  config.setItem(CALENDER, KEY_DAYS_PER_WEEK, (int)m_days_per_week);
  config.setItem(CALENDER, KEY_WEEKS_PER_MONTH, (int)m_weeks_per_month);
  config.setItem(CALENDER, KEY_MONTHS_PER_YEAR, (int)m_months_per_year);

  config.setItem(CALENDER, KEY_DAWN_START, (int)m_dawn_start);
  config.setItem(CALENDER, KEY_DAY_START, (int)m_day_start);
  config.setItem(CALENDER, KEY_DUSK_START, (int)m_dusk_start);
  config.setItem(CALENDER, KEY_NIGHT_START, (int)m_night_start);
  
  for (unsigned int i = 0; i < m_days_per_week; ++i) {
    std::string key = KEY_DAY_NAME + string_fmt(i);
    config.setItem(CALENDER, key, m_day_names[i]);
  }
  
  for (unsigned int i = 0; i < m_months_per_year; ++i) {
    std::string key = KEY_MONTH_NAME + string_fmt(i);
    config.setItem(CALENDER, key, m_month_names[i]);
  }

}

void Calendar::config_update(const std::string &section, const std::string &key, varconf::Config &config) {
  assert ((m_initialised == true) && "Calender not initialised");
  if (section == CALENDER) {
    varconf::Variable temp;
    if (key == KEY_SECONDS_PER_MINUTE) {
      temp = config.getItem(CALENDER, KEY_SECONDS_PER_MINUTE);
      if (temp.is_int()) m_seconds_per_minute = ((int)temp);
    }
    else if (key == KEY_MINUTES_PER_HOUR) {
      temp = config.getItem(CALENDER, KEY_MINUTES_PER_HOUR);
      if (temp.is_int()) m_minutes_per_hour = ((int)temp);
    }
    else if (key == KEY_HOURS_PER_DAY) {
      temp = config.getItem(CALENDER, KEY_HOURS_PER_DAY);
      if (temp.is_int()) m_hours_per_day = ((int)temp);
    }
    else if (key == KEY_DAYS_PER_WEEK) {
      temp = config.getItem(CALENDER, KEY_DAYS_PER_WEEK);
      if (temp.is_int()) m_days_per_week = ((int)temp);
    }
    else if (key == KEY_WEEKS_PER_MONTH) {
      temp = config.getItem(CALENDER, KEY_WEEKS_PER_MONTH);
      if (temp.is_int()) m_weeks_per_month = ((int)temp);
    }
    else if (key == KEY_MONTHS_PER_YEAR) {
      temp = config.getItem(CALENDER, KEY_MONTHS_PER_YEAR);
      if (temp.is_int()) m_months_per_year = ((int)temp);
    }
    else if (key == KEY_DAWN_START) {
      temp = config.getItem(CALENDER, KEY_DAWN_START);
      if (temp.is_int()) m_dawn_start = ((int)temp);
    }
    else if (key == KEY_DAY_START) {
      temp = config.getItem(CALENDER, KEY_DAY_START);
      if (temp.is_int()) m_day_start = ((int)temp);
    }
    else if (key == KEY_DUSK_START) {
      temp = config.getItem(CALENDER, KEY_DUSK_START);
      if (temp.is_int()) m_dusk_start = ((int)temp);
    }
    else if (key == KEY_NIGHT_START) {
      temp = config.getItem(CALENDER, KEY_NIGHT_START);
      if (temp.is_int()) m_night_start = ((int)temp);
    }

    else if (key.substr(0, KEY_DAY_NAME.length()) == KEY_DAY_NAME) {
      temp = config.getItem(CALENDER, key);
      unsigned int index;
      cast_stream(key.substr(KEY_DAY_NAME.length()), index);
      if (temp.is_string()) m_day_names[index] = ((std::string)temp);
    }
     else if (key.substr(0, KEY_MONTH_NAME.length()) == KEY_MONTH_NAME) {
      temp = config.getItem(CALENDER, key);
      unsigned int index;
      cast_stream(key.substr(KEY_MONTH_NAME.length()), index);
      if (temp.is_string()) m_month_names[index] = ((std::string)temp);
    }
  }
  m_current_day_name = m_day_names[m_days];
  m_current_month_name = m_month_names[m_months];
}
 
void Calendar::registerCommands(Console *console) {
  assert ((m_initialised == true) && "Calender not initialised");
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
  assert ((m_initialised == true) && "Calender not initialised");
  if (command == GET_TIME) {
    std::string message = string_fmt(m_hours) + ":" + string_fmt(m_minutes) + ":" + string_fmt((int)m_seconds) + " " + getDayName() + " " + string_fmt(m_days + m_weeks * m_days_per_week + 1) + " of " + getMonthName() + " " + string_fmt(m_years);
    System::instance()->pushMessage(message, 0x1);
  }
  else if (command == SET_SECONDS) {
    cast_stream(args, m_seconds);
    // Calculate new seconds_counter value
    m_seconds_counter = (m_minutes_per_hour * m_hours + m_minutes) * m_seconds_per_minute + m_seconds;
    // Force update of time area
    m_time_area = INVALID;
    
  }
  else if (command == SET_MINUTES) {
    cast_stream(args, m_minutes);
    // Calculate new seconds_counter value
    m_seconds_counter = (m_minutes_per_hour * m_hours + m_minutes) * m_seconds_per_minute + m_seconds;
    // Force update of time area
    m_time_area = INVALID;
  }
  else if (command == SET_HOURS) {
    cast_stream(args, m_hours);
    // Calculate new seconds_counter value
    m_seconds_counter = (m_minutes_per_hour * m_hours + m_minutes) * m_seconds_per_minute + m_seconds;
    // Force update of time area
    m_time_area = INVALID;
  }
  else if (command == SET_DAYS) {
    cast_stream(args, m_days);
    // Update day name as well
    m_current_day_name = m_day_names[m_days];
  }
  else if (command == SET_WEEKS) {
    cast_stream(args, m_weeks);
  }
  else if (command == SET_MONTHS) {
    cast_stream(args, m_months);
    // Update Month name as well
    m_current_month_name = m_month_names[m_months];
  }
  else if (command == SET_YEARS) {
    cast_stream(args, m_years);
  }
}

} /* namespace Sear */
