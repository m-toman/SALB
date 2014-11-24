/**
 * (c) 2013 Markus Toman, FTW GmbH
 *
 * HTS TTS system
 **/

#ifndef FTW_TTSAUSTRIANGERMANNORMALIZER
#define FTW_TTSAUSTRIANGERMANNORMALIZER

#include <string>
#include <memory>
#include <iostream>

namespace htstts {

/**
 * Normalizes text (e.g.: "1000" -> "thousand"/"tausend")
 *
 **/
class AustrianGermanNormalizer {
   private:
      AustrianGermanNormalizer();
      static AustrianGermanNormalizer instance;
   public:

      static AustrianGermanNormalizer& GetInstance() {
         return instance;
      }

      ~AustrianGermanNormalizer();

      /**
       * Normalizes a sentence.
       **/
      std::string Normalize(const std::string& text);


};

}

#endif