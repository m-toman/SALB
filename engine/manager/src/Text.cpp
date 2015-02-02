/**
 * (c) 2013 Markus Toman, FTW GmbH
 *
 * HTS TTS system
 **/

#include "Text.h"

namespace htstts {

/**
 * Text
 **/
Text::Text() {
}


/**
 * ~Text
 **/
Text::~Text() {
}


/**
 * TextFragment
 **/
TextFragment::TextFragment(const std::string& text, const FragmentPropertiesPtr& properties) {   
   // check for UTF-8 bom(b) 
   if (text.size() >= 3 && text[0] == '\xEF' 
         && text[1] == '\xBB' && text[2] == '\xBF') {
      this->text = text.substr(3);
   }
   else {
      this->text = text;
   }

   this->properties = properties;
}

}