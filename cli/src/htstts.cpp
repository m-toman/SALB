#include <memory>
#include <string>
#include <cstring>
#include <sstream>
#include <fstream>
#include <cerrno>
#include <iostream>

#include "TTSManager.h"
#include "utils.h"
#include "Lexicon.h"
#include "LTSTree.h"
#include "InternalTextAnalyzer.h"


using namespace htstts;

inline std::string get_file_contents(const char* filename) {
   std::ifstream in(filename, std::ios::in);
   if (in) {
      std::string contents;
      in.seekg(0, std::ios::end);
      contents.resize((unsigned int)in.tellg());
      in.seekg(0, std::ios::beg);
      in.read(&contents[0], contents.size());
      in.close();
      return (contents);
   }
   throw (errno);
}

void print_usage() {
   std::cout << "Usage: htstts [OPTIONS]... \"TEXT\"" << std::endl;
   std::cout << "Description: Synthesizes TEXT or the text in the file given by the -f option." << std::endl;
   std::cout << "Options:" << std::endl;
   std::cout << " -m MODELFILE \t voice model (.htsvoice) to use" << std::endl;
   std::cout << " -r RULESFILE \t text analysis rules file (.rules) to use" << std::endl;
   std::cout << " -f INPUTFILE \t Input text file to synthesize instead of TEXT" << std::endl;
   std::cout << " -l LANGUAGE \t Uses the language provided for text analysis (e.g. \"en-*\", \"de-*\", \"sq-*\")" << std::endl;
   std::cout << " -s SPEAKINGRATE \t Speaking rate to use, < 1.0 is slower, > 1.0 is faster [1.0]" << std::endl;
   std::cout << " -p PITCH \t change pitch [0.0]" << std::endl;
   std::cout << " -v VOLUME \t change volume 0-100 [100]" << std::endl;
   std::cout << " -pl PRUNEDFILE \t Prunes the supplied RULESFILE and outputs the pruned lexicon to PRUNEDFILE" << std::endl;
   std::cout << " -gl OUTLEX \t Generates a lexicon in OUTLEX with all words from the input using the supplied RULESFILE." << std::endl;
}

void prune_rules(const std::string& rulesfile, const std::string& outfile) {
   Lexicon lexicon;
   LTSTree lts;
   int sizeOld = 0;
   int sizeNew = 0;

   std::cout << "Starting rules pruning." << std::endl;

   // read lexicon and LTS
   std::ifstream rulesstream(rulesfile);
   if (rulesstream.is_open()) {
      lexicon.Read(rulesstream);
      lts.Read(rulesstream);
      rulesstream.close();
   }
   //TODO else

   // now go through lex again and prune using LTS
   std::ifstream rulesstream2(rulesfile);
   std::ofstream outstream(outfile);
   std::string line;

   if (rulesstream2.is_open() && outstream.is_open()) {
      rulesstream2 >> line;
      if (line != "MNCL") {
         throw FileLoadException("Lexicon");
      }
      outstream << "MNCL" << std::endl;

      while (std::getline(rulesstream2, line)) {
         if (line.length() == 0 || line[0] == 0 || line[0] == 13 || line[0] == 10) {
            continue;
         }
         if (!line.empty() && line[line.size() - 1] == '\r') {
            line.erase(line.size() - 1);
         }
         if (line == "ENDLEX") {
            break;
         }

         ++sizeOld;

         std::stringstream lineStream(line);
         std::string word = lexicon.ReadOrthographyWord(lineStream);

         // get word from lexicon and from lts
         std::string entrylex = utt::make_lex_entry(lexicon[word], word);
         std::string entrylts = utt::make_lex_entry(lts[word], word);

         // if they dont match, write it to new lex
         if (entrylex != entrylts) {
            outstream << entrylex << std::endl;
            ++sizeNew;
         }
      }

      //TODO: probably attach rest of file?

      outstream.close();
      rulesstream2.close();
   }

   std::cout << "Finished rules pruning." << std::endl;
}

void gen_lex(FragmentPropertiesPtr properties, const std::string& input, const std::string& genlex) {
   std::ofstream outstream(genlex);
   std::stringstream instr(input);
   std::string line;
   int linenum = 1;

   InternalTextAnalyzer ita;

   std::cout << "Starting lexicon generation." << std::endl;

   if (!outstream.is_open()) {
      std::cout << "Could not open " << genlex << std::endl;
   }
   else {
      while (std::getline(instr, line)) {
         if (line.size() == 0 || line[0] == 0) {
            continue;   // deal with CRLF
         }

         TextFragmentPtr tf = std::make_shared<TextFragment>(line, properties);
         utt::PhrasePtr phrase = ita.TextFragmentToPhrase(tf);

         for (auto word : phrase->words) {
            if (!utt::is_sil(word)) {
               outstream << utt::make_lex_entry(word) << std::endl;
            }
         }
      }
      outstream.close();
   }
   std::cout << "Finished lexicon generation." << std::endl;
}

