/**
* (c) 2013 Markus Toman, FTW GmbH
*
* HTS TTS system
**/

#include <string>
#include <iostream>
#include <sstream>
#include <cctype>

#include "utils.h"
#include "AlbanianNormalizer.h"

namespace htstts {


static const int    MAX_NONSPELL_NUMBER = 999999;
static const char*  MINUS = "minus";
static const char*  NUMBER_JOIN = "und";
static const char*  THOUSAND = "tausend";
static const char*  HUNDRED  = "hundert";
static const int    MAX_NUMBER = 20;
static const char*  NUMBERS[] = { "null", "eins", "zwei", "drei", "vier", "f\xc3\xbcnf", "sechs", "sieben", "acht", "neun", "zehn"
                                  , "elf", "zw\xc3\xb6lf", "dreizehn", "vierzehn", "f\xc3\xbcnfzehn", "sechzehn", "siebzehn", "achtzehn", "neunzehn", "zwanzig"
                                };
static const char*  NUMBERS_TOCONNECT[] = { "null", "ein", "zwei", "drei", "vier", "f\xc3\xbcnf", "sechs", "sieben", "acht", "neun", "zehn", "elf", "zw\xc3\xb6lf"
                                            , "elf", "zw\xc3\xb6lf", "dreizehn", "vierzehn", "f\xc3\xbcnfzehn", "sechzehn", "siebzehn", "achtzehn", "neunzehn", "zwanzig"
                                          };
static const char*  NUMBERS_TENS[] = { "", "zehn", "zwanzig", "dreissig", "vierzig", "f\xc3\xbcnfzig", "sechzig", "siebzig", "achtzig", "neunzig" };
static const char*  TEXT_COMMA_NAME = "komma";

static void convert_digit(char digit, std::stringstream& out) {
   if (digit >= '0' && digit <= '9') {
      out << NUMBERS[ digit - '0' ] << " ";
   }
}

static void convert_number_lt_thousand(int num, std::stringstream& out, bool connect = false) {
   // hundreds
   if (num >= 100) {
      out << NUMBERS_TOCONNECT[(num / 100)] << " ";
      out << HUNDRED  << " ";
      num %= 100;
      if (num == 0) {
         return;
      }
   }

   // tens
   if (num <= MAX_NUMBER) {
      if (connect) {
         out << NUMBERS_TOCONNECT[ num ]  << " ";
      }
      else {
         out << NUMBERS[ num ]  << " ";
      }
   }
   else {
      if (num % 10 > 0) {
         out << NUMBERS_TOCONNECT[ num % 10 ]  << " ";
         out << NUMBER_JOIN  << " ";
      }
      out << NUMBERS_TENS[num / 10]  << " ";
   }
}

static void convert_int(int num, std::stringstream& out)  {
   if (num < 0) {
      num *= -1;
      out << MINUS << " ";
   }

   // thousands
   if (num >= 1000) {
      convert_number_lt_thousand(num / 1000, out, true);
      out << THOUSAND  << " ";
      num %= 1000;
      if (num == 0) {
         return;
      }
   }
   convert_number_lt_thousand(num, out);
}

static bool convert_number(const std::string& s, std::stringstream& out, bool check = true)  {
   std::string::size_type i, numbegin = 0;
   std::string::size_type decimalpos = 0;
   char decimalchar;

   if (s[0] == '-') {
      numbegin = 1;
   }

   if (check) {
      for (i = numbegin; i < s.length() ; i += UTF8_CHAR_LEN(s[i])) {
         // UTF8 multibyte character -> no number
         if (UTF8_CHAR_LEN(s[i]) > 1) {
            return false;
         }

         char c = s[i];
         // decimal "." or ","
         if (c == '.' || c == ',') {
            decimalchar = c;
            // already got a decimal or no number till now - leave:
            if (decimalpos > 0 || i == numbegin) {
               return false;
            }
            decimalpos = i;
         }
         else if (!std::isdigit(s[i])) {
            return false;
         }
      }
   }

   // if we got a decimal point, split the number up
   if (decimalpos > 0) {
      convert_number(s.substr(0, decimalpos), out, false);

      // only if decimal char is not at the end
      if (decimalpos + 1 < s.length()) {
         out << " " << TEXT_COMMA_NAME << " ";
         convert_number(s.substr(decimalpos + 1), out, false);
      }
      else {
         out << " " << decimalchar << " ";
      }
   }
   // so it is a number, then actually convert it.
   else {
      int num;
      std::stringstream ss(s);
      ss >> num;
      if (num > MAX_NONSPELL_NUMBER) {
         for (i = 0; i < s.length() ; ++i) {
            convert_digit(s[i], out);
         }
      }
      else {
         convert_int(num, out);
      }
   }
   return true;
}



AlbanianNormalizer AlbanianNormalizer::instance;

AlbanianNormalizer::AlbanianNormalizer() {
}

AlbanianNormalizer::~AlbanianNormalizer() {
}


/*
* Normalize
*/
std::string AlbanianNormalizer::Normalize(const std::string& text) {
   std::stringstream output;
   std::stringstream cleanedText;
   std::string currword;

   // handle special characters
   for (std::string::size_type i = 0; i < text.length() ; i += UTF8_CHAR_LEN(text[i])) {

      // keep utf8 multibyte chars
      if (UTF8_CHAR_LEN(text[i]) > 1) {
         //TODO: this is just a hack to handle sz
         if (text[i] == '\xc3' && text[i + 1] == '\x9f') {
            cleanedText << "ss";
         }
         else {
            cleanedText << text.substr(i, UTF8_CHAR_LEN(text[i]));
         }
         continue;
      }


      // keep regular characters
      if (::isalpha(text[i]) || ::isdigit(text[i])) {
         cleanedText << text[i];
         continue;
      }

      // special characters
      if (text[i] == '.' || text[i] == ',') {
         // if it is between digits, we ignore it
         if (i > 0 && i + 1 < text.length()
               && std::isdigit(text[i - 1]) && std::isdigit(text[i + 1])) {

            cleanedText << text[i];
         }
         // if there is no space following after ".", we ignore it (also if its at the end of the text)
         else if (text[i] == '.' && (i + 1 >= text.length() || text[i + 1] != ' ')) {
            //TODO... hmm, just ignore it?
         }
         // else we treat it as a word
         else {
            cleanedText << " " << text[i] << " ";
         }
      }
      // all other special characters are replaced with " "
      else {
         cleanedText << " ";
      }
   }


   std::stringstream input(cleanedText.str());

   // now find stuff to normalize on word level
   while (input >> currword) {
      if (convert_number(currword, output)) {
         continue;
      }
      output << currword << " ";
   }
   return output.str();
}



}