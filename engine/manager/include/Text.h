/**
 * (c) 2013 Markus Toman, FTW GmbH
 *
 * HTS TTS system
 **/

#ifndef FTW_TTSTEXT
#define FTW_TTSTEXT


#include <memory>
#include <string>
#include <vector>
#include <map>

#include "common.h"

namespace htstts {


/**
 * Represents a fragment of text with a single set of properties.
 **/
class TextFragment {
   private:
      FragmentPropertiesPtr properties;       ///< properties relevant for this text fragment
      std::string text;                       ///< actual text

   public:
      TextFragment(const std::string& text, const FragmentPropertiesPtr& properties);

      const std::string& GetText() const {
         return text;
      }
      FragmentPropertiesPtr& GetProperties() {
         return properties;
      }

};

typedef std::shared_ptr<TextFragment> TextFragmentPtr;


/**
 * Represents a text to be synthesized with additional meta information.
 **/
class Text : public std::vector<TextFragmentPtr> {

   public:
      Text();
      ~Text();

      //TODO: might override push_back etc. to set references to owner "Text" object
};


}

#endif