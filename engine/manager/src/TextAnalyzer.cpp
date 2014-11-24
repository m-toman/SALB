/**
* (c) 2013 Markus Toman, FTW GmbH
*
* HTS TTS system
**/


#include <memory>

#include "TextAnalyzer.h"
#include "Text.h"
#include "Label.h"

#ifndef NOFLITE
#include "FliteTextAnalyzer.h"
#endif
#include "InternalTextAnalyzer.h"

namespace htstts {

/**
 * NewAnalyer
 **/
TextAnalyzerPtr TextAnalyzer::NewAnalyzer(const std::string& type) {
   if (type == PROPERTY_VALUE_INTERNAL) {
      return std::make_shared< InternalTextAnalyzer >();
   }
#ifndef NOFLITE
   if (type == PROPERTY_VALUE_FLITE) {
      return std::make_shared< FliteTextAnalyzer >();
   }
#endif
   else {
      return std::make_shared< InternalTextAnalyzer >();
   }
}


/**
* TextAnalyzer
**/
TextAnalyzer::TextAnalyzer() {
}


/**
* ~TextAnalyzer
**/
TextAnalyzer::~TextAnalyzer() {
}


}