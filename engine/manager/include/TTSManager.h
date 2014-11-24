/**
* (c) 2013 Markus Toman, FTW GmbH
*
* HTS TTS system
**/

#ifndef FTW_TTSMANAGER
#define FTW_TTSMANAGER

#include <memory>
#include <vector>
#include <map>
#include <mutex>

#include "common.h"
#include "Text.h"
#include "Label.h"
#include "Synthesizer.h"
#include "TextAnalyzer.h"
#include "TTSLogger.h"

namespace htstts {

/**
* TTSManager to synthesize text.
**/
class TTSManager {

   private:
      //std::recursive_mutex objectMutex;

      std::map<std::string, SynthesizerPtr> synthesizers;     ///< Pointers to known synthesizers.
      std::map<std::string, TextAnalyzerPtr> textAnalyzers;   ///< Pointers to known text analyzers.

      int baseVolume;                  ///< base volume that can be further modified by volume set by fragments
      double baseRate;                 ///< base speaking rate that can be further modified by speaking rate set by fragments

   public:
      ~TTSManager();
      TTSManager();


      /**
      * Synthesizes the content of a Text object.
      *
      * The easiest approach: input text, output frames.
      **/
      TTSResultPtr SynthesizeText(Text& text);

      /**
      * Synthesizes the content of a TextFragment object.
      *
      * This is more efficient if all your data is just a single text fragment.
      **/
      TTSResultPtr SynthesizeTextFragment(const TextFragmentPtr& tf);

      //TODO: method to preload voice?

      /**
      * Sets a base volume (range 0-100).
      **/
      void SetBaseVolume(int volume) {
         baseVolume = clamp(volume, 0, 100);
      }

      /**
      * Sets a base speaking rate where 1.0 is original speaking rate, 2.0 is double rate etc.
      */
      void SetBaseSpeakingRate(double rate) {
         baseRate = rate;
      }

      /**
      * SynthesizeLabels.
      * Synthesizes a series of Labels using the given properties.
      **/
      TTSResultPtr SynthesizeLabels(const FragmentPropertiesPtr& properties, const LabelsPtr& labels);
      LabelsPtr AnalyzeTextFragment(const TextFragmentPtr& tf);

   protected:

      /**
      * Retrieves a TextAnalyzer for the given type.
      * If it does not yet exist, it will be created and stored.
      **/
      TextAnalyzerPtr GetTextAnalyzer(const FragmentPropertiesPtr props);

      /**
      * Retrieves a Synthesizer for the given type.
      * If it does not yet exist, it will be created and stored.
      **/
      SynthesizerPtr  GetSynthesizer(const std::string& type);

};

}

#endif
