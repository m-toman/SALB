/**
 * (c) 2013 Markus Toman, FTW GmbH
 *
 * HTS TTS system
 **/

#ifndef FTW_TTSALBANIANNORMALIZER
#define FTW_TTSALBANIANNORMALIZER

#include <string>
#include <memory>
#include <iostream>

namespace htstts {

/**
 * Normalizes text (e.g.: "1000" -> "thousand"/"tausend")
 *
 **/
class AlbanianNormalizer {
   private:
      AlbanianNormalizer();
      static AlbanianNormalizer instance;
   public:

      static AlbanianNormalizer& GetInstance() {
         return instance;
      }

      ~AlbanianNormalizer();

      /**
       * Normalizes a sentence.
       **/
      std::string Normalize(const std::string& text);


};

}

#endif