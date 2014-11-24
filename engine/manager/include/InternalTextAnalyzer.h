/**
 * (c) 2013 Markus Toman, FTW GmbH
 *
 * HTS TTS system
 **/

#ifndef FTW_TTSINTERNALTEXTANALYZER
#define FTW_TTSINTERNALTEXTANALYZER

#include <memory>
#include <vector>
#include <string>
#include <iostream>

#include "TextAnalyzer.h"
#include "Utterance.h"
#include "Lexicon.h"
#include "LTSTree.h"
#include "Normalizer.h"

namespace htstts {


/**
 * The internal text analyzer.
 *
 * Created for text analysis of Austrian German.
 **/
class InternalTextAnalyzer : public TextAnalyzer {

      /**
       * Holds lexicon, letter-to-sound-rules, normalizer etc.
       **/
      struct TextRules {
         Lexicon lexicon;
         LTSTree lts;
         Normalizer normalizer;
      };

      std::unordered_map< std::string, TextRules > textRulesMap;

      /**
       * Loads lexicon, LTS rules etc. if necessary.
       **/
      TextRules& GetTextRules(const FragmentPropertiesPtr& props);

      utt::WordPtr HandleSpecialWords(const std::string& text);
      bool HandleSpellWords(const std::string& text, TextRules& rules, utt::PhrasePtr& phrase);
   public:

      /**
       * AnalyzeTextFragment.
       * Analyze a text fragment and return labels for it.
       **/
      LabelsPtr AnalyzeTextFragment(const TextFragmentPtr& fragment);
      utt::PhrasePtr TextFragmentToPhrase(const TextFragmentPtr& fragment);

      virtual ~InternalTextAnalyzer() {}

};

}

#endif