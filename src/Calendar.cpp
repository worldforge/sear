// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2006 Simon Goodall

// $Id: Calendar.cpp,v 1.25 2006-07-15 11:59:22 simon Exp $

// TODO
// * Check all values are correctly updated on SET_ commands

#include <sigc++/object_slot.h>

#include <Eris/Calendar.h>

#include "Calendar.h"
#include "common/Utility.h"

#include "src/ActionHandler.h"
#include "src/Console.h"
#include "src/System.h"

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
static const std::string KEY_MONTHS_PER_YEAR = "months_per_year";

static const std::string KEY_DAWN_START = "dawn_starts";
static const std::string KEY_DAY_START = "day_starts";
static const std::string KEY_DUSK_START = "dusk_starts";
static const std::string KEY_NIGHT_START = "night_starts";

static const int DEFAULT_DAWN_START = 6;
static const int DEFAULT_DAY_START = 9;
static const int DEFAULT_DUSK_START = 18;
static const int DEFAULT_NIGHT_START = 21;

// Console Commands
static const std::string GET_TIME = "get_time";
static const std::string SET_SECONDS = "set_seconds";
static const std::string SET_MINUTES = "set_minutes";
static const std::string SET_HOURS = "set_hours";
static const std::string SET_DAYS = "set_days";
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
  m_seconds_per_minute(1),
  m_minutes_per_hour(1),
  m_hours_per_day(1),
  m_server_seconds(0.0),
  m_minutes(0),
  m_hours(0), // Default start time is midnight
  m_days(0),
  m_months(0),
  m_years(0),
  m_time_area(INVALID),
  m_dawn_start(DEFAULT_DAWN_START),
  m_day_start(DEFAULT_DAY_START),
  m_dusk_start(DEFAULT_DUSK_START),
  m_night_start(DEFAULT_NIGHT_START),
  m_time_in_area(0.0f),
  m_firstUpdate(true)
{}
	
Calendar::~Calendar() {
  assert(m_initialised == false);
}

void Calendar::init() {
  assert(m_initialised == false);
  // Bind signal to config for further updates
  System::instance()->getGeneral().sigsv.connect(SigC::slot(*this, &Calendar::config_update));

  // Reset calendar on enter world. 
  System::instance()->EnteredWorld.connect(SigC::slot(*this, &Calendar::reset));
 
  m_initialised = true;
}

void Calendar::shutdown() {
  assert(m_initialised == true);

  m_cal.release();
 
  // Explicity Tell sigc to disconnect signals
  notify_callbacks();

  m_initialised = false;
}

void Calendar::setAvatar(Eris::Avatar *avatar)  {
  assert(m_initialised == true);

  if (avatar) {
    m_cal = SPtr<Eris::Calendar>(new Eris::Calendar(avatar));

    m_seconds_per_minute = m_cal->secondsPerMinute();
    m_minutes_per_hour   = m_cal->minutesPerHour();
    m_hours_per_day      = m_cal->hoursPerDay();
  } else {
    m_cal = SPtr<Eris::Calendar>();
  }
  reset();
}

void Calendar::update() {
  assert ((m_initialised == true) && "Calender not initialised");

//  if (!m_cal) return;

  Eris::DateTime dt = m_cal->now();

//  assert(dt.valid());
  if (dt.valid() == false) return;

  m_server_seconds = m_seconds;

  m_seconds = dt.seconds();
  m_minutes = dt.minutes();
  m_hours   = dt.hours();
  m_days    = dt.dayOfMonth();
  m_years   = dt.year();
/*
  // Need to update each time as initially there *could* be no 
  m_seconds_per_minute = m_cal->secondsPerMinute();
  m_minutes_per_hour   = m_cal->minutesPerHour();
  m_hours_per_day      = m_cal->hoursPerDay();
*/
  double time_elapsed = m_seconds - m_server_seconds;
  if (time_elapsed < 0.0) time_elapsed += m_seconds_per_minute;

  if (m_firstUpdate) {
    // initial update
    time_elapsed = 0.0;
    m_firstUpdate = false;
  }

  // Update Time Area
  TimeArea ta = m_time_area; // Store current time area
  // Set new time area
  if (m_hours < m_dawn_start) m_time_area = NIGHT;
  else if (m_hours < m_day_start) m_time_area = DAWN;
  else if (m_hours < m_dusk_start) m_time_area = DAY;
  else if (m_hours < m_night_start) m_time_area = DUSK;
  else if (m_hours < m_hours_per_day) m_time_area = NIGHT;

  int time_1 = 0;
  switch(m_time_area) {
    case INVALID:
    case NIGHT: time_1 = m_night_start; break;
    case DAWN:  time_1 = m_dawn_start;  break;
    case DAY:   time_1 = m_day_start;   break;
    case DUSK:  time_1 = m_dusk_start;  break;
  }

  if (ta != m_time_area) {
    // Emit an action event
    ActionHandler *action_handler = System::instance()->getActionHandler();
    switch(m_time_area) {
      case INVALID: break;
      case NIGHT: action_handler->handleAction(ACTION_NIGHT, NULL); break;
      case DAWN:  action_handler->handleAction(ACTION_DAWN,  NULL); break;
      case DAY:   action_handler->handleAction(ACTION_DAY,   NULL); break;
      case DUSK:  action_handler->handleAction(ACTION_DUSK,  NULL); break;
    }
  }

  m_time_in_area = m_hours - time_1;
  // Check for roll over
  if (m_time_in_area < 0.0) m_time_in_area += m_hours_per_day;
  m_time_in_area *= m_minutes_per_hour;
  m_time_in_area += m_minutes;
  m_time_in_area *= m_seconds_per_minute;
  m_time_in_area += m_seconds;
 
}

void Calendar::readConfig(varconf::Config &config) {
  varconf::Variable temp;

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
}

void Calendar::writeConfig(varconf::Config &config) {
  assert ((m_initialised == true) && "Calender not initialised");

  config.setItem(CALENDER, KEY_DAWN_START, (int)m_dawn_start);
  config.setItem(CALENDER, KEY_DAY_START, (int)m_day_start);
  config.setItem(CALENDER, KEY_DUSK_START, (int)m_dusk_start);
  config.setItem(CALENDER, KEY_NIGHT_START, (int)m_night_start);
}

void Calendar::config_update(const std::string &section, const std::string &key, varconf::Config &config) {
  assert ((m_initialised == true) && "Calender not initialised");
  if (section == CALENDER) {
    varconf::Variable temp;
    if (key == KEY_DAWN_START) {
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

  }
}
 
void Calendar::registerCommands(Console *console) {
  assert ((m_initialised == true) && "Calender not initialised");
  assert ((console != NULL) && "Console is NULL");

  console->registerCommand(GET_TIME, this);
}

void Calendar::runCommand(const std::string &command, const std::string &args) {
  assert ((m_initialised == true) && "Calender not initialised");
  if (command == GET_TIME) {
    const char fmt[] = "%02d:%02d:%02d %04d-%02d-%02d";
    char message[19]; // HH:MM:SS YYYY:MM:DD
    snprintf(message, 19, fmt, m_hours, m_minutes, (int)m_seconds, m_years, m_months, m_days);
    System::instance()->pushMessage(message, 0x1);
  }
}

} /* namespace Sear */