void synthesize_text(FragmentPropertiesPtr properties, const std::string& input) {
   std::stringstream instr(input);
   std::string line;
   int linenum = 1;
   TTSManager tts;

   // loop lines in text
   while (std::getline(instr, line)) {
      if (line.size() == 0 || line[0] == 0) {
         continue;   // deal with CRLF
      }
      std::cout << "Synthesizing: \"" << line << "\"" << std::endl;

      TextFragmentPtr tf = std::make_shared<TextFragment>(line, properties);
      TTSResultPtr result = tts.SynthesizeTextFragment(tf);

      std::stringstream outwav, outlab;
      outwav << "output_" << linenum << ".wav";
      outlab << "output_" << linenum << ".lab";

      std::cout << "Writing files " << outwav.str() << ", " << outlab.str() << std::endl;

      // save wav
      save_result_riff(result, outwav.str());

      // save labels
      std::ofstream labfile;
      labfile.open(outlab.str());
      for (auto lab : result->GetLabels()) {
         labfile << lab->GetAsHTKLabel() << std::endl;
      }
      labfile.close();

      ++linenum;
   }
}

/**
* CLI for HTSTTS
*
* TODO:
*    Input: Text or text file
*    Output: defined by -o OTYPE with OTYPE "wav" "lab" "feat"...
*
*/
int main(int argc, char* argv[]) {
   try {
      if (argc < 3) {
         print_usage();
         return -1;
      }

      // get commandline arguments
      std::string input, model, rules, rate, prune, lang = "de-at", genlex, pitch, volume;

      for (int currarg = 1; currarg < argc; ++currarg) {

         if (!strcmp(argv[currarg], "-h") || !strcmp(argv[currarg], "--help")) {
            print_usage();
            return -1;
         }
         else if (!strcmp(argv[currarg], "-l") || !strcmp(argv[currarg], "--language")) {
            lang = std::string(argv[++currarg]);
            std::cout << "Using language: " << lang << std::endl;
         }
         else if (!strcmp(argv[currarg], "-f") || !strcmp(argv[currarg], "--file")) {
            input = get_file_contents(argv[++currarg]);
            std::cout << "Synthesizing file: " << argv[currarg] << std::endl;
         }
         else if (!strcmp(argv[currarg], "-m") || !strcmp(argv[currarg], "--model")) {
            model = std::string(argv[++currarg]);
            std::cout << "Using voice model file: " << model << std::endl;
         }
         else if (!strcmp(argv[currarg], "-r") || !strcmp(argv[currarg], "--rules")) {
            rules = std::string(argv[++currarg]);
            std::cout << "Using text analysis rules file: " << rules << std::endl;
         }
         else if (!strcmp(argv[currarg], "-s") || !strcmp(argv[currarg], "--speed")) {
            rate = std::string(argv[++currarg]);
            std::cout << "Using speaking rate: " << rate << std::endl;
         }
         else if (!strcmp(argv[currarg], "-pl") || !strcmp(argv[currarg], "--prune")) {
            prune = std::string(argv[++currarg]);
            std::cout << "Using prune out file: " << prune << std::endl;
         }
         else if (!strcmp(argv[currarg], "-gl") || !strcmp(argv[currarg], "--genlex")) {
            genlex = std::string(argv[++currarg]);
            std::cout << "Generating lexicon: " << genlex << std::endl;
         }
         else if (!strcmp(argv[currarg], "-p") || !strcmp(argv[currarg], "--pitch")) {
            pitch = std::string(argv[++currarg]);
            std::cout << "Using pitch: " << pitch << std::endl;
         }
         else if (!strcmp(argv[currarg], "-v") || !strcmp(argv[currarg], "--volume")) {
            volume = std::string(argv[++currarg]);
            std::cout << "Using volume: " << volume << std::endl;
         }
         else if (input.size() == 0) {
            input = std::string(argv[currarg]);
         }
      }

      // set properties
      // prepare properties for synthesis
      FragmentPropertiesPtr properties = std::make_shared<FragmentProperties>();         //- properties for every text fragment
      (*properties)[PROPERTY_KEY_TEXTANALYZER] = PROPERTY_VALUE_AUTOMATIC;               //- use internal text analyzer
      (*properties)[PROPERTY_KEY_TEXTANALYZER_RULES] = rules;                            //- the text analysis rules
      (*properties)[PROPERTY_KEY_LANGUAGE]    = lang;                                    //- voice language (TODO)

      (*properties)[PROPERTY_KEY_SYNTHESIZER] = PROPERTY_VALUE_HTSENGINE;                //- use hts_engine as synthesizer
      (*properties)[PROPERTY_KEY_VOICE_PATH]  = model;                                   //- path to the hts model file
      (*properties)[PROPERTY_KEY_VOICE_NAME]  = "HTS Voice";                             //- voice name
      if (volume.size() > 0) {
         (*properties)[PROPERTY_KEY_VOLUME]      = volume;                                   //- voice volume 0-100
      }
      else {
         (*properties)[PROPERTY_KEY_VOLUME]      = "100";                                   //- voice volume 0-100
      }
      if (rate.size() > 0) {
         (*properties)[PROPERTY_KEY_RATE]        = rate;
      }
      if (pitch.size() > 0) {
         (*properties)[PROPERTY_KEY_PITCH]        = pitch;
      }


      // pruning shall be done?
      if (prune.size() > 0 && rules.size() > 0) {
         prune_rules(rules, prune);
      }

      // generate lexicon from rules and input
      if (genlex.size() > 0 && rules.size() > 0 && input.size() > 0) {
         gen_lex(properties, input, genlex);
      }

      // something to synthesize?
      if (model.size() > 0 && input.size() > 0) {
         synthesize_text(properties, input);
      }
   }
   catch (const PropertyMissingException& e) {
      std::cout << "Property missing: " << e.what() << std::endl;
   }
   catch (const FileLoadException& e) {
      std::cout << "Could not load file: " << e.what() << std::endl;
   }

   return 0;
}
