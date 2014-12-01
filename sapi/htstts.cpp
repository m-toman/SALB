<// htstts.cpp : Implementation of HTSTTS

#include "stdafx.h"
#include "htstts.h"


#include "TTSManager.h"
#include "TTSLogger.h"


using namespace htstts;

static double ConvertSapiRate(int r);

static TTSManager ttsManager;

/******************************************************************************
* HTSTTS Constructor
******************************************************************************/
HTSTTS::HTSTTS() : voiceProperties(new FragmentProperties()) {
   ResetActions();
}


/******************************************************************************
* HTSTTS Destructor
******************************************************************************/
HTSTTS::~HTSTTS() {

}


/******************************************************************************
* Reset action states
******************************************************************************/
void HTSTTS::ResetActions() {
   actionAborted = false;
   actionSkipSentences = 0;
}


/******************************************************************************
* SetObjectToken
* Handle object token which contains info about the voice to be used.
******************************************************************************/
STDMETHODIMP HTSTTS::SetObjectToken(ISpObjectToken* pToken) {
   USES_CONVERSION;
   HRESULT hr;
   CSpDynamicString voicedir, logpath, langID, textrules;


   if (!SUCCEEDED(hr = SpGenericSetObjectToken(pToken, voiceToken))) {
      return hr;
   }

   //- get logfile directory
   if (SUCCEEDED(voiceToken->GetStringValue(L"Logfile", &logpath))) {
      LOG_SETFILE(W2A(logpath));
   }

   LOG_DEBUG("Initializing voice");

   //- get language of voice
   if (SUCCEEDED(voiceToken->GetStringValue(L"Language", &langID))) {
      (*voiceProperties)[ PROPERTY_KEY_LANGUAGE ] = W2A(langID);
      LOG_DEBUG("Language: "  << (*voiceProperties)[ PROPERTY_KEY_LANGUAGE ]);
   }

   //- get voice model directory
   if (!SUCCEEDED(hr = voiceToken->GetStringValue(L"Voxpath", &voicedir))) {
      LOG_DEBUG("Voice model directory not specified, quitting!");
      return hr;
   }
   else {
      (*voiceProperties)[ PROPERTY_KEY_VOICE_PATH ] = W2A(voicedir);
      LOG_DEBUG("Voice model: " << (*voiceProperties)[ PROPERTY_KEY_VOICE_PATH ]);
   }

   //- get text rules if available Textrules
   if (SUCCEEDED(voiceToken->GetStringValue(L"Textrules", &textrules))) {
      (*voiceProperties)[ PROPERTY_KEY_TEXTANALYZER_RULES ] = W2A(textrules);
      LOG_DEBUG("Voice text rules: " << (*voiceProperties)[ PROPERTY_KEY_TEXTANALYZER_RULES ]);
   }

   //TODO: voice name or other features?
   //TODO: preload voice?

   // yet to be checked:
   //if ((ttwv = feat_val(curr_vox->features, "tokentowords_func"))) {
   //    feat_set(curr_vox->features, "old_tokentowords_func", ttwv);
   //    feat_set(curr_vox->features, "tokentowords_func",
   //        itemfunc_val(sapi_tokentowords));
   //}

   return 0;
}

/******************************************************************************
* GetObjectToken
* Retrieve current token which contains info about the voice to be used.
******************************************************************************/
STDMETHODIMP HTSTTS::GetObjectToken(ISpObjectToken** ppToken) {
   return SpGenericGetObjectToken(ppToken, voiceToken);
}

/******************************************************************************
* GetOutputFormat
******************************************************************************/
STDMETHODIMP HTSTTS::GetOutputFormat(const GUID* pTargetFormatId,
                                     const WAVEFORMATEX* pTargetWaveFormatEx,
                                     GUID* pDesiredFormatId,
                                     WAVEFORMATEX** ppCoMemDesiredWaveFormatEx) {

   //return SpConvertStreamFormatEnum(SPSF_48kHz16BitMono,
   //                                 pDesiredFormatId,
   //                                 ppCoMemDesiredWaveFormatEx);
   WAVEFORMATEX* wfx;

   TTSResultPtr result = ttsManager.SynthesizeTextFragment(
                            TextFragmentPtr(new TextFragment("", this->voiceProperties)));

   if ((wfx = (WAVEFORMATEX*)CoTaskMemAlloc(sizeof(*wfx))) == NULL) {
      return E_OUTOFMEMORY;
   }
   memset(wfx, 0, sizeof(*wfx));
   wfx->nChannels = 1;
   wfx->nSamplesPerSec = result->GetSamplingRate();
   wfx->wFormatTag = WAVE_FORMAT_PCM;
   wfx->wBitsPerSample = 16;
   wfx->nBlockAlign = wfx->nChannels * wfx->wBitsPerSample / 8;
   wfx->nAvgBytesPerSec = wfx->nSamplesPerSec * wfx->nBlockAlign;

   *pDesiredFormatId = SPDFID_WaveFormatEx;
   *ppCoMemDesiredWaveFormatEx = wfx;

   return S_OK;
}


