#ifndef INCLUDED_RHODEUS_COMMON_HPP
#define INCLUDED_RHODEUS_COMMON_HPP

#include <fmt/format.h>
#include <plog/Log.h>
#include <boost/filesystem.hpp>
#include <nlohmann/json.hpp>

#define CERR_LOG_RED "\033[37;41m"
#define CERR_LOG_CLR "\033[0m"

namespace fs = boost::filesystem;
namespace json = nlohmann;

#endif
