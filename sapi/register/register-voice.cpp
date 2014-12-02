///////////////////////////////////////////////////////////////////////////
//                                                                       //
//                           Cepstral, LLC                               //
//                        Copyright (c) 2001                             //
//                        All Rights Reserved.                           //
//                                                                       //
//  Permission is hereby granted, free of charge, to use and distribute  //
//  this software and its documentation without restriction, including   //
//  without limitation the rights to use, copy, modify, merge, publish,  //
//  distribute, sublicense, and/or sell copies of this work, and to      //
//  permit persons to whom this work is furnished to do so, subject to   //
//  the following conditions:                                            //
//   1. The code must retain the above copyright notice, this list of    //
//      conditions and the following disclaimer.                         //
//   2. Any modifications must be clearly marked as such.                //
//   3. Original authors' names are not deleted.                         //
//   4. The authors' names are not used to endorse or promote products   //
//      derived from this software without specific prior written        //
//      permission.                                                      //
//                                                                       //
//  CEPSTRAL, LLC AND THE CONTRIBUTORS TO THIS WORK DISCLAIM ALL         //
//  WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING ALL IMPLIED       //
//  WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL         //
//  CEPSTRAL, LLC NOR THE CONTRIBUTORS BE LIABLE FOR ANY SPECIAL,        //
//  INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER          //
//  RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION    //
//  OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR  //
//  IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.          //
//                                                                       //
///////////////////////////////////////////////////////////////////////////
//             Author:  David Huggins-Daines (dhd@cepstral.com)          //
//               Date:  November 2001                                    //
///////////////////////////////////////////////////////////////////////////

#pragma warning(disable: 4996)

#include <atlbase.h>
#include <stdio.h>
#include <sphelper.h>

#include <iostream>

#include "../htssapi_i.c"

#define VOICE_LANG_DE 0x407
#define VOICE_LANG_DE_STR L"407"
#define VOICE_LANG_DE_AT_STR L"c07"
#define VOICE_LANG_EN 0x409
#define VOICE_LANG_EN_STR L"409"

// converts a language id to a language string
// if we get more languages in future, this should be replaced by an actual function
#define VOICE_LANG_ID_TO_STR(langid)  ( langid == VOICE_LANG_DE ? VOICE_LANG_DE_STR : VOICE_LANG_EN_STR )

// converts a language string to a language id
// if we get more languages in future, this should be replaced by an actual function
#define VOICE_LANG_STR_TO_ID(langstr)  ( !wcscmp( langstr, VOICE_LANG_DE_STR ) ? VOICE_LANG_DE : VOICE_LANG_EN )


int wmain(int argc, WCHAR* argv[]) {
   USES_CONVERSION;
   HRESULT hr;

   ::CoInitialize(NULL);

   bool toRemove = false;

   ISpObjectToken* cpToken;
   ISpDataKey* cpDataKeyAttribs;

   WCHAR* voicePath = L"C:\\leo.htsvoice";
   WCHAR* voiceName = L"SALB Leo";
   WCHAR* voiceLang = L"de-at";
   WCHAR* voiceGender = L"male";
   WCHAR* voiceAge = L"Adult";
   WCHAR* voiceLog = L"leo.log";
   WCHAR* voiceRules = L"";
   int    langID = VOICE_LANG_DE;
   int argnum = 1;

   if (argc <= 1) {
      std::cout << "Usage: register-voice <model> <name> <language> <gender> <age> <log> <textrules>" << std::endl;
   }

   if (argc > argnum) {
      if (argv[argnum][0] == L'/' && argv[argnum][1] == L'u') {
         printf("Unregistering voice\n");
         toRemove = true;
         ++argnum;
      }

      if (argc > argnum) {
         voicePath = argv[argnum];
         ++argnum;
      }
   }
   if (argc > argnum) {
      voiceName = argv[argnum];
      ++argnum;
   }
   if (argc > argnum) {
      voiceLang = argv[argnum];

      if (voiceLang[0] == L'd' && voiceLang[1] == L'e') {
         langID = VOICE_LANG_DE;
      }
      else {
         langID = VOICE_LANG_EN;
      }
      ++argnum;
   }
   if (argc > argnum) {
      voiceGender = argv[argnum];
      ++argnum;
   }
   if (argc > argnum) {
      voiceAge = argv[argnum];
      ++argnum;
   }
   if (argc > argnum) {
      voiceLog = argv[argnum];
      ++argnum;
   }
   if (argc > argnum) {
      voiceRules = argv[argnum];
      ++argnum;
   }


   printf("Registering %s with language %s on path: %s\n\r", W2A(voiceName), W2A(voiceLang), W2A(voicePath));

   hr = SpCreateNewTokenEx(
           SPCAT_VOICES,
           voiceName,                    //  A unique name for your voice.
           &CLSID_HTSTTS,                // The CLSID for your voice object.
           voiceName,                    // Language-independent full name.
           langID,                       //  Language ID (US English is 0x409).
           voiceName,                    // Language-dependent full name.

           &cpToken,
           &cpDataKeyAttribs);

   //- remove the token
   if (toRemove) {
      cpToken->Remove(NULL);
   }

   //--- Set additional attributes for searching and the path to the
   //    voice data file we just created.
   else if (SUCCEEDED(hr)) {
      /* The gender of your speaker. */
      //TODO.. provide this on commandline?
      hr = cpDataKeyAttribs->SetStringValue(L"Gender",  voiceGender);

      /* The name of your speaker. */
      if (SUCCEEDED(hr)) {
         hr = cpDataKeyAttribs->SetStringValue(L"Name", voiceName);
      }

      /* The language IDs supported by your voice. */
      if (SUCCEEDED(hr)) {
         hr = cpDataKeyAttribs->SetStringValue(L"Language",
                                               VOICE_LANG_ID_TO_STR(langID));
         hr = cpToken->SetStringValue(L"Language",
                                      voiceLang);
      }

      /* The general age of your speaker (Adult, Child). */
      if (SUCCEEDED(hr)) {
         hr = cpDataKeyAttribs->SetStringValue(L"Age", voiceAge);
      }

      /* The vendor name for your voice. */
      if (SUCCEEDED(hr)) {
         hr = cpDataKeyAttribs->SetStringValue(L"Vendor", L"FTW");
      }

      /* path to the vox file */
      if (SUCCEEDED(hr)) {
         hr = cpToken->SetStringValue(L"Voxpath", voicePath);
         hr = cpDataKeyAttribs->SetStringValue(L"Voxpath", voicePath);
      }

      // voice rules
      if (SUCCEEDED(hr)) {
         hr = cpToken->SetStringValue(L"Textrules", voiceRules);
      }

      if (SUCCEEDED(hr) && argc > 2) {
         /* path to the vox file */
         printf("Using Logfile: %s\n\r", W2A(voiceLog));
         hr = cpToken->SetStringValue(L"Logfile", voiceLog);
      }
   }

   if (FAILED(hr)) {
      fprintf(stderr, "SpCreateNewTokenEx failed, code %x\n",
              hr);
      ::CoUninitialize();
      return FAILED(hr);
   }



   ::CoUninitialize();

   return 0;
}