/******************************************************************************
* Speak
* SAPI interface function to trigger synthesis.
******************************************************************************/
STDMETHODIMP
HTSTTS::Speak(DWORD dwSpeakFlags,
              REFGUID rguidFormatId,
              const WAVEFORMATEX* pWaveFormatEx,
              const SPVTEXTFRAG* pTextFragList,
              ISpTTSEngineSite* pOutputSite) {

   LOG_DEBUG("BEGIN__SPEAK");

   const SPVTEXTFRAG* curr_frag;
   Text fullText;

   ResetActions();

   //- translate SAPI text fragments to our synthesizers text fragments
   for (curr_frag = pTextFragList; curr_frag; curr_frag = curr_frag->pNext) {

      char* tmptext;
      int len;
      LPCWSTR currStart = curr_frag->pTextStart;
      ULONG charsLeft = curr_frag->ulTextLen;
      ULONG currlen;
      ULONG FRAG_SIZE = 500;
      FragmentPropertiesPtr props = AdjustProperties(&(curr_frag->State), this->voiceProperties);

      switch (curr_frag->State.eAction) {
      //TODO: change some parameters for spellout/pronounce in text fragment properties and handle them
      case SPVA_SpellOut:
         LOG_DEBUG("[Speak] Should spell out something");
      case SPVA_Pronounce:
         LOG_DEBUG("[Speak] Should pronounce something");
      case SPVA_Speak:

         LOG_DEBUG("[Speak] Converting text");

         if (curr_frag->ulTextLen == 0) {
            continue;
         }

         while (charsLeft > 0) {
            if (charsLeft > FRAG_SIZE) {
               currlen = FRAG_SIZE;
            }
            else {
               currlen = charsLeft;
            }

            len = WideCharToMultiByte(CP_UTF8, 0, currStart, currlen, NULL, 0, NULL, NULL);

            if (len == 0) {
               continue;
            }

            tmptext = new char[ len + 1 ];
            WideCharToMultiByte(CP_UTF8, 0, currStart,  currlen, tmptext, len, NULL, NULL);
            tmptext[len] = 0;

            //TODO: copy and modify properties if this fragment has specific needs

            LOG_DEBUG("[Speak] Text = " << tmptext);
            fullText.push_back(TextFragmentPtr(new TextFragment(tmptext, props)));

            delete[] tmptext;
            tmptext = NULL;

            charsLeft -= currlen;
            currStart += currlen;
         }

         break;

      case SPVA_Silence:
         LOG_DEBUG("[Speak] Should do silence");
         break;
      case SPVA_Bookmark:
         break;
      default:
         break;
      }
   }

   //- now loop fragments, synthesize, play and handle actions
   std::vector<TextFragmentPtr>::iterator it = fullText.begin();
   for (; it != fullText.end(); ++it) {
      TextFragmentPtr tf = *it;

      LOG_DEBUG("[Speak] Handle actions");
      HandleActions(pOutputSite);
      HandleEventInterests(pOutputSite);

      if (actionAborted) {
         break;
      }

      LOG_DEBUG("[Speak] Synthesize text fragment");

      try {
         //TODO: modify volume and speaking rate according to live action events for future fragments
         //      combine it with values from SAPI XML
         TTSResultPtr result = ttsManager.SynthesizeTextFragment(tf);

         //TODO: is result->GetSamplingRate() correct?

         LOG_DEBUG("[Speak] Writing buffer");

         //- ugly code incoming again
         //- unfortunately we have to prebuffer the whole sample as the SAPI write
         //- function does not work on all tested systems
         //- when single samples (not bytes!) are written.
         //- we could use &result->GetFrames[0] as a double array
         //- or use templates for our TTSResults
         ULONG b;
         INT16* returnBuffer = new INT16[ result->GetFrames().size() ];
         for (std::vector<double>::size_type i = 0
                                                 ; i < result->GetFrames().size()
               ; ++i) {

            double sample = result->GetFrames()[i];
            if (sample > 32767.0)        {
               returnBuffer[i] = 32767;
            }
            else if (sample < -32768.0)  {
               returnBuffer[i] = -32768;
            }
            else                          {
               returnBuffer[i] = (INT16) sample;
            }
         }
         pOutputSite->Write(returnBuffer, (ULONG)result->GetFrames().size() * (ULONG)sizeof(*returnBuffer), &b);
         delete[] returnBuffer;

         LOG_DEBUG("[Speak] Writing buffer done");
      }
      catch (const PropertyMissingException& e) {
         LOG_DEBUG("[Speak] Property missing: " << e.what());
         //TODO: some sort of error messaging to SAPI
         return -1;
      }
      catch (const FileLoadException& e) {
         LOG_DEBUG("[Speak] File load failed: " << e.what());
         //TODO: some sort of error messaging to SAPI
         return -1;
      }
      catch (const std::exception& e) {
         LOG_DEBUG("[Speak] Exception occurred: " << e.what());
         //TODO: some sort of error messaging to SAPI
         return -1;
      }
   }
   LOG_DEBUG("END__SPEAK");
   return S_OK;
}


