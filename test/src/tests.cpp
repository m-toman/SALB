#include <memory>
#include <string>
#include <list>
#include <iostream>
#include <fstream>
#include <sstream>

#include "TTSManager.h"
#include "Lexicon.h"
#include "LTSTree.h"
#include "Normalizer.h"
#include "InternalTextAnalyzer.h"

using namespace htstts;
using namespace std;

/**
* Test base class.
**/
class Test {
   public:
      virtual bool Run() = 0;
      virtual std::string GetName() = 0;
};


/**
* Lexicon test.
**/
class LexiconTest : public Test {
      bool test1() {
         Lexicon lex;

         //while ( getline (myfile,line) )

         ifstream myfile("testdata/mini.lex");

         if (myfile.is_open()) {
            lex.Read(myfile);
            myfile.close();
         }
         else {
            cout << "Unable to open file ../testdata/mini.lex" << std::endl;
            return false;
         }

         if (!lex[ "\xc3\xa4rzte" ]) {
            return false;
         }
         if (!lex[ "sch\xc3\xb6nes" ]) {
            return false;
         }
         if (!lex[ "affe" ]) {
            return false;
         }
         if (!lex[ "boden" ]) {
            return false;
         }
         if (!lex[ "spannende" ]) {
            return false;
         }
         if (!lex[ "wetter" ]) {
            return false;
         }
         if (!lex[ "eine" ]) {
            return false;
         }

         utt::WordPtr word = lex[ "eine" ];

         int i = 0;
         for (utt::SyllablePtr syl : word->syllables) {

            if (i == 0 && syl->stressed != 1) {
               return false;
            }
            else if (i == 1 && syl->stressed != 0) {
               return false;
            }

            int j = 0;
            for (utt::PhonePtr phone : syl->phones) {
               if (i == 0 && j == 0 && phone->symbol != "GS") {
                  return false;
               }
               if (i == 0 && j == 1 && phone->symbol != "aI") {
                  return false;
               }
               if (i == 1 && j == 0 && phone->symbol != "n") {
                  return false;
               }
               if (i == 1 && j == 1 && phone->symbol != "schwa") {
                  return false;
               }
               ++j;
            }
            ++i;
         }

         return true;
      }

   public:
      virtual bool Run() {
         return test1();

      }

      inline std::string GetName() {
         return "LexiconTest";
      }
};


/**
* LTS test.
**/
class LTSTest : public Test {
      bool test1() {
         LTSTree lts;
         ifstream myfile("testdata/lts_rules.scm");

         if (myfile.is_open()) {
            lts.Read(myfile);
            myfile.close();
         }
         else {
            cout << "Unable to open file ../testdata/lts_rules.scm" << std::endl;
            return false;
         }

         if (!lts[ "k\xc3\xa4se" ]) {
            return false;
         }
         if (!lts[ "\xc3\xa4pfel" ]) {
            return false;
         }
         if (!lts[ "affe" ]) {
            return false;
         }
         if (!lts[ "boden" ]) {
            return false;
         }
         if (!lts[ "spannende" ]) {
            return false;
         }
         if (!lts[ "wetter" ]) {
            return false;
         }
         if (!lts[ "eine" ]) {
            return false;
         }

         utt::WordPtr word = lts[ "eine" ];

         int i = 0;
         for (utt::SyllablePtr syl : word->syllables) {

            int j = 0;
            for (utt::PhonePtr phone : syl->phones) {
               if (j == 0 && phone->symbol != "aI") {
                  return false;
               }
               if (j == 1 && phone->symbol != "n") {
                  return false;
               }
               if (j == 2 && phone->symbol != "schwa") {
                  return false;
               }
               ++j;
            }
            ++i;
         }
         return true;
      }

   public:
      virtual bool Run() {
         return test1();

      }

      inline std::string GetName() {
         return "LTSTest";
      }
};




