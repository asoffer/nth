#include "nth/commandline/internal/parse.h"

#include "nth/debug/log/log.h"

namespace nth::internal_commandline {

Flag const *FlagParsingState::find(char c) {
  for (auto iter = valid_flags_.rbegin(); iter != valid_flags_.rend(); ++iter) {
    auto &v = **iter;
    for (Flag const &f : v) {
      if (f.name.short_name() == c) { return &f; }
    }
  }
  return nullptr;
}

Flag const *FlagParsingState::find(std::string_view name) {
  for (auto iter = valid_flags_.rbegin(); iter != valid_flags_.rend(); ++iter) {
    auto &v = **iter;
    for (Flag const &f : v) {
      if (f.name.full_name() == name) { return &f; }
    }
  }
  return nullptr;
}

bool FlagParsingState::parse(char short_flag) {
  if (Flag const *flag = find(short_flag)) {
    if (flag->type.type_ != nth::type<void>) {
      NTH_LOG("'-{}' accepts and argument of type '{}'.") <<=
          {short_flag, flag->type.type_};
      return false;
    }
    flags_.insert(Flag::Value(*flag));
    return true;
  } else {
    NTH_LOG("'-{}' is not valid flag.") <<= {short_flag};
    return false;
  }
}

bool FlagParsingState::parse(size_t position, std::string_view argument) {
  if (size_t eq_pos = argument.find('='); eq_pos == std::string_view::npos) {
    if (Flag const *flag = find(argument)) {
      if (flag->type.type_ != nth::type<void>) {
        NTH_LOG("'--{}' accepts and argument of type '{}'.") <<=
            {argument, flag->type.type_};

        return false;
      }
      flags_.insert(Flag::Value(*flag));
    } else {
      NTH_LOG("'--{}' is not valid flag name.") <<= {argument};
      return false;
    }
  } else {
    if (eq_pos == 2) {
      NTH_LOG(
          "Argument provided in position {} has no name. All "
          "arguments must be named with a non-zero number of "
          "characters.") <<= {position};
      return false;
    } else {
      std::string_view argument_name  = argument.substr(0, eq_pos);
      std::string_view argument_value = argument.substr(eq_pos + 1);
      if (Flag const *flag = find(argument_name)) {
        flags_.insert(Flag::Value(*flag, argument_value));
      } else {
        NTH_LOG("'--{}' is not valid flag name.") <<= {argument_name};
        return false;
      }
    }
  }
  return true;
}

bool FlagParsingState::ParseFlag(size_t position, std::string_view argument) {
  switch (argument.size()) {
    case 0: {
      return false;
    } break;
    case 1: {
      NTH_LOG(
          "Argument '{}' found at position {} is not valid. Did you "
          "mean to type a short-flag?") <<= {argument, position};
      return false;
    } break;
    case 2: {
      if (argument[1] == '-') {
        NTH_LOG("Argument '{}' found at position {} is not valid.") <<=
            {argument, position};
        return false;
      } else {
        return parse(argument[1]);
      }
    } break;
    default:
      if (argument[1] == '-') {
        return parse(position, argument.substr(2));
      } else {
        size_t eq_pos = argument.find('=');
        if (eq_pos == std::string_view::npos) {
          bool ok = true;
          for (char c : argument.substr(1)) { ok &= parse(c); }
          return ok;
        } else {
          if (eq_pos == 2) {
            if (Flag const *arg = find(argument[1])) {
              NTH_LOG(
                  "Argument '{}' found at position {} is not valid. Short "
                  "arguments may not be assigned values. Use the full name "
                  "(--{}) if this is intended.") <<=
                  {argument, position, arg->name.full_name()};
            } else {
              NTH_LOG(
                  "Argument '{}' found at position {} is not valid. Short "
                  "arguments may not be assigned values. Use the full name "
                  "if this is intended.") <<= {argument, position};
            }
          } else {
            NTH_LOG(
                "Arguments '{}' found at position {} are not valid. Short "
                "arguments may not be assigned values. Use their full "
                "names if this is intended.") <<= {argument, position};
          }
          return false;
        }
      }
  }
  // TODO: Handle flag repetitions.
}

}  // namespace nth::internal_commandline
