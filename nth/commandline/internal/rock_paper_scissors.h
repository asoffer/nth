#ifndef NTH_COMMANDLINE_INTERNAL_ROCK_PAPER_SCISSORS_H
#define NTH_COMMANDLINE_INTERNAL_ROCK_PAPER_SCISSORS_H

#include <string_view>

namespace nth::internal_commandline {

enum class Throw { Rock, Paper, Scissors };
template <typename ParserErrorReporter>
bool NthCommandlineParse(std::string_view s, Throw &t,
                         ParserErrorReporter reporter) {
  if (s == "rock") {
    t = Throw::Rock;
    return true;
  } else if (s == "paper") {
    t = Throw::Paper;
    return true;
  } else if (s == "scissors") {
    t = Throw::Scissors;
    return true;
  } else {
    reporter("Argument must be one of 'rock', 'paper', or 'scissors'.");
    return false;
  }
}

}  // namespace nth::internal_commandline

#endif  // NTH_COMMANDLINE_INTERNAL_ROCK_PAPER_SCISSORS_H
