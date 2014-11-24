/**
* (c) 2013 Markus Toman, FTW GmbH
*
* HTS TTS system
**/

#include <string>
#include <iostream>
#include <sstream>

#include "utils.h"
#include "Lexicon.h"

namespace htstts {

Lexicon::Lexicon() {
}

Lexicon::~Lexicon() {
}


std::string Lexicon::ReadOrthographyWord(std::istream& lineStream) {
   std::string word;
   lineStream >> word;
   //word = std::regex_replace( word, std::regex( "[^a-zA-Z0-9]+" ), "" );
   word = word.substr(2, word.length() - 3);
   utf8_tolower(word);
   return word;
}


/*
* Read
*/
void Lexicon::Read(std::istream& in) {
   static std::unordered_map< std::string, std::string > textmap;
   std::string tmp;
   std::string line;
   in >> tmp;

   if (tmp != "MNCL") {
      throw FileLoadException("Lexicon");
   }

   // one lexicon entry is currently exactly one line
   while (getline(in, line)) {
      if (line.length() == 0 || line[0] == 0 || line[0] == 13 || line[0] == 10) {
         continue;
      }
      if (!line.empty() && line[line.size() - 1] == '\r') {
         line.erase(line.size() - 1);
      }
      if (line == "ENDLEX") {
         break;
      }

      std::stringstream lineStream(line);
      std::string word, type;

      // read 2 times up to white space to get word and wordtype, drop '(' and ')'

      word = ReadOrthographyWord(lineStream);
      lineStream >> type;

      // lexicon entry already got a word object?
      // we drop this variant for now
      // TODO: handle multiple lexicon variants of a word
      if (lexiconEntries.find(word) != lexiconEntries.end()) {
         continue;
      }

      LexiconEntry& lexEntry = lexiconEntries[word];

      // lazy evaluation on demand:
      getline(lineStream, lexEntry.content);
      // alternative, parse now:
      //lexEntry.word = ReadWord( lineStream );

   }
}


/*
 * ReadWord
 */
utt::WordPtr Lexicon::ReadTranscriptionWord(std::stringstream& wordstream) {
   utt::WordPtr newWord = std::make_shared<utt::Word>();

   // read syllables
   while (wordstream)  {
      utt::SyllablePtr syl = ReadSyllable(wordstream);
      if (syl) {
         newWord->syllables.push_back(syl);
      }
   }
   return newWord->syllables.size() > 0 ? newWord : nullptr;
}


/*
* ReadSyllable
*/
utt::SyllablePtr Lexicon::ReadSyllable(std::istream& stream) {
   std::string syllable;
   int stress;

   // read to ')' and add syllables to word
   if (getline(stream, syllable, ')')
         && syllable.length() > 0
         && stream >> stress)  {

      // remove special characters
      //syllable = std::regex_replace( syllable, std::regex( "[^a-zA-Z0-9 ]+" ), "" );

      utt::SyllablePtr syl(new utt::Syllable());
      syl->stressed = stress > 0;

      std::string phonestr;
      std::stringstream syllableStream(syllable);

      // loop phones in syllable
      while (syllableStream >> phonestr) {
         if (phonestr[0] == '(') {
            continue;
         }
         // perhaps in future use a fixed phone set with
         // additional phone information
         utt::PhonePtr phone = std::make_shared<utt::Phone>();
         phone->symbol = phonestr;
         syl->phones.push_back(phone);
      }

      if (syl->phones.size() == 0) {
         return nullptr;
      }
      return syl;
   }
   return nullptr;
}

/*
* GetWord
*/
utt::WordPtr Lexicon::GetWord(const std::string& text) {
   std::string tmp = text;
   utf8_tolower(tmp);
   // no entry exists
   if (lexiconEntries.find(tmp) == lexiconEntries.end()) {
      return nullptr;
   }
   else {
      LexiconEntry& le = lexiconEntries[ tmp ];
      // word was maybe loaded lazy?
      if (le.word == nullptr) {
         // then parse it now
         if (le.content.size() > 0) {
            std::stringstream ss(le.content);
            le.word = ReadTranscriptionWord(ss);
            le.content.resize(0);
         }
         // no, it just isnt there
         // TODO: remove this entry from lexicon as it is faulty
         else {
            return nullptr;
         }
      }
      return le.word;
   }
}

}