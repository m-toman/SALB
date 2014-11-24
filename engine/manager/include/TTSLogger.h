/**
 * (c) 2013 Markus Toman, FTW GmbH
 *
 * HTS TTS system
 * Logger
 **/

#ifndef FTW_TTSLOGGER
#define FTW_TTSLOGGER

#include <iostream>
#include <fstream>
#include <string>

namespace htstts {

namespace log {

//TODO: remove
//#define TTSLOGGER_ENABLE_LOG


#ifdef TTSLOGGER_ENABLE_LOG
void set_logfile(const std::string& path);
void set_logfile(const char* path);
void reopen_logfile();
void close_logfile();
std::ofstream& get_logfile();

//TODO: add some log prefix (date, time)
//      only log if log level is debug
//      use operator<< in a new logging class instead? (nicer vs. unnecessary calls)
#define LOG_SETFILE( path ) htstts::log::set_logfile( path );
//#define LOG_DEBUG( msg ) if( htstts::log::get_logfile().is_open() ) { htstts::log::get_logfile() << msg << std::endl; }
#define LOG_DEBUG( msg ) { htstts::log::reopen_logfile(); htstts::log::get_logfile() << msg << std::endl; htstts::log::close_logfile(); }
#else
#define LOG_DEBUG( msg )
#define LOG_SETFILE( path )
#endif



}

}

#endif