void HTSTTS::HandleEventInterests(ISpTTSEngineSite* site) {
   ULONGLONG  interest;
   if (site->GetEventInterest(&interest) == S_OK) {

      if (interest & SPEI_TTS_BOOKMARK) {
         LOG_DEBUG("[HandleEventInterests] Application interested in SPEI_TTS_BOOKMARK");
      }
      if (interest & SPEI_WORD_BOUNDARY) {
         LOG_DEBUG("[HandleEventInterests] Application interested in SPEI_WORD_BOUNDARY");
      }
      if (interest & SPEI_SENTENCE_BOUNDARY) {
         LOG_DEBUG("[HandleEventInterests] Application interested in SPEI_SENTENCE_BOUNDARY");
      }
      if (interest & SPEI_PHONEME) {
         LOG_DEBUG("[HandleEventInterests] Application interested in SPEI_PHONEME");
      }
      if (interest & SPEI_VISEME) {
         LOG_DEBUG("[HandleEventInterests] Application interested in SPEI_VISEME");
      }
   }

}

/******************************************************************************
* HandleActions
* Handles actions from the SAPI interface
******************************************************************************/
void HTSTTS::HandleActions(ISpTTSEngineSite* site) {
   DWORD actions;

   actions = site->GetActions();

   //- abort action
   if (actions & SPVES_ABORT) {
      actionAborted = true;
      return;
   }

   //- sentence skip action
   if (actions & SPVES_SKIP) {
      SPVSKIPTYPE stype;
      long count;

      site->GetSkipInfo(&stype, &count);

      if (stype == SPVST_SENTENCE) {
         actionSkipSentences += count;
      }
      LOG_DEBUG("[HandleActions] Skip sentences.");
   }

   //- change base speaking rate action
   if (actions & SPVES_RATE) {
      long adj;

      site->GetRate(&adj);

      ttsManager.SetBaseSpeakingRate(ConvertSapiRate(adj));
   }

   //- change base volume
   if (actions & SPVES_VOLUME) {
      USHORT adj;

      site->GetVolume(&adj);

      ttsManager.SetBaseVolume((int)adj);
   }
}



/******************************************************************************
* AdjustProperties
* Evaluate SAPI XML properties.
******************************************************************************/
FragmentPropertiesPtr HTSTTS::AdjustProperties(const SPVSTATE* state, FragmentPropertiesPtr props) {

   FragmentProperties* newProps = NULL;

   //TODO: if( state.EmphAdj )
   //TODO: if( state.PitchAdj.MiddleAdj )

   //- speaking rate changed for this fragment?
   if (state->RateAdj) {
      std::stringstream ss;
      ss << ConvertSapiRate(state->RateAdj);

      if (newProps == NULL) {
         newProps = new FragmentProperties(*props);
      }
      (*newProps)[PROPERTY_KEY_RATE] = ss.str();
   }

   //- volume changed for this fragment?
   if (state->Volume != 100) {
      std::stringstream ss;
      ss << state->Volume;

      if (newProps == NULL) {
         newProps = new FragmentProperties(*props);
      }
      (*newProps)[PROPERTY_KEY_VOLUME] = ss.str();
   }

   //- return either the new properties
   //- or if nothing has changed, the old ones
   if (newProps) {
      return FragmentPropertiesPtr(newProps);
   }
   else {
      return props;
   }
}


static const double sapi_ratetab_foo[21] = {
   0.5, 0.55, 0.6, 0.65, 0.7, 0.75, 0.8, 0.85, 0.9, 0.95,
   1.0,
   1.25, 1.5, 1.75, 2.0, 2.25, 2.5, 2.75, 3.0, 3.25, 3.5
};
static const double* sapi_ratetab = sapi_ratetab_foo + 10;

/******************************************************************************
* ConvertSapiRate
******************************************************************************/
static double ConvertSapiRate(int r) {
   if (r < -10) {
      r = -10;
   }
   else if (r > 10) {
      r = 10;
   }
   return sapi_ratetab[r];
}
