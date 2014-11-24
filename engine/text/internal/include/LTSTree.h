/**
 * (c) 2013 Markus Toman, FTW GmbH
 *
 * HTS TTS system
 **/

#ifndef FTW_TTSLTSTREE
#define FTW_TTSLTSTREE

#include <string>
#include <vector>
#include <memory>
#include <iostream>
#include <unordered_map>

#include "Utterance.h"


namespace htstts {

/**
 * Represents a lexicon with mappings from words to phonetic/syllable information.
 *
 **/
class LTSTree {
   private:

      /**
       * Represents an LTS tree question.
       * Example: "left left character is 'n'?"
       **/
      struct Question {
         int pos;
         std::string value;

         void Init(const std::string& cond, const std::string& val);
      };

      /**
       * Represents a node in the LTS tree.
       **/
      struct Node {
         Question question;
         std::string value;
         std::shared_ptr<Node> trueChild, falseChild;
         bool valueNode;

         Node() {
            valueNode = false;
            trueChild = falseChild = nullptr;
         }
      };
      typedef std::shared_ptr<Node> NodePtr;

      std::unordered_map< std::string, NodePtr > ltsTrees;

      void ParseExpression(std::istream& in, NodePtr node);
      void ParseNodeValue(const std::string& val, NodePtr node);
      std::string ParseValue(std::istream& in);
   public:
      LTSTree();
      ~LTSTree();

      /**
       * Reads lexicon data from supplied stream.
       **/
      void Read(std::istream& in);

      /**
       * Returns the word object for text or nullptr.
       **/
      utt::WordPtr GetWord(const std::string& text);

      inline utt::WordPtr operator[](const std::string& text) {
         return GetWord(text);
      }

};

}

#endif