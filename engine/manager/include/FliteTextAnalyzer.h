/**
 * (c) 2013 Markus Toman, FTW GmbH
 *
 * HTS TTS system
 **/

#ifndef FTW_TTSFLITETEXTANALYZER
#define FTW_TTSFLITETEXTANALYZER

#include <memory>
#include <vector>
#include <string>
#include "TextAnalyzer.h"


namespace htstts {


/**
 * Our default Flite textanalyzer
 **/
class FliteTextAnalyzer : public TextAnalyzer {



   public:

      /**
       * AnalyzeText.
       *
       *
       **/
      //virtual LabelsPtr AnalyzeText( const Text& text );

      /**
       * AnalyzeTextFragment.
       *
       *
       **/
      virtual LabelsPtr AnalyzeTextFragment(const TextFragmentPtr& fragment);
};

}

#endif