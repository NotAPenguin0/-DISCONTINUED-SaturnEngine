#ifndef MVG_LOG_HPP_
#define MVG_LOG_HPP_

#include "depend/log++.h"
#include <cassert>
#include <string>

namespace mvg {

inline void log(std::string const& msg) { logpp::Console::log(msg); }

inline void error(std::string const& msg) { logpp::Console::error(msg); }

inline void warning(std::string const& msg) { logpp::Console::warning(msg); }

inline void info(std::string const& msg) { logpp::Console::log(msg); }

inline void debug(std::string const& msg) { logpp::Console::debug(msg); }

} // namespace mvg

#endif