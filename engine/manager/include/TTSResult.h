/**
* (c) 2013 Markus Toman, FTW GmbH
*
* HTS TTS system
**/

#ifndef FTW_TTSRESULT
#define FTW_TTSRESULT

#include <memory>
#include <vector>
#include <map>

#include "common.h"
#include "Label.h"

namespace htstts {

/**
* Structure holding results of a synthesis call.
**/
class TTSResult {
      Labels labels;
      std::vector<double> frames;
      int samplingRate;

   public:
      TTSResult() {
         samplingRate = DEFAULT_SAMPLING_RATE;
      }

      Labels& GetLabels() {
         return labels;
      }
      std::vector<double>& GetFrames() {
         return frames;
      }
      inline int GetSamplingRate() const {
         return samplingRate;
      }
      inline void SetSamplingRate(int r) {
         samplingRate = r;
      }

      /**
      * Append another TTSResult to this one
      **/
      inline void append(const TTSResult& other) {
         frames.insert(frames.end(), other.frames.begin(), other.frames.end());
         labels.insert(labels.end(), other.labels.begin(), other.labels.end());

         //TODO: exception if sampling rates differ? or do resampling?
         samplingRate = other.samplingRate;
      }

      //TODO: positions of phone borders and other meta information
};
typedef std::shared_ptr<TTSResult> TTSResultPtr;

}

#endif