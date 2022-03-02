// (c) Facebook, Inc. and its affiliates. Confidential and proprietary.

#pragma once

#if !defined(__FB_COMMAND_LINE_PARSER_H)
#define __FB_COMMAND_LINE_PARSER_H

#include <motion_search/inc/xplatform.h>

#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

namespace facebook {

class CommandLineParser {
 public:
  // constructor
  CommandLineParser(const size_t argc, const char* const argv[]) {
    for (size_t i = 1; i < argc; ++i) {
      // a parameter
      if ('-' == argv[i][0]) {
        std::string arg(argv[i]);

        if (1 == arg.size()) {
          continue;
        }

        const size_t eqMarkPos = arg.find_first_of("=", 1);

        // a flag
        if (std::string::npos == eqMarkPos) {
          m_params[arg.substr(1, eqMarkPos)] = std::string();
        }
        // value parameter
        else if (1 < eqMarkPos) {
          m_params[arg.substr(1, eqMarkPos - 1)] = arg.substr(eqMarkPos + 1);
        } else {
          std::cout << "invalid parameter <" << argv[i] << ">" << std::endl;
        }
      }
      // a file name
      else {
        m_fileNames.push_back(std::string(argv[i]));
      }
    }
  }

  // destructor
  virtual ~CommandLineParser(void){};

  // is a given parameter passed
  bool Find(const char* const pName) const {
    auto iter = m_params.find(pName);

    return ((m_params.end() == iter) ? (false) : (true));
  }

  // get a parameter's value
  template <typename type_t = const std::string>
  inline type_t Get(const char* const pName) const;
  template <typename type_t>
  inline type_t Get(const char* const pName, const type_t def) const;

  // working with provided file names/pathes
  inline size_t NumFileNames(void) const {
    return m_fileNames.size();
  }
  inline const char* GetFileName(const size_t idx) const {
    if (NumFileNames() <= idx) {
      OnError("invalid file index");
    }

    return m_fileNames[idx].c_str();
  }

 private:
  void OnError(const char* pError) const {
    throw std::string(pError);
  }

  // passed parameters
  std::unordered_map<std::string, std::string> m_params;
  std::vector<std::string> m_fileNames;
};

template <>
inline const std::string CommandLineParser::Get<const std::string>(
    const char* const pName) const {
  const auto& iter = m_params.find(pName);

  if (m_params.end() == iter) {
    return std::string();
  }

  return (*iter).second;
}

template <>
inline int32_t CommandLineParser::Get<int32_t>(const char* const pName) const {
  const auto value = Get<>(pName);

  if (value.empty()) {
    OnError("unknown or invalid parameter");
  }

  return std::atoi(value.c_str());
}
template <>
inline uint32_t CommandLineParser::Get<uint32_t>(
    const char* const pName) const {
  return (uint32_t) Get<int32_t>(pName);
}

template <>
inline int32_t CommandLineParser::Get<int32_t>(
    const char* const pName,
    const int32_t def) const {
  const auto value = Get<>(pName);

  if (value.empty()) {
    return def;
  }

  return std::atoi(value.c_str());
}
template <>
inline uint32_t CommandLineParser::Get<uint32_t>(
    const char* const pName,
    const uint32_t def) const {
  return (uint32_t) Get<int32_t>(pName, (int32_t) def);
}

template <>
inline double CommandLineParser::Get<double>(
    const char* const pName,
    const double def) const {
  const auto value = Get<>(pName);

  if (value.empty()) {
    return def;
  }

  return std::atof(value.c_str());
}

} // namespace facebook

#endif // !defined(__FB_COMMAND_LINE_PARSER_H)
