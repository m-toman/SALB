/**
 * (c) 2013 Markus Toman, FTW GmbH
 *
 * HTS TTS system
 **/

#ifndef FTW_TTSSYNTHESIZER
#define FTW_TTSSYNTHESIZER

#include <memory>
#include <vector>

#include "common.h"
#include "Label.h"
#include "Text.h"
#include "TTSResult.h"

namespace htstts {


class Synthesizer;
typedef std::shared_ptr<Synthesizer> SynthesizerPtr;

/**
 * Base class for all Synthesizers.
 * Synthesizers use Label objects to generate TTSResult objects.
 *
 * TODO: we need a contract which properties (like model path) a synthesizer absolutely needs
 *       and which one are optional.
 **/
class Synthesizer {

   protected:
      Synthesizer();

      int baseVolume;
      double baseRate;

   public:
      virtual ~Synthesizer();

      static SynthesizerPtr NewSynthesizer(const std::string& type);

      /**
       * SynthesizeLabel.
       * Synthesizes a series of Labels.
       *
       **/
      virtual TTSResultPtr SynthesizeLabels(const FragmentPropertiesPtr& properties, const LabelsPtr& labels) = 0;

      /**
       * Sets a base volume from 0-100.
       **/
      void SetBaseVolume(int volume) {
         baseVolume = volume;
      }

      /**
       * Sets a base speaking rate where 1.0 is original speaking rate, 2.0 is double rate etc.
       */
      void SetBaseSpeakingRate(double rate) {
         baseRate = rate;
      }

};

}

#endif