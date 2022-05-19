#include <unistd.h>

#include <filesystem>
#include <iostream>
#include <iterator>
#include <regex>
#include <string>
#include <string_view>
#include <vector>
#include <unordered_map>

namespace fs = std::filesystem;

// globals
enum class ECommand
{
   CD,
   HISTORY,
   EXIT
};

std::unordered_map< std::string, ECommand> supportedCommands {
   { "cd",      ECommand::CD },
   { "history", ECommand::HISTORY },
   { "exit",    ECommand::EXIT }
};

std::vector< std::string > hist;

// functions
void
parseInput(std::vector< std::string >& tokenizedInput, fs::path& cwd);

void
cd(std::vector< std::string >& tokenizedInput, fs::path& cwd);

void
history();

std::vector< std::string >
tokenize(std::string_view input);

int
main() {
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

      // tokenize the user's input
      tokenizedInput = tokenize(input);

      // parse the input to select which command will be executed
      parseInput(tokenizedInput, cwDir);
      hist.push_back(input);
      input.clear();
      tokenizedInput.clear();
   }
   std::exit(EXIT_SUCCESS);
}

void
parseInput(std::vector< std::string >& tokenizedInput, fs::path& cwd) {
   auto it { supportedCommands.find(tokenizedInput.front()) };
  
   if (it != supportedCommands.end()) {
      switch (it->second) {
         case ECommand::CD:
            cd(tokenizedInput, cwd);
            break;
         case ECommand::HISTORY:
            history();
            break;
         case ECommand::EXIT:
            std::exit(EXIT_SUCCESS);
         break;
      }
   } else {
      std::cout << tokenizedInput.front() << ": Unknown command" << std::endl;
   }
}

void
cd(std::vector< std::string >& tokenizedInput, fs::path& cwd) {
   if (tokenizedInput.size() == 2) {
      fs::path destination { tokenizedInput.at(1) };
      if (fs::exists(destination)) {
         cwd.assign(destination);
      } else {
         std::cout << "Invalid path: directory " << destination.c_str() << " does not exist" << std::endl;
      }
   } else {
      std::cout << "cd: Too many arguments!" << std::endl;
   }
}

void
history() {
   for (auto& command : hist) {
      std::cout << command << std::endl;
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
