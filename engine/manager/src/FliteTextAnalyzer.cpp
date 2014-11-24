/**
 * (c) 2013 Markus Toman, FTW GmbH
 *
 * HTS TTS system
 **/



#include <iostream>

#include "TTSLogger.h"
#include "FliteTextAnalyzer.h"
#include "common.h"
#include "Text.h"
#include "Label.h"


#define FLITE_PLUS_HTS_ENGINE
#include "flite.h"

extern "C" {
   //VOICES
   cst_voice* register_cmu_us_kal(const char* voxdir);
   void unregister_cmu_us_kal(cst_voice* v);

   cst_voice* register_ftw_at_leo(const char* voxdir);
   void unregister_ftw_at_leo(cst_voice* v);
}


namespace htstts {

//======================= FliteTextAnalyzer ==========================

static std::string create_label(cst_item* item);

/**
 * Analyze text and return labels.
 **/
//LabelsPtr FliteTextAnalyzer::AnalyzeText( const Text& text ) {
//TODO: implement this method
//    return LabelsPtr( new Labels() );
//}

/**
 * Analyze a text fragment and return labels for it.
 **/
LabelsPtr FliteTextAnalyzer::AnalyzeTextFragment(const TextFragmentPtr& fragment) {

   LabelsPtr labels(new Labels());
   cst_item* segment = NULL;
   int i;

   //TODO: cache cst_voice object
   //static cst_voice* curr_vox = NULL;

   FragmentProperties& props = *(fragment->GetProperties());
   cst_voice* curr_vox = NULL;

   LOG_DEBUG("[FliteTextAnalyzer::AnalyzeTextFragment] Registering kal");
   curr_vox = register_cmu_us_kal(NULL);

   //- analyze text
   cst_utterance* curr_utt = flite_synth_text(fragment->GetText().c_str(), curr_vox);

   //- call hook
   if (curr_vox->utt_init) {
      curr_vox->utt_init(curr_utt, curr_vox);
   }

   //- loop segments and create labels for each
   for (i = 0, segment = relation_head(utt_relation(curr_utt, "Segment"))
                         ; segment
         ; segment = item_next(segment), i++) {

      Label* tmpLabel = new HTKOnlyLabel(create_label(segment));
      tmpLabel->SetTextFragment(fragment);
      labels->push_back(LabelPtr(tmpLabel));
   }

   delete_utterance(curr_utt);
   return labels;
}


/******************************************************************************
 * create label per phoneme
 *  This is from flite+hts_engine.
 *  Converts a cst_utt into a full context label to be used by hts_engine
 ******************************************************************************/
#if _MSC_VER
#define snprintf sprintf_s
#else
#define strcpy_s strcpy
#endif

static std::string create_label(cst_item* item) {
   const int MAXBUFLEN = 1024;
   char label[MAXBUFLEN];
   char seg_pp[8];
   char seg_p[8];
   char seg_c[8];
   char seg_n[8];
   char seg_nn[8];
   char endtone[8];
   int sub_phrases = 0;
   int lisp_total_phrases = 0;
   int tmp1 = 0;
   int tmp2 = 0;
   int tmp3 = 0;
   int tmp4 = 0;

   /* load segments */
   strcpy_s(seg_pp, ffeature_string(item, "p.p.name"));
   strcpy_s(seg_p, ffeature_string(item, "p.name"));
   strcpy_s(seg_c, ffeature_string(item, "name"));
   strcpy_s(seg_n, ffeature_string(item, "n.name"));
   strcpy_s(seg_nn, ffeature_string(item, "n.n.name"));

   /* load endtone */
   strcpy_s(endtone, ffeature_string(item, "R:SylStructure.parent.parent.R:Phrase.parent.daughtern.R:SylStructure.daughtern.endtone"));

   if (strcmp(seg_c, "pau") == 0) {

      /* for pause */
      if (item_next(item) != NULL) {
         sub_phrases = ffeature_int(item, "n.R:SylStructure.parent.R:Syllable.sub_phrases");
         tmp1 = ffeature_int(item, "n.R:SylStructure.parent.parent.R:Phrase.parent.lisp_total_syls");
         tmp2 = ffeature_int(item, "n.R:SylStructure.parent.parent.R:Phrase.parent.lisp_total_words");
         lisp_total_phrases = ffeature_int(item, "n.R:SylStructure.parent.parent.R:Phrase.parent.lisp_total_phrases");
      }
      else {
         sub_phrases = ffeature_int(item, "p.R:SylStructure.parent.R:Syllable.sub_phrases");
         tmp1 = ffeature_int(item, "p.R:SylStructure.parent.parent.R:Phrase.parent.lisp_total_syls");
         tmp2 = ffeature_int(item, "p.R:SylStructure.parent.parent.R:Phrase.parent.lisp_total_words");
         lisp_total_phrases = ffeature_int(item, "p.R:SylStructure.parent.parent.R:Phrase.parent.lisp_total_phrases");
      }
      snprintf(label, MAXBUFLEN - 1, "%s^%s-%s+%s=%s@x_x/A:%d_%d_%d/B:x-x-x@x-x&x-x#x-x$x-x!x-x;x-x|x/C:%d+%d+%d/D:%s_%d/E:x+x@x+x&x+x#x+x/F:%s_%d/G:%d_%d/H:x=x^%d=%d|%s/I:%d=%d/J:%d+%d-%d"
               , strcmp(seg_pp, "0") == 0 ? "x" : seg_pp, strcmp(seg_p, "0") == 0 ? "x" : seg_p, seg_c, strcmp(seg_n, "0") == 0 ? "x" : seg_n, strcmp(seg_nn, "0") == 0 ? "x" : seg_nn, ffeature_int(item, "p.R:SylStructure.parent.R:Syllable.stress"), ffeature_int(item, "p.R:SylStructure.parent.R:Syllable.accented"), ffeature_int(item, "p.R:SylStructure.parent.R:Syllable.syl_numphones"), ffeature_int(item, "n.R:SylStructure.parent.R:Syllable.stress"), ffeature_int(item, "n.R:SylStructure.parent.R:Syllable.accented"), ffeature_int(item, "n.R:SylStructure.parent.R:Syllable.syl_numphones"), ffeature_string(item, "p.R:SylStructure.parent.parent.R:Word.gpos"), ffeature_int(item, "p.R:SylStructure.parent.parent.R:Word.word_numsyls"), ffeature_string(item, "n.R:SylStructure.parent.parent.R:Word.gpos"), ffeature_int(item, "n.R:SylStructure.parent.parent.R:Word.word_numsyls"), ffeature_int(item, "p.R:SylStructure.parent.parent.R:Phrase.parent.lisp_num_syls_in_phrase"), ffeature_int(item, "p.R:SylStructure.parent.parent.R:Phrase.parent.lisp_num_words_in_phrase"), sub_phrases + 1, lisp_total_phrases - sub_phrases, endtone, ffeature_int(item, "n.R:SylStructure.parent.parent.R:Phrase.parent.lisp_num_syls_in_phrase"), ffeature_int(item, "n.R:SylStructure.parent.parent.R:Phrase.parent.lisp_num_words_in_phrase"), tmp1, tmp2, lisp_total_phrases);
   }
   else {

      /* for no pause */
      tmp1 = ffeature_int(item, "R:SylStructure.pos_in_syl");
      tmp2 = ffeature_int(item, "R:SylStructure.parent.R:Syllable.syl_numphones");
      tmp3 = ffeature_int(item, "R:SylStructure.parent.R:Syllable.pos_in_word");
      tmp4 = ffeature_int(item, "R:SylStructure.parent.parent.R:Word.word_numsyls");
      sub_phrases = ffeature_int(item, "R:SylStructure.parent.R:Syllable.sub_phrases");
      lisp_total_phrases = ffeature_int(item, "R:SylStructure.parent.parent.R:Phrase.parent.lisp_total_phrases");

      snprintf(label, MAXBUFLEN - 1, "%s^%s-%s+%s=%s@%d_%d/A:%d_%d_%d/B:%d-%d-%d@%d-%d&%d-%d#%d-%d$%d-%d!%d-%d;%d-%d|%s/C:%d+%d+%d/D:%s_%d/E:%s+%d@%d+%d&%d+%d#%d+%d/F:%s_%d/G:%d_%d/H:%d=%d^%d=%d|%s/I:%d=%d/J:%d+%d-%d"
               , strcmp(seg_pp, "0") == 0 ? "x" : seg_pp, strcmp(seg_p, "0") == 0 ? "x" : seg_p, seg_c, strcmp(seg_n, "0") == 0 ? "x" : seg_n, strcmp(seg_nn, "0") == 0 ? "x" : seg_nn, tmp1 + 1, tmp2 - tmp1, ffeature_int(item, "R:SylStructure.parent.R:Syllable.p.stress"), ffeature_int(item, "R:SylStructure.parent.R:Syllable.p.accented"), ffeature_int(item, "R:SylStructure.parent.R:Syllable.p.syl_numphones"), ffeature_int(item, "R:SylStructure.parent.R:Syllable.stress"), ffeature_int(item, "R:SylStructure.parent.R:Syllable.accented"), tmp2, tmp3 + 1, tmp4 - tmp3, ffeature_int(item, "R:SylStructure.parent.R:Syllable.syl_in") + 1, ffeature_int(item, "R:SylStructure.parent.R:Syllable.syl_out") + 1, ffeature_int(item, "R:SylStructure.parent.R:Syllable.ssyl_in") + 1, ffeature_int(item, "R:SylStructure.parent.R:Syllable.ssyl_out") + 1, ffeature_int(item, "R:SylStructure.parent.R:Syllable.asyl_in") + 1, ffeature_int(item, "R:SylStructure.parent.R:Syllable.asyl_out") + 1, ffeature_int(item, "R:SylStructure.parent.R:Syllable.lisp_distance_to_p_stress"), ffeature_int(item, "R:SylStructure.parent.R:Syllable.lisp_distance_to_n_stress"), ffeature_int(item, "R:SylStructure.parent.R:Syllable.lisp_distance_to_p_accent"), ffeature_int(item, "R:SylStructure.parent.R:Syllable.lisp_distance_to_n_accent"), ffeature_string(item, "R:SylStructure.parent.R:Syllable.syl_vowel"), ffeature_int(item, "R:SylStructure.parent.R:Syllable.n.stress"), ffeature_int(item, "R:SylStructure.parent.R:Syllable.n.accented"), ffeature_int(item, "R:SylStructure.parent.R:Syllable.n.syl_numphones"), ffeature_string(item, "R:SylStructure.parent.parent.R:Word.p.gpos"), ffeature_int(item, "R:SylStructure.parent.parent.R:Word.p.word_numsyls"), ffeature_string(item, "R:SylStructure.parent.parent.R:Word.gpos"), tmp4, ffeature_int(item, "R:SylStructure.parent.parent.R:Word.pos_in_phrase") + 1, ffeature_int(item, "R:SylStructure.parent.parent.R:Word.words_out"), ffeature_int(item, "R:SylStructure.parent.parent.R:Word.content_words_in") + 1, ffeature_int(item, "R:SylStructure.parent.parent.R:Word.content_words_out") + 1, ffeature_int(item, "R:SylStructure.parent.parent.R:Word.lisp_distance_to_p_content"), ffeature_int(item, "R:SylStructure.parent.parent.R:Word.lisp_distance_to_n_content"), ffeature_string(item, "R:SylStructure.parent.parent.R:Word.n.gpos"), ffeature_int(item, "R:SylStructure.parent.parent.R:Word.n.word_numsyls"), ffeature_int(item, "R:SylStructure.parent.parent.R:Phrase.parent.p.lisp_num_syls_in_phrase"), ffeature_int(item, "R:SylStructure.parent.parent.R:Phrase.parent.p.lisp_num_words_in_phrase"), ffeature_int(item, "R:SylStructure.parent.parent.R:Phrase.parent.lisp_num_syls_in_phrase"), ffeature_int(item, "R:SylStructure.parent.parent.R:Phrase.parent.lisp_num_words_in_phrase"), sub_phrases + 1, lisp_total_phrases - sub_phrases, strcmp(endtone, "0") == 0 ? "NONE" : endtone, ffeature_int(item, "R:SylStructure.parent.parent.R:Phrase.parent.n.lisp_num_syls_in_phrase"), ffeature_int(item, "R:SylStructure.parent.parent.R:Phrase.parent.n.lisp_num_words_in_phrase"), ffeature_int(item, "R:SylStructure.parent.parent.R:Phrase.parent.lisp_total_syls"), ffeature_int(item, "R:SylStructure.parent.parent.R:Phrase.parent.lisp_total_words"), lisp_total_phrases);

   }
   return std::string(label);
}
}
