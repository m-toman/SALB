/**
* (c) 2013 Markus Toman, FTW GmbH
*
* HTS TTS system
**/


#include <stdio.h>
#include <string>
#include <sstream>

#include "utils.h"


namespace htstts {

static void _save_riff(TTSResultPtr& result, FILE* fp);

//=============================== PUBLIC =====================================

/**
* save_result_riff
* TODO: use streams instead of fopen
**/
void save_result_riff(TTSResultPtr& result, const std::string& path) {
   FILE* fp = fopen(path.c_str(), "wb");
   if (fp) {
      _save_riff(result, fp);
      fclose(fp);
   }
}


/**
* lowercase a string, considering utf-8 characters
* this currently only handles german umlaute
**/
void utf8_tolower(std::string& text) {
   for (std::string::size_type i = 0; i < text.length();  i += UTF8_CHAR_LEN(text[i])) {
      if (UTF8_CHAR_LEN(text[i]) == 1) {
         text[i] = ::tolower(text[i]);
      }
      else {
         if (text[i] == '\xc3') {
            // AE umlaut
            if (text[i + 1] == '\x84') {
               text[i + 1] = '\xa4';
            }
            // OE umlaut
            else if (text[i + 1] == '\x96') {
               text[i + 1] = '\xb6';
            }
            // UE umlaut
            else if (text[i + 1] == '\x9c') {
               text[i + 1] = '\xbc';
            }
         }
      }
   }
}

std::string::size_type utf8_find_first_umlaut(const std::string& text) {
   for (std::string::size_type i = 0; i < text.length();  i += UTF8_CHAR_LEN(text[i])) {
      if (UTF8_CHAR_LEN(text[i]) > 1) {
         if (text[i] == '\xc3') {
            // AE umlaut   // OE umlaut  // UE umlaut
            std::string::size_type t = i + 1;
            if (text[t] == '\x84' || text[t] == '\xa4'
                  || text[t] == '\x96' || text[t] == '\xb6'
                  || text[t] == '\x9c' || text[t] == '\xbc') {
               return t;
            }
         }
      }
   }
   return std::string::npos;
}

std::string utf8_remove_wide(const std::string& text) {
   std::stringstream ret;

   for (std::string::size_type i = 0; i < text.length(); i += UTF8_CHAR_LEN(text[i])) {
      if (UTF8_CHAR_LEN(text[i]) == 1) {
         ret << text[i];
      }
   }
   return ret.str();
}


//=============================== INTERNAL =====================================


/**
* _fwrite_little_endian: fwrite with byteswap
**/
static size_t _fwrite_little_endian(const void* buf, size_t size, size_t n, FILE* fp) {
#ifdef WORDS_BIGENDIAN
   HTS_byte_swap(buf, size, n * size);
#endif                          /* WORDS_BIGENDIAN */
   return fwrite(buf, size, n, fp);
}

/**
* save riff
**/
static void _save_riff(TTSResultPtr& result, FILE* fp) {
   std::vector<double>& samples = result->GetFrames();
   size_t i;
   double x;
   short temp;

   // build header
   char data_01_04[] = { 'R', 'I', 'F', 'F' };
   int data_05_08 = (int)samples.size() * sizeof(short) + 36;
   char data_09_12[] = { 'W', 'A', 'V', 'E' };
   char data_13_16[] = { 'f', 'm', 't', ' ' };
   int data_17_20 = 16;
   short data_21_22 = 1;        // PCM
   short data_23_24 = 1;        // monoral
   int data_25_28 = result->GetSamplingRate();
   int data_29_32 = result->GetSamplingRate() * sizeof(short);
   short data_33_34 = sizeof(short);
   short data_35_36 = (short)(sizeof(short) * 8);
   char data_37_40[] = { 'd', 'a', 't', 'a' };
   int data_41_44 = (int)samples.size() * sizeof(short);

   // write header
   _fwrite_little_endian(data_01_04, sizeof(char), 4, fp);
   _fwrite_little_endian(&data_05_08, sizeof(int), 1, fp);
   _fwrite_little_endian(data_09_12, sizeof(char), 4, fp);
   _fwrite_little_endian(data_13_16, sizeof(char), 4, fp);
   _fwrite_little_endian(&data_17_20, sizeof(int), 1, fp);
   _fwrite_little_endian(&data_21_22, sizeof(short), 1, fp);
   _fwrite_little_endian(&data_23_24, sizeof(short), 1, fp);
   _fwrite_little_endian(&data_25_28, sizeof(int), 1, fp);
   _fwrite_little_endian(&data_29_32, sizeof(int), 1, fp);
   _fwrite_little_endian(&data_33_34, sizeof(short), 1, fp);
   _fwrite_little_endian(&data_35_36, sizeof(short), 1, fp);
   _fwrite_little_endian(data_37_40, sizeof(char), 4, fp);
   _fwrite_little_endian(&data_41_44, sizeof(int), 1, fp);

   // write data
   for (i = 0; i < samples.size(); i++) {
      x = samples[i];
      if (x > 32767.0) {
         temp = 32767;
      }
      else if (x < -32768.0) {
         temp = -32768;
      }
      else {
         temp = (short) x;
      }
      _fwrite_little_endian(&temp, sizeof(short), 1, fp);
   }
}

}