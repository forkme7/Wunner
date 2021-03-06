/*
 *
 * Author : Anishka Gupta
 * Created on : August 26, 2017
 *
 */

#include "autocomplete.hpp"

namespace wunner
{
  
  Node::Node()
  {
      for (int i = 0; i < ALPHABET_SIZE; i++) {
          children[i] = NULL;
      }
      is_leaf = false;
  }

  Trie::Trie()
  {
      root = new Node();
  }
  
  void Trie::insert(std::string const & query)
  {
      Node* curr = root;
      for (size_t i = 0; i < query.length(); i++) {
          int index = (int)query[i] - ASCII_START_CODE;
          if (curr->children[index] == NULL) {
              curr->children[index] = new Node();
          }
          curr = curr->children[index];
      }
      curr->is_leaf = true;
  }

  bool Trie::remove(std::string const & outdated_query)
  {
      return remove(outdated_query, root, 0);
  }

  bool Trie::remove(std::string const & outdated_query, Node* curr, size_t curr_depth)
  {
      if (curr != NULL) {
          if (curr_depth == outdated_query.length()) {
              if (curr->is_leaf) {
                  curr->is_leaf = false;
                  for (int i = 0; i < ALPHABET_SIZE; i++) {
                      if (curr->children[i]) {
                          return false;
                      }
                  }
                  return true;
              }
          } else {
              int index = (int)outdated_query[curr_depth] - ASCII_START_CODE;
              if (remove(outdated_query, curr->children[index], curr_depth + 1)) {
                  delete curr->children[index];
                  curr->children[index] = NULL;
                  for (int i = 0; i < ALPHABET_SIZE; i++) {
                      if (curr->children[i]) {
                          return false;
                      }
                  }
                  return !curr->is_leaf;
              }
          }
      }
      return false;
  }

  std::vector<std::string> Trie::search(std::string const & prefix) // expect RVO, so while getting value in caller function, don't use const reference
  {
      Node* curr = root;
      std::vector<std::string> results;

      for (size_t i = 0; i < prefix.length(); i++) {
          int index = (int)prefix[i] - ASCII_START_CODE;
          if (curr->children[index] == NULL) {
              return results;
          }
          curr = curr->children[index];
      }

      std::string copied_prefix = prefix;
      get_all_children(curr, copied_prefix, results);
      return results;
  }

  void Trie::get_all_children(Node* curr_root, std::string & prefix, std::vector<std::string> & results)
  {
      if (results.size() > MAX_RESULTS || curr_root == NULL) {
          return;
      }
      if (curr_root->is_leaf) {
          results.push_back(prefix);
      }
      for (int i = 0; i < ALPHABET_SIZE; i++) {
          if (curr_root->children[i]) {
              char next = i + ASCII_START_CODE;
              prefix.push_back(next);
              get_all_children(curr_root->children[i], prefix, results);
              prefix.pop_back();
          }
      }
  }

  void Autocomplete::submit_new_query(std::string const & query)
  {
      if (submitted_queries.size() >= MAX_QUERIES) {
          trie.remove(submitted_queries.front());
          submitted_queries.pop();
      }
      trie.insert(query);
      submitted_queries.push(query);
  }

  std::vector<std::string> Autocomplete::search_for(std::string const & prefix_query)
  {
      return trie.search(prefix_query);
  }

}
