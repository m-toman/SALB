/**
 * (c) 2013 Markus Toman, FTW GmbH
 *
 * HTS TTS system
 **/

#ifndef FTW_TTSUTILS
#define FTW_TTSUTILS

#include <memory>
#include <vector>
#include <map>

#include "common.h"
#include "TTSResult.h"

namespace htstts {


//============= WAV HANDLING ===============

void save_result_riff(TTSResultPtr& result, const std::string& path);

//============= UTF8 HANDLING ===============

// source: stackoverflow (http://stackoverflow.com/a/2954379)
// for a given first UTF-8 byte, returns the number of bytes of the full character
#define UTF8_CHAR_LEN( byte ) (( 0xE5000000 >> (( byte >> 3 ) & 0x1e )) & 3 ) + 1

/**
 * Lowercase a string, considering utf-8 characters
 * this currently only handles german umlaute
 **/
void utf8_tolower(std::string& text);

/**
* Finds the position of the first German Umlaut.
* Returns std::string::npos if none found.
**/
std::string::size_type utf8_find_first_umlaut(const std::string& text);

/**
* Removes all multibyte UTF-8 characters from text.
* Returns a new string containing only single-byte characters.
**/
std::string utf8_remove_wide(const std::string& text);

}

#endif