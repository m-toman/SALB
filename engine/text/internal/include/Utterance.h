/**
 * (c) 2013 Markus Toman, FTW GmbH
 *
 * HTS TTS system
 **/

#ifndef FTW_TTSUTTERANCE
#define FTW_TTSUTTERANCE


#include <vector>
#include <list>
#include <memory>
#include <string>
#include <sstream>

namespace htstts {

namespace utt {

//TODO: reasonable places to store the sil and pau symbol(s)?
#define SILENCE_SYMBOL "sil";
#define PAUSE_SYMBOL "pau";

/**
 * Represents a single phone.
 * In future we might add phone information here and the lexicon might use a fixed set of phone objects.
 **/
struct Phone {
   std::string symbol;
};
typedef std::shared_ptr<Phone> PhonePtr;

/**
 * Represents a single syllable.
 *
 * A syllable consists of one or more phones.
 **/
struct Syllable {
   std::list<PhonePtr> phones;
   bool stressed;          ///< Syllables can be stressed.

   Syllable() : stressed(false) {}
};
typedef std::shared_ptr<Syllable> SyllablePtr;


/**
 * Represents a single word.
 *
 * A word consists of one or more syllables.
 **/
struct Word {
   std::string orthography;
   std::list<SyllablePtr> syllables;
};
typedef std::shared_ptr<Word> WordPtr;

/**
 * Create a word that represents silence.
 **/
inline WordPtr make_sil_word() {
   WordPtr word = std::make_shared<Word>();
   SyllablePtr syl = std::make_shared<Syllable>();
   PhonePtr phone = std::make_shared<Phone>();
   phone->symbol = SILENCE_SYMBOL;
   syl->phones.push_back(phone);
   word->syllables.push_back(syl);
   return word;
}

/**
 * Create a word that represents pause.
 **/
inline WordPtr make_pau_word() {
   WordPtr word = std::make_shared<Word>();
   SyllablePtr syl = std::make_shared<Syllable>();
   PhonePtr phone = std::make_shared<Phone>();
   phone->symbol = PAUSE_SYMBOL;
   syl->phones.push_back(phone);
   word->syllables.push_back(syl);
   return word;
}

/**
 * Produce a lexicon entry in the Lisp form, e.g.
 * ("abbau" N ((( GS a p )1) (( b aU )0)))
 **/
inline std::string make_lex_entry(WordPtr word, const std::string& orthography) {
   std::stringstream entry;
   entry << "(\"" << orthography << "\" X (";
   for (auto syl : word->syllables) {
      entry << " (( ";
      for (auto p : syl->phones) {
         entry << p->symbol << " ";
      }
      entry << ")" << (syl->stressed ? "1" : "0") << ")";
   }
   entry << "))";
   return entry.str();
}

inline std::string make_lex_entry(WordPtr word) {
   return make_lex_entry(word, word->orthography);
}

/**
 * returns true if a phone object is a silence
 **/
inline bool is_sil(const PhonePtr& p)   {
   return p->symbol == SILENCE_SYMBOL;
}

/**
 * returns true if a word object is just a silence
 **/
inline bool is_sil(const WordPtr& w)   {
   return (w->syllables.size() == 1 &&
           w->syllables.front()->phones.size() == 1 &&
           is_sil(w->syllables.front()->phones.front()));
}

/**
 * Represents a single phrase.
 *
 * A phrase consists of one or more words.
 **/
struct Phrase {
   std::list<WordPtr> words;
};
typedef std::shared_ptr<Phrase> PhrasePtr;


/**
 * Iterates phones of a phrase.
 * This is actually a tree iterator that
 * selects a specific path at a time and can be moved
 * on different levels of the tree.
 * Rationale why we don't use a lean iterator here:
 * Phone, Syllable and Word object might be used by multiple
 * utterances, a pointer to its parent is therefore not unique.
 * We assume here that a word, a syllable and a phone is never empty!
 **/
class PhraseIterator {
      PhrasePtr phrase;
      std::list<WordPtr>::iterator currWord;
      std::list<SyllablePtr>::iterator currSyllable;
      std::list<PhonePtr>::iterator currPhone;
      bool valid;

   public:
      PhraseIterator() : phrase(nullptr) {
         valid = false;
      }
      PhraseIterator(PhrasePtr& _phrase) : phrase(_phrase) {
         currWord = phrase->words.begin();
         if (currWord == phrase->words.end()) {
            valid = false;
         }
         else {
            // a word and a syllable must not be empty
            currSyllable = (*currWord)->syllables.begin();
            currPhone = (*currSyllable)->phones.begin();
            valid = true;
         }
      }

      inline bool isValid() {
         return valid;
      }


