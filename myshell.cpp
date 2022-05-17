#include <unistd.h>

#include <filesystem>
#include <iostream>
#include <iterator>
#include <regex>
#include <string>
#include <string_view>
#include <vector>

namespace fs = std::filesystem;

// globals
constexpr int numOfSupportedCommands = 2;
std::array< std::string, numOfSupportedCommands > supportedCommands { "cd", "exit" };

// functions
void parseInput(std::vector< std::string >& tokenizedInput, fs::path& cwd);
void cd(std::vector< std::string >& tokenizedInput, fs::path& cwd);

std::vector< std::string > tokenize(std::string_view input);

int main() {
   fs::path cwDir = fs::current_path();
   std::string username { getlogin() };
   std::string input {};
   std::vector< std::string > tokenizedInput;

   std::cout << "================================\n"
                "== WELCOME TO VAROS TERMINAL! ==\n" <<
                "== Supported commands are:    ==\n" <<
                "== 1)cd:   change directory   ==\n" <<
                "== 2)exit: exit terminal      ==\n" <<
                "================================"   <<
                std::endl;
   // main loop
   while (true) {
      std::cout << username << ":" << cwDir.string() << "$ ";
      std::getline(std::cin, input);

      tokenizedInput = tokenize(input);
      parseInput(tokenizedInput, cwDir);
      input.clear();
      tokenizedInput.clear();
   }
   return 0;
}

void
parseInput(std::vector< std::string >& tokenizedInput, fs::path& cwd) {
   auto command { tokenizedInput.front() };
   
   if (command == "cd") {
      cd(tokenizedInput, cwd);
   } else if (command == "exit") {
      std::exit(EXIT_SUCCESS);
   }
}

void
cd(std::vector< std::string >& tokenizedInput, fs::path& cwd) {
   if (tokenizedInput.size() == 2) {
      cwd.assign(tokenizedInput.at(1));
   } else {
      std::cout << "cd: Too many arguments!" << std::endl;
   }
}

std::vector< std::string >
tokenize(std::string_view input) {
   std::vector< std::string > tokens;
   
   // Regular expression to find either words (i.e. "cd), or entire paths
   std::regex words { "[/?\\w+]+", std::regex_constants::ECMAScript };

   auto it =
    std::regex_iterator< std::string_view::iterator >(input.cbegin(), input.cend(), words);

   for (decltype(it) last; it != last; ++it) {
      tokens.push_back((*it).str());
   }
   return tokens;
}
