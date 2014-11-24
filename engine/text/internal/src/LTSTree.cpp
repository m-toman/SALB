/**
* (c) 2013 Markus Toman, FTW GmbH
*
* HTS TTS system
**/

#include <string>
#include <iostream>
#include <sstream>

#include "common.h"
#include "utils.h"
#include "LTSTree.h"

namespace htstts {

LTSTree::LTSTree() {
}

LTSTree::~LTSTree() {
}

int PeekNonWhitespace(std::istream& in) {
   int peekchar;
   peekchar = in.peek();
   while (peekchar == ' ' || peekchar == '\n' || peekchar == '\t' || peekchar == '\r') {
      in.get();
      peekchar = in.peek();
   }
   return peekchar;
}

std::string LTSTree::ParseValue(std::istream& in) {
   std::string line;
   getline(in, line, ')');
   return line;
}

void LTSTree::Question::Init(const std::string& cond, const std::string& val) {
   this->value = val;
   pos = 0;

   for (std::string::size_type start = 0;
         (start = cond.find("n.", start)) != std::string::npos; ++pos) {
      start += 2;
   }
   for (std::string::size_type start = 0;
         (start = cond.find("p.", start)) != std::string::npos; --pos) {
      start += 2;
   }
}

void LTSTree::ParseNodeValue(const std::string& val, NodePtr node) {
   //TODO: trim() everything
   std::string::size_type pos = val.find(" is ");
   // value
   if (pos == std::string::npos) {
      node->valueNode = true;
      node->value = val;
   }
   else {
      node->valueNode = false;
      node->question.Init(val.substr(0, pos), val.substr(pos + 4));
   }
}

void LTSTree::ParseExpression(std::istream& in, NodePtr node) {
   int peekchar;

   if ((peekchar = PeekNonWhitespace(in)) == std::char_traits<char>::eof()) {
      return;
   }

   //- another expression begins, recursive call
   if (peekchar == '(') {
      in.get();
      ParseExpression(in, node);
   }
   //- else parse condition, true and false expression
   else {
      // get the question for this node
      std::string val = ParseValue(in);
      ParseNodeValue(val, node);

      // expression ends here?
      if ((peekchar = PeekNonWhitespace(in)) == std::char_traits<char>::eof()) {
         return;
      }
      if (peekchar == ')') {
         in.get();
         return;
      }
      // expression for "true" condition available?
      if (peekchar == '(') {
         in.get();
         node->trueChild = std::make_shared<Node>();
         ParseExpression(in, node->trueChild);
      }

      // expression ends here?
      if ((peekchar = PeekNonWhitespace(in)) == std::char_traits<char>::eof()) {
         return;
      }
      if (peekchar == ')') {
         in.get();
         return;
      }
      // expression for "false" condition available?
      if (peekchar == '(') {
         in.get();
         node->falseChild = std::make_shared<Node>();
         ParseExpression(in, node->falseChild);
      }

      // here it should really end
      if ((peekchar = PeekNonWhitespace(in)) == std::char_traits<char>::eof()) {
         return;
      }
      if (peekchar != ')') {
         throw FileLoadException("Invalid character in LTS tree");
      }
      in.get();
   }
}

/*
* Read
*/
void LTSTree::Read(std::istream& in) {
   std::string tmp;
   std::string line;

   // read header
   getline(in, line);
   getline(in, line);

   int peekchar;
   if ((peekchar = PeekNonWhitespace(in)) == std::char_traits<char>::eof()) {
      return;
   }

   // ignore one level of "()s"
   in.get();
   if ((peekchar = PeekNonWhitespace(in)) == std::char_traits<char>::eof()) {
      return;
   }

   // on top level, we have an expression for every center character, we build a hash map for this
   //- another expression begins, recursive call
   while (peekchar == '(') {
      in.get();
      std::string centerCharacter;
      in >> centerCharacter;

      NodePtr node = std::make_shared<Node>();

      ltsTrees[ centerCharacter ] = node;

      // now build the tree for centerCharacter
      if ((peekchar = PeekNonWhitespace(in)) == std::char_traits<char>::eof()) {
         return;
      }
      if (peekchar == '(') {
         in.get();
         ParseExpression(in, node);
      }
      else {
         //no tree for this character?
         throw FileLoadException("Invalid character in LTS tree");
      }

      // find our closing ")"
      if ((peekchar = PeekNonWhitespace(in)) == std::char_traits<char>::eof()) {
         return;
      }
      if (peekchar != ')') {
         throw FileLoadException("Invalid character in LTS tree");
      }
      in.get();
      if ((peekchar = PeekNonWhitespace(in)) == std::char_traits<char>::eof()) {
         return;
      }
   }
}


/*
* GetWord
*/
utt::WordPtr LTSTree::GetWord(const std::string& input) {

   utt::WordPtr word = std::make_shared<utt::Word>();

   //TODO: syllabification
   utt::SyllablePtr syl(new utt::Syllable());
   std::string text = input;
   syl->stressed = 0;
   word->syllables.push_back(syl);

   utf8_tolower(text);

   for (std::string::size_type i = 0; i < text.length();  i += UTF8_CHAR_LEN(text[i])) {

      std::string c = text.substr(i, UTF8_CHAR_LEN(text[i]));

      // get tree for this character
      if (ltsTrees.find(c) == ltsTrees.end()) {
         //TODO: unknown character, log that?
         continue;
      }

      NodePtr node = ltsTrees[c];
      // walk the tree
      while (node != nullptr && !node->valueNode) {
         int qpos = (int)i + node->question.pos;

         // either out of bounds and value is 0 or # and on bounds or inbound and value == text[qpos] then question is true
         if (((node->question.value == "#") && (qpos == -1 || qpos == (int)text.size())) ||
               ((node->question.value == "0") && (qpos <  -1 || qpos > (int)text.size()))   ||
               ((qpos >= 0 && qpos < (int)text.size() && text.substr(qpos, 1) == node->question.value))) {
            node = node->trueChild;
         }
         else {
            node = node->falseChild;
         }
      }

      // now add the phone if node found and value != epsilon
      if (node && node->value != "_epsilon_") {
         // multiple phones might be split by "-"
         std::string val;
         std::stringstream valstream(node->value);

         while (getline(valstream, val, '-')) {
            // perhaps in future use a fixed phone set with
            // additional phone information
            utt::PhonePtr phone = std::make_shared<utt::Phone>();
            phone->symbol = val;
            syl->phones.push_back(phone);
         }
      }
   }

   return (syl->phones.size() == 0) ? nullptr : word;
}

}