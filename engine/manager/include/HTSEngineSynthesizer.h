/**
 * (c) 2013 Markus Toman, FTW GmbH
 *
 * HTS TTS system
 **/

#ifndef FTW_TTSHTSENGINESYNTHESIZER
#define FTW_TTSHTSENGINESYNTHESIZER

#include <memory>
#include <vector>

#include "Synthesizer.h"
#include "common.h"
#include "Label.h"
#include "Text.h"


//forward declaration fails here
//struct _HTS_Engine;
//typedef struct _HTS_Engine HTS_Engine;
#include "HTS_engine.h"


namespace htstts {

/**
 * Our default synthesizer: the hts_engine synthesizer.
 *
 **/
class HTSEngineSynthesizer : public Synthesizer {
      std::string lastModel;
      HTS_Engine engine;          ///< HTS_engine to be used for synthesis.
   public:
      HTSEngineSynthesizer();
      ~HTSEngineSynthesizer();
      virtual TTSResultPtr SynthesizeLabels(const FragmentPropertiesPtr& properties, const LabelsPtr& labels);
};
}

#endif