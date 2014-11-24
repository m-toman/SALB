/**
 * (c) 2013 Markus Toman, FTW GmbH
 *
 * HTS TTS system
 **/

#ifndef FTW_TTSNORMALIZER
#define FTW_TTSNORMALIZER

#include <string>
#include <memory>
#include <iostream>

namespace htstts {

/**
 * Normalizes text (e.g.: "1000" -> "thousand"/"tausend")
 *
 **/
class Normalizer {
   private:

   public:
      Normalizer();
      ~Normalizer();

      /**
       * Normalizes a sentence.
       **/
      std::string Normalize(const std::string& text, const std::string& language);


};

}

#endif