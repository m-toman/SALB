/**
 * (c) 2013 Markus Toman, FTW GmbH
 *
 * HTS TTS system
 **/

#ifndef FTW_TTSLABEL
#define FTW_TTSLABEL

#include <vector>
#include <memory>

#include "Text.h"

namespace htstts {

/**
 * Represents a single label to be used by synthesizers.
 * A label represents the most elemental unit of synthesis (usually a phone or for example a quinphone)
 * and its properties (prosodic information, context information etc.)
 *
 **/
class Label {
   private:
      TextFragmentPtr textFragment;      ///< link to the text fragment from which this element was generated.

   public:
      Label();
      virtual ~Label();

      /**
       * Return a reference to the label in HTK format.
       **/
      virtual const std::string& GetAsHTKLabel() = 0;

      inline virtual void SetTextFragment(const TextFragmentPtr& textFragment) {
         this->textFragment = textFragment;
      }
};
typedef std::shared_ptr<Label> LabelPtr;

/**
 * HTKOnlyLabel
 * This class holds no internal structure but only a label string in HTK format.
 * Therefore it is only compatible with synthesizers using the HTK label format.
 **/
class HTKOnlyLabel : public Label {
   private:
      std::string htkLabel;
   public:
      HTKOnlyLabel(const std::string& htklab) : htkLabel(htklab)  {   }
      inline virtual const std::string& GetAsHTKLabel() {
         return htkLabel;
      }

      virtual ~HTKOnlyLabel() {}
};


/**
 * Represents a series of labels.
 * These are produced by TextAnalyzers from TextFragments.
 **/
class Labels : public std::vector<LabelPtr> {
   public:
      Labels() {}
      ~Labels() {}

};

typedef std::shared_ptr<Labels> LabelsPtr;

}

#endif