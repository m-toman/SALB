/**
* (c) 2013 Markus Toman, FTW GmbH
*
* HTS TTS system
**/



#include <iostream>
#include <sstream>

#include "InternalTextAnalyzer.h"
#include "TTSLogger.h"
#include "common.h"
#include "Text.h"
#include "Label.h"
#include "Utterance.h"
#include "utils.h"

namespace htstts {

namespace utt {

//TODO: move label building to another file?
/**
* Build Label Quin Phone Block
**/
inline void BuildLabelQuinPhone(std::stringstream& htklabel, utt::PhraseIterator& phit) {
   std::string p;
   std::string pp;
   std::string n;
   std::string nn;
   utt::PhraseIterator tmpit = phit;

   --tmpit;
   p = tmpit.isValid() ? tmpit.GetCurrentPhone()->symbol : "x";
   --tmpit;
   pp = tmpit.isValid() ? tmpit.GetCurrentPhone()->symbol : "x";

   tmpit = phit;
   ++tmpit;
   n = tmpit.isValid() ? tmpit.GetCurrentPhone()->symbol : "x";
   ++tmpit;
   nn = tmpit.isValid() ? tmpit.GetCurrentPhone()->symbol : "x";

   htklabel << pp << "^" << p << "-" << phit.GetCurrentPhone()->symbol << "+" << n << "=" << nn << "@";

   if (utt::is_sil(phit.GetCurrentPhone())) {
      htklabel << "x_x";
   }
   else {
      int pix = phit.GetPhoneIndexInSyllable();
      htklabel << (pix + 1) << "_" << (phit.GetCurrentSyllable()->phones.size() - pix);
   }
}

/**
* Build Label Block A, regarding previous syllable.
**/
inline void BuildLabelBlockA(std::stringstream& htklabel, utt::PhraseIterator& phit) {
   utt::PhraseIterator tmpit = phit;
   tmpit.PrevSyllable();

   if (tmpit.isValid()) {
      htklabel << "/A:";
      htklabel << (tmpit.GetCurrentSyllable()->stressed ? "1" : "0");
      htklabel << "_0_";
      htklabel << tmpit.GetCurrentSyllable()->phones.size();
   }
   else {
      htklabel << "/A:0_0_0";
   }
}

/**
* Build Label Block B, regarding current syllable.
* (/B:x-x-x@x-x&x-x#x-x$x-x!x-x;x-x|x)
**/
inline void BuildLabelBlockB(std::stringstream& htklabel, utt::PhraseIterator& phit) {
   if (utt::is_sil(phit.GetCurrentPhone())) {
      htklabel << "/B:x-x-x@x-x&x-x#x-x$x-x!x-x;x-x|x";
   }
   else {
      htklabel << "/B:";
      htklabel << (phit.GetCurrentSyllable()->stressed ? "1" : "0");
      htklabel << "-";
      htklabel << (phit.GetCurrentSyllable()->stressed ? "1" : "0");  //TODO: this should be actually accented
      htklabel << "-";
      htklabel << phit.GetCurrentSyllable()->phones.size(); // number of phones in current syllable
      htklabel << "@";
      int pix = phit.GetSyllableIndexInWord();   // syllable position in word forward and backword
      htklabel << (pix + 1) << "+" << (phit.GetCurrentWord()->syllables.size() - pix);
      htklabel << "&";
      pix = phit.GetSyllableIndexInPhrase();
      htklabel << (pix + 1) << "-" << (phit.GetSyllablesInPhrase() - pix);
      htklabel << "#";
      //TODO a lot of stuff missing
      //htklabel << "x-x$x-x!x-x;x-x";
      htklabel << "|0";   //TODO: name of vowel of current syllable
   }


}

/**
* Build Label Block C, regarding next syllable.
* (/C:1+0+2)
**/
inline void BuildLabelBlockC(std::stringstream& htklabel, utt::PhraseIterator& phit) {
   utt::PhraseIterator tmpit = phit;
   tmpit.NextSyllable();

   if (tmpit.isValid()) {
      htklabel << "/C:";
      htklabel << (tmpit.GetCurrentSyllable()->stressed ? "1" : "0");
      htklabel << "+0+";
      htklabel << tmpit.GetCurrentSyllable()->phones.size();
   }
   else {
      htklabel << "/C:0+0+0";
   }
}

/**
* Build Label Block D
* (/D:content_2)
**/
inline void BuildLabelBlockD(std::stringstream& htklabel, utt::PhraseIterator& phit) {
   utt::PhraseIterator tmpit = phit;
   tmpit.PrevWord();

   if (tmpit.isValid()) {
      htklabel << "/D:";
      htklabel << "content";  //TODO: POS of previous word
      htklabel << "_";
      htklabel << tmpit.GetCurrentWord()->syllables.size();
   }
   else {
      htklabel << "/D:0_0";
   }
}

/**
* Build Label Block E
* (E:content+2@1+12&1+9#0+1)
**/
inline void BuildLabelBlockE(std::stringstream& htklabel, utt::PhraseIterator& phit) {

   if (utt::is_sil(phit.GetCurrentPhone())) {
      htklabel << "/E:x+x@x+x&x+x#x+x";
   }
   else {
      htklabel << "/E:";
      htklabel << "content";  //TODO: POS of previous word
      htklabel << "+";
      htklabel << phit.GetCurrentWord()->syllables.size();
      htklabel << "@";
      int pix = phit.GetWordIndex();
      htklabel << (pix + 1) << "+" << (phit.GetPhrase()->words.size() - pix);
      htklabel << "&";

      //TODO a lot of stuff missing
   }
}

/**
* Build Label Block F
* (/D:content_2)
**/
inline void BuildLabelBlockF(std::stringstream& htklabel, utt::PhraseIterator& phit) {
   utt::PhraseIterator tmpit = phit;
   tmpit.NextWord();

   if (tmpit.isValid()) {
      htklabel << "/F:";
      htklabel << "content";  //TODO: POS of previous word
      htklabel << "_";
      htklabel << tmpit.GetCurrentWord()->syllables.size();
   }
   else {
      htklabel << "/F:0_0";
   }
}

/**
* Build Label Block G
* (/G:0_0)
**/
inline void BuildLabelBlockG(std::stringstream& htklabel, utt::PhraseIterator& phit) {
   htklabel << "/G:0_0";
}

/**
* Build Label Block H
* (/H:6=5@1=1|L-L%)
**/
inline void BuildLabelBlockH(std::stringstream& htklabel, utt::PhraseIterator& phit) {
   // tobi endtone 0 for silence, otherwise currently hardcoded to L-L%
   if (utt::is_sil(phit.GetCurrentPhone())) {
      htklabel << "/H:x=x@1=1|0";
   }
   else {
      htklabel << "/H:";
      htklabel << phit.GetSyllablesInPhrase() << "=" << phit.GetPhrase()->words.size();
      htklabel << "@1=1|L-L%";
   }
}

/**
* Build Label Block I
* (/I:0=0)
**/
inline void BuildLabelBlockI(std::stringstream& htklabel, utt::PhraseIterator& phit) {
   htklabel << "/I:0=0";
}

/**
* Build Label Block J
* (/J:6+5-1)
**/
inline void BuildLabelBlockJ(std::stringstream& htklabel, utt::PhraseIterator& phit) {
   htklabel << "/J:";
   htklabel << phit.GetSyllablesInPhrase() << "+" << phit.GetPhrase()->words.size();
   htklabel << "-1";
}

/**
*
* Builds labels from phrase and adds them to labels.
* We build HTKOnlyLabels.
* If at any point we would need other labels too, we'd just move this code
* into a subclass of Label and store the Phrase/Utterance there until needed in HTK format.
*/
inline void BuildLabels(utt::PhrasePtr phrase, const LabelsPtr& labels, const TextFragmentPtr& fragment) {

   for (utt::PhraseIterator phit(phrase); phit.isValid(); ++phit) {

      // create a label per phone
      std::stringstream htklabel;

      BuildLabelQuinPhone(htklabel, phit);
      BuildLabelBlockA(htklabel, phit);
      BuildLabelBlockB(htklabel, phit);
      BuildLabelBlockC(htklabel, phit);
      BuildLabelBlockD(htklabel, phit);
      BuildLabelBlockE(htklabel, phit);
      BuildLabelBlockF(htklabel, phit);
      BuildLabelBlockG(htklabel, phit);
      BuildLabelBlockH(htklabel, phit);
      BuildLabelBlockI(htklabel, phit);
      BuildLabelBlockJ(htklabel, phit);

      //std::cout << htklabel.str() << std::endl;

      LabelPtr tmpLabel = std::make_shared<HTKOnlyLabel>(htklabel.str());
      tmpLabel->SetTextFragment(fragment);
      labels->push_back(tmpLabel);

   }
}
}


utt::PhrasePtr InternalTextAnalyzer::TextFragmentToPhrase(const TextFragmentPtr& fragment) {

   TextRules& rules = GetTextRules(fragment->GetProperties());

   std::string language = "de-at";
   if (fragment->GetProperties()->find(PROPERTY_KEY_LANGUAGE) != fragment->GetProperties()->end()) {
      language = (*fragment->GetProperties())[ PROPERTY_KEY_LANGUAGE ];
   }

   // normalize text and create phrase object
   std::string input = rules.normalizer.Normalize(fragment->GetText(), language);
   utt::PhrasePtr phrase(new utt::Phrase());

   LOG_DEBUG("[InternalTextanalyzer] After normalization: " << input);

   // now loop single words in input
   std::stringstream inputStream(input);
   std::string tmpword;

   //  TODO: if fragment forbids silence
   phrase->words.push_back(utt::make_sil_word());

   while (inputStream >> tmpword) {
      // is word a special word?
      utt::WordPtr word = HandleSpecialWords(tmpword);

      // if not, see if word is in lexicon then?
      if (!word) {
         word = rules.lexicon[tmpword];
      }
      // not in lexicon?
      // see if this word should be spelled
      if (!word) {
         if (HandleSpellWords(tmpword, rules, phrase)) {
            continue;
         }
      }

      // nope? get it from LTS tree then
      if (!word) {
         word = rules.lts[tmpword];
      }
      if (!word) {
         //TODO: notify caller of a problem with a word
         continue;
      }

      word->orthography = tmpword;
      phrase->words.push_back(word);
   }

   phrase->words.push_back(utt::make_sil_word());

   return phrase;
}

/*
 * AnalyzeTextFragment
 */
LabelsPtr InternalTextAnalyzer::AnalyzeTextFragment(const TextFragmentPtr& fragment) {
   LabelsPtr labels(new Labels());
   utt::BuildLabels(TextFragmentToPhrase(fragment), labels, fragment);
   return labels;
}


/*
* GetTextRules
*/
InternalTextAnalyzer::TextRules& InternalTextAnalyzer::GetTextRules(const FragmentPropertiesPtr& props) {
   // no text rules given, throw exception
   if (props->find(PROPERTY_KEY_TEXTANALYZER_RULES) == props->end()) {
      throw PropertyMissingException(PROPERTY_KEY_TEXTANALYZER_RULES);
   }
   else {
      // see if we already know these rules or if we have to load them
      const std::string& textRulesFile = (*props)[ PROPERTY_KEY_TEXTANALYZER_RULES ];
      if (textRulesMap.find(textRulesFile) == textRulesMap.end()) {
         LOG_DEBUG("Loading text analysis rules " << textRulesFile);
         // load textrules
         TextRules textRules;
         std::ifstream rulesstream(textRulesFile);
         if (rulesstream.is_open()) {
            textRules.lexicon.Read(rulesstream);
            textRules.lts.Read(rulesstream);
            rulesstream.close();

            textRulesMap[ textRulesFile ] = textRules;
         }
         else {
            throw FileLoadException(textRulesFile);
         }
      }

      return textRulesMap[ textRulesFile ];
   }
}


utt::WordPtr InternalTextAnalyzer::HandleSpecialWords(const std::string& text) {
   // symbols that create a pause
   if (text == ",") {
      return utt::make_pau_word();
   }
   else if (text == ".") {
      return utt::make_sil_word();
   }
   else if (text == "?") {
      //TODO: mark sentence as question
   }

   return nullptr;
}


bool InternalTextAnalyzer::HandleSpellWords(const std::string& text, TextRules& rules, utt::PhrasePtr& phrase) {
   // check if this word should be spelled
   // no vowels?
   if (text.find_first_of("aAeEiIoOuUyY") == std::string::npos
         && utf8_find_first_umlaut(text) == std::string::npos) {

      for (std::string::size_type i = 0; i < text.length();  i += UTF8_CHAR_LEN(text[i])) {
         std::string spellunit = text.substr(i, UTF8_CHAR_LEN(text[i]));

         // see if word is in lexicon
         utt::WordPtr word = rules.lexicon[spellunit];

         // nope? get it from LTS tree then
         if (!word) {
            word = rules.lts[spellunit];
         }
         if (!word) {
            //TODO: notify caller of a problem with a word
            continue;
         }

         word->orthography = spellunit;
         phrase->words.push_back(word);
      }
      return true;
   }

   return false;
}

}
