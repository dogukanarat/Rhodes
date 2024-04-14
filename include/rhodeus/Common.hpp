#ifndef INCLUDED_RHODEUS_COMMON_HPP
#define INCLUDED_RHODEUS_COMMON_HPP

#include <errno.h>
#include <stdint.h>

#include <string>
#include <format>

#include <fmt/format.h>
#include <nlohmann/json.hpp>
#include <plog/Log.h>

#define COMMON_COLOR_RED      "\033[31m"
#define COMMON_COLOR_GREEN    "\033[32m"
#define COMMON_COLOR_YELLOW   "\033[33m"
#define COMMON_COLOR_PURPLE   "\033[35m"
#define COMMON_COLOR_CYAN     "\033[36m"
#define COMMON_COLOR_CLR      "\033[0m"

#define CERR_LOG_RED          "\033[37;41m"
#define CERR_LOG_CLR          "\033[0m"

#define PACKED __attribute__((packed))

#define UNUSED(x)            (void)(x)
#define OK                   (0u)
#define TRUE                 (0xFFu)
#define FALSE                (0x00u)

#define EFAILED              (333u)

#define ERRNO(errno)         (status_t)(-errno)

#define CONSOLE_LOG(message) std::cout << "\r" << message << std::endl
#define CONSOLE_ERR(message) std::cerr << CERR_LOG_RED << "\r" << message << CERR_LOG_CLR <<  std::endl << std::flush

typedef int32_t status_t;
typedef uint8_t bool_t;

namespace json = nlohmann;

#endif
