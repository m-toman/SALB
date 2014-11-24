/**
* (c) 2013 Markus Toman, FTW GmbH
*
* HTS TTS system
**/

#include <iostream>
#include "TTSManager.h"
#include "Synthesizer.h"

namespace htstts {

/**
* TTSManager
**/
TTSManager::TTSManager() {
   baseVolume = 100;
   baseRate = 1.0;
}


/**
* ~TTSManager
**/
TTSManager::~TTSManager() {
}


/**
* Synthesize text and return frames.
**/
TTSResultPtr TTSManager::SynthesizeText(Text& text) {
   //      std::lock_guard<std::mutex> lock(objectMutex);

   TTSResultPtr results = std::make_shared<TTSResult>();

   for (auto it = text.begin() ; it != text.end(); ++it) {
      // synthesize and cat frames
      TTSResultPtr result = SynthesizeTextFragment(*it);
      results->append(*result);
   }
   return results;
}


/**
* Synthesize a text fragment
**/
TTSResultPtr TTSManager::SynthesizeTextFragment(const TextFragmentPtr& tf) {
   //      std::lock_guard<std::mutex> lock(objectMutex);

   // feed text to TextAnalyzer, retrieve Labels
   LOG_DEBUG("[TTSManager::SynthesizeTextFragment] Analyzing text fragment.");
   LabelsPtr labels = this->AnalyzeTextFragment(tf);

   // feed Labels to Synthesizer, retrieve and return frames
   LOG_DEBUG("[TTSManager::SynthesizeTextFragment] Synthesizing labels.");
   return this->SynthesizeLabels(tf->GetProperties(), labels);
}


//=========== INTERNAL METHODS ===============

/**
* Synthesize a series of labels
**/
TTSResultPtr TTSManager::SynthesizeLabels(const FragmentPropertiesPtr& properties, const LabelsPtr& labels) {

   std::string requestedSynthesizer = PROPERTY_VALUE_AUTOMATIC;

   FragmentProperties& props = (*properties);

   //- another synthesizer given?
   if (props.find(PROPERTY_KEY_SYNTHESIZER) != props.end()) {
      requestedSynthesizer = props[ PROPERTY_KEY_SYNTHESIZER ];
   }

   SynthesizerPtr synthesizer = GetSynthesizer(requestedSynthesizer);

   //- set synthesizer params
   synthesizer->SetBaseVolume(this->baseVolume);
   synthesizer->SetBaseSpeakingRate(this->baseRate);

   return synthesizer->SynthesizeLabels(properties, labels);
}

/**
* Text analysis on a text fragment, returns resulting labels.
**/
LabelsPtr TTSManager::AnalyzeTextFragment(const TextFragmentPtr& tf) {

   LOG_DEBUG("[TTSManager::AnalyzeTextFragment] Analyzing Text.");

   TextAnalyzerPtr textAnalyzer = GetTextAnalyzer(tf->GetProperties());

   LOG_DEBUG("[TTSManager::AnalyzeTextFragment] Call Analyzer.");
   return textAnalyzer->AnalyzeTextFragment(tf);
}

/**
* GetTextAnalyzer
**/
TextAnalyzerPtr TTSManager::GetTextAnalyzer(const FragmentPropertiesPtr props) {

   std::string requestedAnalyzer = PROPERTY_VALUE_AUTOMATIC;

   //- another text analyzer given?
   if (props->find(PROPERTY_KEY_TEXTANALYZER) != props->end()) {
      requestedAnalyzer = (*props)[ PROPERTY_KEY_TEXTANALYZER ];
   }
   // none given or automatic?
   if (requestedAnalyzer == PROPERTY_VALUE_AUTOMATIC) {
      // language given?
      if (props->find(PROPERTY_KEY_LANGUAGE) !=  props->end()) {
         // german? use internal
         if ((*props)[ PROPERTY_KEY_LANGUAGE ].substr(0, 2) == "de") {
            requestedAnalyzer = PROPERTY_VALUE_INTERNAL;
         }
         // english? use flite (if available)
         else if ((*props)[ PROPERTY_KEY_LANGUAGE ].substr(0, 2) == "en") {
            requestedAnalyzer = PROPERTY_VALUE_FLITE;
         }
         // albanian? use internal
         else if ((*props)[ PROPERTY_KEY_LANGUAGE ].substr(0, 2) == "sq") {
            requestedAnalyzer = PROPERTY_VALUE_INTERNAL;
         }
      }
   }

   //- analyzer does not yet exist?
   if (textAnalyzers.find(requestedAnalyzer) == textAnalyzers.end()) {
      textAnalyzers[ requestedAnalyzer ] = TextAnalyzer::NewAnalyzer(requestedAnalyzer);
   }
   return textAnalyzers[ requestedAnalyzer ];
}


/**
* GetSynthesizer
**/
SynthesizerPtr TTSManager::GetSynthesizer(const std::string& type) {
   //- synthesizer does not yet exist?
   if (synthesizers.find(type) == synthesizers.end()) {
      synthesizers[ type ] = Synthesizer::NewSynthesizer(type);
   }

   return synthesizers[ type ];
}


}