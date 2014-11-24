/**
 * (c) 2013 Markus Toman, FTW GmbH
 *
 * HTS TTS system
 **/


#include <memory>

#include "Synthesizer.h"
#include "Label.h"

//- Synthesizers
#include "HTSEngineSynthesizer.h"


namespace htstts {

SynthesizerPtr Synthesizer::NewSynthesizer(const std::string& type) {
   //TODO: currently we always return an hts_engine synthesizer
   return SynthesizerPtr(new HTSEngineSynthesizer());
}

/**
 * TTSManager
 **/
Synthesizer::Synthesizer() {
   baseVolume = 100;
   baseRate = 1.0;
}


/**
 * ~TTSManager
 **/
Synthesizer::~Synthesizer() {
}






}