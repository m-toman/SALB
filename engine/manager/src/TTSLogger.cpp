/**
 * (c) 2013 Markus Toman, FTW GmbH
 *
 * HTS TTS system
 * Logger
 **/


#include "TTSLogger.h"


namespace htstts {
namespace log {

/**
 * Helper class for closing logfile on destruction.
 **/
class PrivateLogger {
   public:
      PrivateLogger() {
      }
      ~PrivateLogger() {
         logFile.close();
      }
      void open(const std::string& path) {
         logPath = path;
         logFile.open(path, std::ios::app);
      }
      void open(const char* path) {
         logPath = std::string(path);
         logFile.open(path, std::ios::app);
      }
      void close() {
         logFile.close();
      }
      void reopen() {
         try {
            if (logFile.is_open()) {
               logFile.close();
            }
         }
         catch (std::exception&) { }
         logFile.clear();
         logFile.open(logPath, std::ios::app);
      }
      std::string logPath;
      std::ofstream logFile;
};

static PrivateLogger logger;

void set_logfile(const std::string& path) {
   logger.open(path);
}

void set_logfile(const char* path) {
   logger.open(path);
}

void close_logfile() {
   logger.close();
}

void reopen_logfile() {
   logger.reopen();
}

std::ofstream& get_logfile() {
   return logger.logFile;
}
}
}