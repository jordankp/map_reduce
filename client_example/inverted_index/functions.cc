#include <string>
#include <utility>
#include <vector>
#include <fstream>
#include <cstdlib>
#include <unordered_set>

using KeyIn = std::string; // document ID
using ValueIn = void *; // document contents
using KeyOut = std::string; // word
using ValueOut = void *; // list of document IDs

#define NUM_FILES 30

extern "C"
{

std::vector<std::pair<KeyIn, ValueIn>> read()
{
  std::vector<std::pair<KeyIn, ValueIn>> result;

  for (size_t i = 0; i < NUM_FILES; ++i)
  {
    std::string file_name = std::string(INPUT_DIR) + "/doc" + std::to_string(i) + ".txt";
    char *doc_contents = nullptr;

    std::ifstream f_in(file_name);
    f_in.seekg(0, std::ios::end);
    size_t length = f_in.tellg();
    f_in.seekg(0, std::ios::beg);

    doc_contents = new char[length + 1];
    f_in.read(doc_contents, length);
    doc_contents[length] = '\0';

    result.push_back(std::make_pair(std::to_string(i), (void *)doc_contents));
  }

  return result;
}

void write(const std::vector<std::pair<KeyOut, ValueOut>> &vec)
{
  std::ofstream f_out(std::string(OUTPUT_DIR) + "/output.txt");

  for (auto &pair : vec)
  {
    f_out << pair.first << ": ";
    std::unordered_set<size_t> *doc_ids = (std::unordered_set<size_t> *)pair.second;

    for (size_t id : *doc_ids)
      f_out << id << " ";

    f_out << "\n";

    delete doc_ids;
  }
}

std::vector<std::pair<KeyOut, ValueOut>> map(KeyIn doc_id, ValueIn doc_contents)
{
  std::vector<std::pair<KeyOut, ValueOut>> result;
  char *iter = (char *)doc_contents;

  while (*iter)
  {
    while (*iter == ' ' || *iter == '\t' || *iter == '\n')
      ++iter;

    if (*iter)
    {
      size_t word_length = 0;
      char *word_begin = iter;

      while (*iter && *iter != ' ' && *iter != '\t' && *iter != '\n')
      {
        ++iter;
        ++word_length;
      }

      result.push_back(std::make_pair(std::string(word_begin, word_length), (void *)stol(doc_id)));
    }
  }

  delete[] (char *)doc_contents;

  return result;
}

std::pair<KeyOut, ValueOut> reduce(KeyOut word, const std::vector<ValueOut> &doc_ids)
{
  std::unordered_set<size_t> *reduced_doc_ids = new std::unordered_set<size_t>();

  for (size_t i = 0; i < doc_ids.size(); ++i)
    reduced_doc_ids->insert((size_t)doc_ids[i]);

  return std::make_pair(word, (void *)reduced_doc_ids);
}

}
