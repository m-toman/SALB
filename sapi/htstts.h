// htstts.h : Declaration of the HTSTTS

#pragma once

#include <atlbase.h>
#include <atlcom.h>

using namespace ATL;

#include <spddkhlp.h>
#include <spcollec.h>


#include "resource.h"       // main symbols
#include "htssapi_i.h"

#include "common.h"
#include "TTSManager.h"




#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Single-threaded COM objects are not properly supported on Windows CE platform, such as the Windows Mobile platforms that do not include full DCOM support. Define _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA to force ATL to support creating single-thread COM object's and allow use of it's single-threaded COM object implementations. The threading model in your rgs file was set to 'Free' as that is the only threading model supported in non DCOM Windows CE platforms."
#endif




/**
 * Represents the hts_engine TTS engine SAPI object.
 * This is for communication of SAPI with hts_engine and text analysis.
 *
 **/
class ATL_NO_VTABLE HTSTTS :
   public CComObjectRootEx<CComMultiThreadModel>,
   public ISpTTSEngine,
   public ISpObjectWithToken,
   public CComCoClass<HTSTTS, &CLSID_HTSTTS>
   //,public IDispatchImpl<IHTSTTS, &IID_IHTSTTS, &LIBID_htssapiLib, /*wMajor =*/ 1, /*wMinor =*/ 0>
{
   public:
      HTSTTS();
      ~HTSTTS();

      //================= SAPI interface methods //=================

      // ISpObjectWithToken
      // interface implementation methods
      STDMETHODIMP SetObjectToken(ISpObjectToken* pToken);
      STDMETHODIMP GetObjectToken(ISpObjectToken** ppToken);

      // ISpTTSEngine methods
      // interface implementation methods
      STDMETHOD(Speak)(DWORD dwSpeakFlags,
                       REFGUID rguidFormatId, const WAVEFORMATEX* pWaveFormatEx,
                       const SPVTEXTFRAG* pTextFragList, ISpTTSEngineSite* pOutputSite);
      STDMETHOD(GetOutputFormat)(const GUID* pTargetFormatId,
                                 const WAVEFORMATEX* pTargetWaveFormatEx,
                                 GUID* pDesiredFormatId,
                                 WAVEFORMATEX** ppCoMemDesiredWaveFormatEx);


      //================= COM stuff ======================
   public:
      DECLARE_REGISTRY_RESOURCEID(IDR_HTSTTS)

      BEGIN_COM_MAP(HTSTTS)
      //COM_INTERFACE_ENTRY(IHTSTTS)
      //COM_INTERFACE_ENTRY(IDispatch)
      COM_INTERFACE_ENTRY(ISpTTSEngine)
      COM_INTERFACE_ENTRY(ISpObjectWithToken)
      END_COM_MAP()

      DECLARE_PROTECT_FINAL_CONSTRUCT()

      HRESULT FinalConstruct() {
         return S_OK;
      }

      void FinalRelease() {
      }

      //================= INTERNALS =======================
   private:
      CComPtr<ISpObjectToken> voiceToken;
      htstts::FragmentPropertiesPtr voiceProperties;       ///< default properties for synthesis

      bool actionAborted;
      int  actionSkipSentences;

      void ResetActions();
      void HandleActions(ISpTTSEngineSite* site);
      void HandleEventInterests(ISpTTSEngineSite* site);
      htstts::FragmentPropertiesPtr AdjustProperties(const SPVSTATE* state, htstts::FragmentPropertiesPtr props);



};

OBJECT_ENTRY_AUTO(__uuidof(HTSTTS), HTSTTS)
