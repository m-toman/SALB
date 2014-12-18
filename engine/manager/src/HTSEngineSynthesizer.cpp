/**
* (c) 2013 Markus Toman, FTW GmbH
*
* HTS TTS system
**/


#include <memory>
#include <iostream>
#include <cstdlib>
#include <locale>
#include <sstream>

#include "common.h"
#include "HTSEngineSynthesizer.h"

#include "HTS_engine.h"

namespace htstts {

//============================= HTS ENGINE =============================

HTSEngineSynthesizer::HTSEngineSynthesizer() {
   HTS_Engine_initialize(&engine);
}

HTSEngineSynthesizer::~HTSEngineSynthesizer() {
   HTS_Engine_clear(&engine);
}

/**
* SynthesizeLabels
**/
TTSResultPtr HTSEngineSynthesizer::SynthesizeLabels(const FragmentPropertiesPtr& properties, const LabelsPtr& labels) {

   TTSResultPtr result(new TTSResult());

   //- no model given?
   if (properties->find(PROPERTY_KEY_VOICE_PATH) == properties->end()) {
      throw PropertyMissingException(PROPERTY_KEY_VOICE_PATH);
   }

   //- model to use changed? load model
   // probably think about some caching mechanism.
   std::string& path = (*properties)[PROPERTY_KEY_VOICE_PATH];
   if (path != lastModel)  {
      HTS_Engine_clear(&engine);

      char* p = const_cast<char*>(path.c_str());
      if (HTS_Engine_load(&engine, &p, 1) != TRUE) {
         throw FileLoadException(path);
      }
      lastModel = path;
   }

   //TODO: what about sampling frequency?

   //- set speaking rate
   if (properties->find(PROPERTY_KEY_RATE) == properties->end()) {
      HTS_Engine_set_speed(&engine, baseRate);
   }
   else {
      std::istringstream i((*properties)[PROPERTY_KEY_RATE]);
      i.imbue(std::locale::classic());
      double rate;
      if (!(i >> rate)) {
         HTS_Engine_set_speed(&engine, baseRate);
      }
      else {
         HTS_Engine_set_speed(&engine, rate * baseRate);
      }
   }

   //- set pitch
   if (properties->find(PROPERTY_KEY_PITCH) != properties->end()) {
      std::istringstream i((*properties)[PROPERTY_KEY_PITCH]);
      i.imbue(std::locale::classic());
      double pitch;
      if (i >> pitch) {
         //TODO: this is not exactly what we want
         //HTS_Engine_set_alpha(&engine, pitch);
         HTS_Engine_add_half_tone(&engine, pitch);
      }
   }

   //- set volume
   if (properties->find(PROPERTY_KEY_VOLUME) == properties->end()) {
      HTS_Engine_set_volume(&engine, (double)(100 - baseVolume) * -0.1);
   }
   else {
      int vol = strtol((*properties)[PROPERTY_KEY_VOLUME].c_str(), NULL, 10);
      vol = (int)((double)vol * ((double)baseVolume / 100.0));
      HTS_Engine_set_volume(&engine, (double)(100 - vol) * -0.1);
   }

   //- synthesize

   if (labels->size() > 0) {
      //- this is a big ugly hack as we need char** instead of std::string
      //- we currently use an array of pointers to the internal c_str of the label data.
      //- this is faster but also unsafer than copying it.
      char** labelData = new char* [ labels->size() ];
      std::vector<LabelPtr>::iterator it = labels->begin();
      for (int i = 0; it != labels->end(); ++it, ++i) {
         LabelPtr lab = *it;
         labelData[i] = const_cast<char*>(lab->GetAsHTKLabel().c_str());
      }

      HTS_Engine_synthesize_from_strings(&engine, labelData, labels->size());

      //- store result data
      int num_samples = HTS_Engine_get_nsamples(&engine);
      result->GetFrames().reserve(num_samples);

      for (int i = 0; i < num_samples; ++i) {
         result->GetFrames().push_back(HTS_Engine_get_generated_speech(&engine, i));
      }
      delete[] labelData;

      //- store labels
      // we could add time alignment information to labels here
      result->GetLabels().insert(result->GetLabels().begin(), labels->begin(), labels->end());
   }

   //- store meta information
   // additional meta information in TTS result can be added here.
   result->SetSamplingRate(HTS_Engine_get_sampling_frequency(&engine));

   return result;
}





}
