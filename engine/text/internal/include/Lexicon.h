/**
 * (c) 2013 Markus Toman, FTW GmbH
 *
 * HTS TTS system
 **/

#ifndef FTW_TTSLEXICON
#define FTW_TTSLEXICON

#include <string>
#include <vector>
#include <memory>
#include <iostream>
#include <unordered_map>
#include <map>

#include "Utterance.h"

namespace htstts {

/**
 * Represents a lexicon with mappings from words to phonetic/syllable information.
 *
 **/
class Lexicon {
   private:

      /**
       * Holds a lexicon entry.
       * This structure currently only holds a WordPtr
       * but might become more complex when word variants are stored in the lexicon.
       **/
      struct LexiconEntry {
         utt::WordPtr word;
         std::string content;
      };

      /** Holds all lexicon entries. Could change to a multimap when adding variants. **/
      std::unordered_map< std::string, LexiconEntry > lexiconEntries;

      /** Reads the next syllable from the given stream. **/
      utt::SyllablePtr ReadSyllable(std::istream& stream);
      utt::WordPtr ReadTranscriptionWord(std::stringstream& wordstream);

   public:

      Lexicon();
      ~Lexicon();

      /**
       * Reads lexicon data from supplied stream.
       **/
      void Read(std::istream& in);

      /**
       * Returns the word object for text, or nullptr.
       **/
      utt::WordPtr GetWord(const std::string& text);

      inline utt::WordPtr operator[](const std::string& text) {
         return GetWord(text);
      }

      std::string ReadOrthographyWord(std::istream& lineStream);
};

}

#endif
