#include <argh.h>

#include <cstdlib>
#include <fstream>
#include <initializer_list>
#include <iomanip>
#include <iostream>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>
#include <chrono>

int main(int argc, char** argv) {
  using namespace std::literals::string_literals;
  // goal: take an input list of strings
  // and an english dictionary list of words
  // output: list of acronyms that form valid english words
  // settings: allow letter/word repetition

  /* Parse Command Line */
  argh::parser cmdl;

  // input filepath param (input.txt)
  cmdl.add_param({"-i"});
  // output filepath param (output.txt)
  cmdl.add_param({"-o"});
  // dictionary param (optional)
  cmdl.add_param({"-d"});
  // allow repetition (optional)
  cmdl.add_param({"-r"});

  cmdl.parse(argc, argv);

  // path to input txt file
  std::string inputFilepath;
  if (!(cmdl({"-i"}) >> inputFilepath)) {
    std::cerr << "Please specify an input filepath with -i" << std::endl;
    return EXIT_FAILURE;
  }

  // path to output txt file
  std::string outputFilepath;
  if (!(cmdl({"-o"}) >> outputFilepath)) {
    std::cerr << "Please specify an output filepath with -o" << std::endl;
    return EXIT_FAILURE;
  }

  // path to dictionary
  std::string dictionaryFilepath;
  if (!(cmdl({"-d"}) >> dictionaryFilepath)) {
    std::cerr << "Please specify a dictionary filepath with -d" << std::endl;
    return EXIT_FAILURE;
  }

  /* Parse The Dictionary (if enabled) */
  std::vector<std::string> dictionaryWords;
  if (!dictionaryFilepath.empty()) {
    // each line is a unique word
    std::ifstream file(dictionaryFilepath);
    if (!file) {
      std::cerr << "Could not open " << std::quoted(dictionaryFilepath) << std::endl;
      return EXIT_FAILURE;
    }

    std::string line;
    while (std::getline(file, line, '\n')) {
      dictionaryWords.emplace_back(std::move(line));
    }

    std::cout << "Read dictionary " << std::quoted(dictionaryFilepath);
    std::cout << ", got " << dictionaryWords.size() << " words" << std::endl;
  }

  // allow repetition of input words
  const bool allowRepetition = cmdl["-r"];

  /* Parse Input File */
  std::vector<std::string> inputWords;
  {
    // each line is a unique word
    std::ifstream file(inputFilepath);
    if (!file) {
      std::cerr << "Could not open " << std::quoted(inputFilepath) << std::endl;
      return EXIT_FAILURE;
    }

    std::string line;
    while (std::getline(file, line, '\n')) {
      inputWords.emplace_back(std::move(line));
    }

    std::cout << "Read input " << std::quoted(inputFilepath);
    std::cout << ", got " << inputWords.size() << " words" << std::endl;
  }

  // get the first lower-case character of each word
  std::vector<char> inputChars;
  {
    for (const auto& inputWord : inputWords) {
      if (!inputWord.empty()) {
        inputChars.push_back(inputWord.front());
      }
    }

    std::sort(inputChars.begin(), inputChars.end());
    if (!allowRepetition) {
      auto it = std::unique(inputChars.begin(), inputChars.end());
      inputChars.erase(it, inputChars.end());
      std::cout << "without repetition: " << inputChars.size();
      std::cout << " words" << std::endl;
    } else {
      std::cout << "with repetition: " << inputChars.size();
      std::cout << " words" << std::endl;
    }
  }

  // Now make every combination of letters here:
  std::vector<std::string> combos;
  {
    const auto start = std::chrono::steady_clock::now();
    
    void generate(const std::string& root, const int level, const std::vector<char>& inputs,
                  std::vector<std::string>& output);
  
    std::string root;
    int level = inputChars.size();
    generate(root, level - 1, inputChars, combos);

    const auto end = std::chrono::steady_clock::now();
    const auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    std::cout << "Combination elapsed: " << elapsed << std::endl;

    std::cout << "generated " << combos.size() << " words" << std::endl;
  }

  /* Filter the dictionary */
  {
    const auto start = std::chrono::steady_clock::now();

    // remove words that are longer or shorter than the combo list
    auto it = std::remove_if(dictionaryWords.begin(), dictionaryWords.end(), [&inputChars](const std::string& val) -> bool {
      return val.size() != inputChars.size();
    });

    dictionaryWords.erase(it, dictionaryWords.end());

    const auto end = std::chrono::steady_clock::now();
    const auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    std::cout << "Filter elapsed: " << elapsed << std::endl;

    std::cout << "Narrowed search space to " << dictionaryWords.size() << std::endl;
  }

  // for each word: compute the hash
  // using StringHash = std::hash<std::string>;
  // using HashResult = decltype(std::declval<StringHash>().operator()(std::declval<std::string>()));
  std::vector<std::size_t> dictionaryHash;
  std::vector<std::size_t> combinationHash;
  {
    const auto start = std::chrono::steady_clock::now();

    for (const auto& string : dictionaryWords) {
      const std::size_t hash = std::hash<std::string>{}(string);
      dictionaryHash.push_back(hash);
    }
    
    for (const auto& string : combos) {
      const std::size_t hash = std::hash<std::string>{}(string);
      combinationHash.push_back(hash);
    }

    const auto end = std::chrono::steady_clock::now();
    const auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    std::cout << "Hash elapsed: " << elapsed << std::endl;
  }

  // compare everything
  {
    const auto start = std::chrono::steady_clock::now();
    
    std::vector<std::size_t> finalists;
    for (int i{}; i < dictionaryHash.size(); i++) {
      const std::size_t word = dictionaryHash.at(i);
      for (int j{}; j < combinationHash.size(); j++) {
        const std::size_t combo = combinationHash.at(j);
        // if any of the words match
        if (word == combo) {
          std::cout << "word: " << combos.at(j) << std::endl;
        }
      }
    }
    
    const auto end = std::chrono::steady_clock::now();
    const auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    std::cout << "Comparison elapsed: " << elapsed << std::endl;
  }

  return 0;
}

void generate(const std::string& root, const int level, const std::vector<char>& inputs,
              std::vector<std::string>& output) {
  for (int i{}; i < inputs.size(); i++) {
    char c = inputs.at(i);
    std::string next = root + c;
    if (level != 0) {
      generate(next, level - 1, inputs, output);
    } else {
      output.push_back(std::move(next));
    }
  }
}