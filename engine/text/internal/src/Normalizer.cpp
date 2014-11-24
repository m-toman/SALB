/**
* (c) 2013 Markus Toman, FTW GmbH
*
* HTS TTS system
**/

#include <string>
#include <iostream>
#include <sstream>
#include <cctype>

#include "utils.h"
#include "Normalizer.h"

#include "AustrianGermanNormalizer.h"
#include "AlbanianNormalizer.h"

namespace htstts {

Normalizer::Normalizer() {
}

Normalizer::~Normalizer() {
}

/*
* Normalize
*/
std::string Normalizer::Normalize(const std::string& text, const std::string& language) {
   // KISS principle ;)
   if (language.substr(0, 2) == "de") {
      return AustrianGermanNormalizer::GetInstance().Normalize(text);
   }
   else if (language.substr(0, 2) == "sq") {
      return AlbanianNormalizer::GetInstance().Normalize(text);
   }
   else {
      return text;
   }
}



}