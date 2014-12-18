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
   this->text = text;
   this->properties = properties;
}

}