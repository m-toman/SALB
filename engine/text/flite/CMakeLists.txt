
add_definitions( -DFLITE_PLUS_HTS_ENGINE )

include_directories (
	"${CMAKE_SOURCE_DIR}/engine/text/flite/include"
	"${CMAKE_SOURCE_DIR}/engine/text/flite/lang/cmulex"
	"${CMAKE_SOURCE_DIR}/engine/text/flite/lang/usenglish"
	)

add_library (flite
   lang/cmulex/cmu_lex.c
   lang/cmulex/cmu_lex_data.c
   lang/cmulex/cmu_lex_entries.c
   lang/cmulex/cmu_lts_model.c
   lang/cmulex/cmu_lts_rules.c
   lang/cmulex/cmu_postlex.c
   lang/cmu_us_kal/cmu_us_kal.c
   lang/usenglish/usenglish.c
   lang/usenglish/us_aswd.c
   lang/usenglish/us_expand.c
   lang/usenglish/us_ffeatures.c
   lang/usenglish/us_gpos.c
   lang/usenglish/us_int_accent_cart.c
   lang/usenglish/us_int_tone_cart.c
   lang/usenglish/us_nums_cart.c
   lang/usenglish/us_phoneset.c
   lang/usenglish/us_phrasing_cart.c
   lang/usenglish/us_pos_cart.c
   lang/usenglish/us_text.c
   src/hrg/cst_ffeature.c
   src/hrg/cst_item.c
   src/hrg/cst_relation.c
   src/hrg/cst_utterance.c
   src/lexicon/cst_lexicon.c
   src/lexicon/cst_lts.c
   src/regex/cst_regex.c
   src/regex/regexp.c
   src/stats/cst_cart.c
   src/synth/cst_ffeatures.c
   src/synth/cst_phoneset.c
   src/synth/cst_synth.c
   src/synth/cst_utt_utils.c
   src/synth/cst_voice.c
   src/synth/flite.c
   src/utils/cst_alloc.c
   src/utils/cst_error.c
   src/utils/cst_features.c
   src/utils/cst_string.c
   src/utils/cst_tokenstream.c
   src/utils/cst_val.c
   src/utils/cst_val_const.c
   src/utils/cst_val_user.c
   )