/**
* Normalizer test.
**/
class NormalizerTest : public Test {
      bool test1() {
         Normalizer norm;

         //TODO: actually check results

         for (int i = 0; i <= 40; ++i) {
            std::stringstream s;
            s << i;
            std::cout << norm.Normalize(s.str(), "de-at") << std::endl;
         }

         std::cout << norm.Normalize("41", "de-at") << std::endl;
         std::cout << norm.Normalize("45", "de-at") << std::endl;
         std::cout << norm.Normalize("101", "de-at") << std::endl;
         std::cout << norm.Normalize("111", "de-at") << std::endl;
         std::cout << norm.Normalize("110", "de-at") << std::endl;
         std::cout << norm.Normalize("105", "de-at") << std::endl;
         std::cout << norm.Normalize("145", "de-at") << std::endl;
         std::cout << norm.Normalize("145,21", "de-at") << std::endl;
         std::cout << norm.Normalize("-100,2", "de-at") << std::endl;
         std::cout << norm.Normalize("-321340,2", "de-at") << std::endl;
         std::cout << norm.Normalize("999999", "de-at") << std::endl;
         std::cout << norm.Normalize("1000000", "de-at") << std::endl;
         std::cout << norm.Normalize("1999999", "de-at") << std::endl;
         std::cout << norm.Normalize("1,2", "de-at") << std::endl;
         std::cout << norm.Normalize("1,22", "de-at") << std::endl;
         std::cout << norm.Normalize("1,222", "de-at") << std::endl;


         return true;
      }

   public:
      virtual bool Run() {
         return test1();

      }

      inline std::string GetName() {
         return "NormalizerTest";
      }
};


/**
* InternalTextAnalyzer test.
**/
class InternalTextAnalyzerTest : public Test {

      FragmentPropertiesPtr properties;
      InternalTextAnalyzer ita;

      bool testLabelGen1() {

         TextFragmentPtr tf = std::make_shared<TextFragment>("eine \xc3\xa4rzte spannende affe wetter", properties);
         LabelsPtr labels = ita.AnalyzeTextFragment(tf);

         //TODO
         return true;
      }


   public:

      virtual bool Run() {
         properties = std::make_shared<FragmentProperties>();                            //- properties for every text fragment
         (*properties)[PROPERTY_KEY_TEXTANALYZER] = PROPERTY_VALUE_INTERNAL;             //- use internal text analyzer
         (*properties)[PROPERTY_KEY_TEXTANALYZER_RULES] = "testdata/mini.lex";           //- the text analysis rules
         (*properties)[PROPERTY_KEY_LANGUAGE]    = "de-at";                              //- voice language

         (*properties)[PROPERTY_KEY_SYNTHESIZER] =  PROPERTY_VALUE_HTSENGINE;            //- use hts_engine as synthesizer
         (*properties)[PROPERTY_KEY_VOICE_PATH]  = "testdata/leo.htsvoice";              //- path to the hts model file
         (*properties)[PROPERTY_KEY_VOICE_NAME]  = "Leopold";                            //- voice name
         (*properties)[PROPERTY_KEY_VOLUME]      = "100";                                //- voice volume 0-100

         return testLabelGen1();
      }

      inline std::string GetName() {
         return "InternalTextAnalyzerTest";
      }
};



/**
* Tests for HTSTTS
*/
int main(int argc, char* argv[]) {

   std::list<Test*> tests;
   tests.push_back(new LexiconTest());
   tests.push_back(new LTSTest());
   tests.push_back(new NormalizerTest());
   tests.push_back(new InternalTextAnalyzerTest());
   std::string tmp;


   for (std::list<Test*>::const_iterator it = tests.begin();
         it != tests.end();
         ++it) {

      if ((*it)->Run()) {
         std::cout << "TEST OK:  " << (*it)->GetName() << std::endl;
      }
      else {
         std::cout << "TEST FAIL: " << (*it)->GetName() << std::endl;
      }
   }

   std::cin >> tmp;

   return 0;
}