      /**
       * Set the iterator to the begining of the next word.
       **/
      inline PhraseIterator& NextWord() {
         if (valid) {
            // try next word, if it is the last word, we are done
            if (++currWord == phrase->words.end()) {
               valid = false;
            }
            // else we can set the other iterators to the beginning of that word
            else {
               currSyllable = (*currWord)->syllables.begin();
               currPhone = (*currSyllable)->phones.begin();
            }
         }
         return *this;
      }

      /**
       * Set the iterator to the previous word.
       * The other iterators will then point to the last syllable in the new
       * word and to the last phone in the new syllable.
       **/
      inline PhraseIterator& PrevWord() {
         if (valid) {
            // if this is the first word, we are done and invalid now
            if (currWord == phrase->words.begin()) {
               valid = false;
            }
            // else we can decrease the word and update the other iterators
            // (i.e. set them to the last syllable in the word and the last
            //  phone in this syllable)
            else {
               --currWord;
               currSyllable = --((*currWord)->syllables.end());
               currPhone = --((*currSyllable)->phones.end());
            }
         }
         return *this;
      }

      /**
       * Set the iterator to the beginning of the next syllable in this word.
       * If this word is at its end, go on to the next word.
       **/
      inline PhraseIterator& NextSyllable() {
         if (valid)  {
            // try next syllable, if it is at end, try next word
            if (++currSyllable == (*currWord)->syllables.end()) {
               NextWord();
            }
            // just increased our syllable, set the phone accordingly
            else {
               currPhone = (*currSyllable)->phones.begin();
            }
         }
         return *this;
      }

      /**
       * Set the iterator to the end of the previous syllable in this word.
       * If this word is at its end, go to the previous word.
       * The phone iterator will the point to the last phone in the new syllable.
       **/
      inline PhraseIterator& PrevSyllable() {
         if (valid) {
            // at the beginning of the syllable, go to previous word
            if (currSyllable == (*currWord)->syllables.begin()) {
               PrevWord();
            }
            // else go to previous syllable and set phone iterator to end of new syllable
            else {
               --currSyllable;
               currPhone = --((*currSyllable)->phones.end());
            }
         }
         return *this;
      }

      /**
       * Set the iterator to the next phone.
       * If the phone is the last phone in this syllable, go to the first phone
       * of the next syllable.
       **/
      inline PhraseIterator& NextPhone() {
         if (valid) {
            // try next phone, if we are at the end, try next syllable
            if (++currPhone == (*currSyllable)->phones.end()) {
               NextSyllable();
            }
         }
         return *this;
      }

      /**
       * Set the iterator to the previous phone.
       * If the phone is the first phone in this syllable, go to the last phone
       * of the previous syllable.
       **/
      inline PhraseIterator& PrevPhone() {
         if (valid) {
            // phone iterator at beginning, we have to go to previous syllable
            if (currPhone == (*currSyllable)->phones.begin()) {
               PrevSyllable();
            }
            else {
               --currPhone;
            }
         }
         return *this;
      }

      /**
       * prefix increment operator
       **/
      inline PhraseIterator& operator++() {
         return NextPhone();
      }

      /**
       * prefix decrement operator
       **/
      inline PhraseIterator& operator--() {
         return PrevPhone();
      }

      inline PhraseIterator operator++(int) {
         PhraseIterator temp = *this;
         ++*this;
         return temp;
      }

      inline PhraseIterator operator--(int) {
         PhraseIterator temp = *this;
         --*this;
         return temp;
      }

      inline int GetSyllableIndexInPhrase() {
         int sylpos = GetSyllableIndexInWord();
         PhraseIterator it = *this;
         while (it.PrevWord().isValid()) {
            sylpos += (int)it.GetCurrentWord()->syllables.size();
         }
         return sylpos;
      }

      inline int GetSyllablesInPhrase() {
         int num = 0;
         PhraseIterator it(phrase);
         while (it.isValid()) {
            num += (int)it.GetCurrentWord()->syllables.size();
            it.NextWord();
         }
         return num;
      }

      inline WordPtr GetCurrentWord() {
         return valid ? *currWord : nullptr;
      }
      inline SyllablePtr GetCurrentSyllable() {
         return valid ? *currSyllable : nullptr;
      }
      inline PhonePtr GetCurrentPhone() {
         return valid ? *currPhone : nullptr;
      }
      inline PhrasePtr GetPhrase() {
         return valid ? phrase : nullptr;
      }
      inline int GetPhoneIndexInSyllable() {
         return (int)std::distance((*currSyllable)->phones.begin(), currPhone);
      }
      inline int GetSyllableIndexInWord() {
         return (int)std::distance((*currWord)->syllables.begin(), currSyllable);
      }
      inline int GetWordIndex() {
         return (int)std::distance(phrase->words.begin(), currWord);
      }
};

}
}
#endif