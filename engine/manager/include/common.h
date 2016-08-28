/**
* (c) 2013 Markus Toman, FTW GmbH
*
* HTS TTS system
**/

#ifndef FTW_TTSCOMMON
#define FTW_TTSCOMMON


#include <string>
#include <memory>
#include <vector>
#include <unordered_map>
#include <exception>
#include <sstream>

namespace htstts {

#define PROPERTY_KEY_SYNTHESIZER "synthesizer"      ///< property defining which synthesizer to use
#define PROPERTY_KEY_TEXTANALYZER "tanalyzer"       ///< property defining which text analyzer to use
#define PROPERTY_KEY_TEXTANALYZER_RULES "trules"    ///< property defining path to a file with text analysis rules
#define PROPERTY_KEY_LANGUAGE "lang"                ///< property defining the language of the text fragment
#define PROPERTY_KEY_VOICE_NAME "vName"             ///< property defining the name of the voice to use
#define PROPERTY_KEY_VOICE_PATH "vPath"             ///< property defining the path to the voice model
#define PROPERTY_KEY_VOLUME "vol"                   ///< property defining the synthesis volume (0-100)
#define PROPERTY_KEY_RATE   "rate"                  ///< property defining the synthesis speaking rate (~0.5-2.0)
#define PROPERTY_KEY_PITCH  "pitch"                 ///< property defining the synthesis pitch (0.0 - no change)

#define PROPERTY_KEY_FRAGMENT_NOSIL_BEGIN  "silbeg"   ///< no silence at beginning of fragment (values: PROPERTY_VALUE_TRUE or PROPERTY_VALUE_FALSE)
#define PROPERTY_KEY_FRAGMENT_NOSIL_END    "silend"   ///< no silence at end of fragment (values: PROPERTY_VALUE_TRUE or PROPERTY_VALUE_FALSE)

#define PROPERTY_VALUE_TRUE "1"
#define PROPERTY_VALUE_FALSE "0"
#define PROPERTY_VALUE_AUTOMATIC "automatic"        ///< property value for automatic choices
#define PROPERTY_VALUE_FLITE "flite"                ///< property value for flite as text analyzer
#define PROPERTY_VALUE_INTERNAL "internal"          ///< property value for using the internal text analyzer
#define PROPERTY_VALUE_HTSENGINE "htsengine"        ///< property value for hts_engine as synthesis engine

#define DEFAULT_SAMPLING_RATE 48000                 ///< default sampling rate


//TODO: Exception OperationNotSupported

/**
* Represents meta information on text fragments.
**/
class FragmentProperties : public std::unordered_map<std::string, std::string> {
   public:
      FragmentProperties() {}

      std::string Serialize() {
         std::ostringstream ostr;
         for (auto const& p : *this) {
            ostr << "|" << p.first << "=" << p.second;
         }
         return ostr.str();
      }

      bool Exists(const std::string& key) const {
         return find(key) != end();
      }

      /**
      * Returns the value for key as std::string.
      * If key does not exist, returns defaultValue
      **/
      const std::string& Get(const std::string& key, const std::string& defaultValue) const {
         if (find(key) != end()) {
            return at(key);
         }
         return defaultValue;
      }

      /**
      * Returns the value for key as type T.
      * If key does not exist or can not be converted, returns defaultValue.
      **/
      template<typename T> T GetAs(const std::string& key, T defaultValue) const {
         if (find(key) != end()) {
            std::istringstream i(at(key));
            i.imbue(std::locale::classic());
            T val;
            if (i >> val) {
               return val;
            }
         }
         return defaultValue;
      }
};
typedef std::shared_ptr<FragmentProperties> FragmentPropertiesPtr;

/**
 * Thrown if some property is missing.
 **/
class PropertyMissingException : public std::exception {
      std::string prop;
   public:
      PropertyMissingException(const std::string& _prop) : prop(_prop) {  }
      virtual const char* what() const throw()  {
         return prop.c_str();
      }
      ~PropertyMissingException() throw() {}
};

/**
 * Thrown if some necessary file is missing or could not be loaded.
 **/
class FileLoadException : public std::exception {
      std::string filename;
   public:
      FileLoadException(const std::string& _filename) : filename(_filename) {  }
      virtual const char* what() const throw()  {
         return filename.c_str();
      }
      ~FileLoadException() throw() {}
};

/**
* Clamps a value between lower and upper.
**/
template <typename T>
inline T clamp(const T& x, const T& lower, const T& upper) {
   return x < lower ? lower : (x > upper ? upper : x);
}


}

#endif
