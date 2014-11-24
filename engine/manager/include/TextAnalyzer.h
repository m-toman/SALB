/**
 * (c) 2013 Markus Toman, FTW GmbH
 *
 * HTS TTS system
 **/

#ifndef FTW_TTSTEXTANALYZER
#define FTW_TTSTEXTANALYZER

#include <memory>
#include <vector>
#include <string>
#include "Label.h"

namespace htstts {

class Text;
class TextFragment;

class TextAnalyzer;
typedef std::shared_ptr<TextAnalyzer> TextAnalyzerPtr;

/**
 * TextAnalyzer
 **/
class TextAnalyzer {

   protected:

      TextAnalyzer();
   public:
      virtual ~TextAnalyzer();

      static TextAnalyzerPtr NewAnalyzer(const std::string& type);


      /**
       * Analyzes a Text object and returns Labels for it.
       *
       *
       **/
      //virtual LabelsPtr AnalyzeText( const Text& text ) = 0;

      /**
       * AnalyzeTextFragment.
       * Analyzes a text fragment and returns labels for it.
       * TODO: If the text analysis needs context information, something like fragment->owner (Text) should be implemented.
       *
       **/
      virtual LabelsPtr AnalyzeTextFragment(const TextFragmentPtr& fragment) = 0;

};

}

#endif