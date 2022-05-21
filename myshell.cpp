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
   HELP,
   LS,
   CD,
   HISTORY,
   EXIT
};

std::unordered_map< std::string, ECommand> supportedCommands {
   { "help",    ECommand::HELP    },
   { "ls",      ECommand::LS      },
   { "cd",      ECommand::CD      },
   { "history", ECommand::HISTORY },
   { "exit",    ECommand::EXIT    }
};

std::vector< std::string > hist;

// functions
void
parseInput(std::vector< std::string >& tokenizedInput, fs::path& cwd);

void
ls(std::vector< std::string >& tokenizedInput);

void
cd(std::vector< std::string >& tokenizedInput, fs::path& cwd);

void
history();

void help();

std::vector< std::string >
tokenize(std::string_view input);

int
main() {
   fs::path cwDir = fs::current_path();
   std::string username { getlogin() };
   std::string input {};
   std::vector< std::string > tokenizedInput;

   std::cout << "==========================================\n"
                "== WELCOME TO VAROS TERMINAL!           ==\n" <<
                "== Type \"help\" for supported commands   ==\n" <<
                "=========================================="   <<
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
         case ECommand::HELP:
            help();
            break;
         case ECommand::LS:
            ls(tokenizedInput);
            break;
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
ls(std::vector< std::string >& tokenizedInput) {
   if (tokenizedInput.size() == 2) {
      auto dir { tokenizedInput.at(1) };
      try {
         for (auto const& dirEntry : std::filesystem::directory_iterator{ dir }) {
            std::cout << dirEntry << std::endl;
         }
      } catch (std::exception e) {
         std::cout << "ls: Directory " << dir << " does not exist" << std::endl;
      }
   } else if (tokenizedInput.size() < 2) {
      std::cout << "ls: Too few arguments" << std::endl;
   } else {
      std::cout << "ls: Too many arguments" << std::endl;
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

void
help() {
   for (auto& command : supportedCommands) {
      std::cout << command.first << std::endl;
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
