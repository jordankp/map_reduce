#include <string>
#include <utility>
#include <vector>
#include <fstream>

using KeyIn = std::string;
using KeyOut = std::string;
using ValueIn = void *;
using ValueOut = void *;

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

    result.push_back(std::make_pair(file_name, (void *)doc_contents));
  }

  return result;
}

void write(const std::vector<std::pair<KeyOut, ValueOut>> &vec)
{
  std::ofstream f_out(std::string(OUTPUT_DIR) + "/output.txt");

  for (auto &pair : vec)
    f_out << pair.first << ":" << (size_t)(pair.second) << "\n";
}

std::vector<std::pair<KeyOut, ValueOut>> map(KeyIn, ValueIn doc_contents)
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

      result.push_back(std::make_pair(std::string(word_begin, word_length), (void *)(size_t)1));
    }
  }

  delete[] (char *)doc_contents;

  return result;
}

std::pair<KeyOut, ValueOut> reduce(KeyOut word, const std::vector<ValueOut> &word_counts)
{
  size_t count = 0;

  for (auto num : word_counts)
    count += (size_t)num;

  return std::make_pair(word, (void *)count);
}

}
