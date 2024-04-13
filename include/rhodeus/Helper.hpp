#ifndef INCLUDED_RHODEUS_HELPER_HPP
#define INCLUDED_RHODEUS_HELPER_HPP

#include <rhodeus/Common.hpp>
#include <vector>
#include <string>

namespace Rhodeus
{
    class Helper
    {
        public:
            static std::vector<std::string> split(const std::string& str, char delimiter);

    };
}

#endif /* INCLUDED_RHODEUS_HELPER_HPP */
