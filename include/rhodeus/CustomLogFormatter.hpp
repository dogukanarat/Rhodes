#ifndef INCLUDED_RHODEUS_LOGFORMATTER_HPP
#define INCLUDED_RHODEUS_LOGFORMATTER_HPP
#include <plog/Record.h>
#include <plog/Util.h>
#include <iomanip>

namespace Rhodeus
{
    template<bool useTime, bool useUtcTime, bool userFunctionName, bool useFileName, bool useLineNumber, bool useTid, bool userLogLevel>
    class CustomLogFormatterImpl
    {
    public:
        static plog::util::nstring header()
        {
            return plog::util::nstring();
        }

        static plog::util::nstring format(const plog::Record& record)
        {
            tm t;
            plog::util::nostringstream ss;

            if (useTime)
            {
                useUtcTime ? plog::util::gmtime_s(&t, &record.getTime().time) : plog::util::localtime_s(&t, &record.getTime().time);
                ss << t.tm_year + 1900 << "-" << std::setfill(PLOG_NSTR('0')) << std::setw(2) << t.tm_mon + 1 << PLOG_NSTR("-") << std::setfill(PLOG_NSTR('0')) << std::setw(2) << t.tm_mday << PLOG_NSTR(" ");
                ss << std::setfill(PLOG_NSTR('0')) << std::setw(2) << t.tm_hour << PLOG_NSTR(":") << std::setfill(PLOG_NSTR('0')) << std::setw(2) << t.tm_min << PLOG_NSTR(":") << std::setfill(PLOG_NSTR('0')) << std::setw(2) << t.tm_sec << PLOG_NSTR(".") << std::setfill(PLOG_NSTR('0')) << std::setw(3) << static_cast<int> (record.getTime().millitm) << PLOG_NSTR(" ");
            }
            if (userLogLevel)
            {
                ss << PLOG_NSTR("[") << record.getSeverity() << PLOG_NSTR("]") << PLOG_NSTR(" ");
            }
            if (useTid)
            {
                ss << PLOG_NSTR("[") << record.getTid() << PLOG_NSTR("] ");
            }
            if (userFunctionName)
            {
                ss << PLOG_NSTR("[") << record.getFunc() << PLOG_NSTR("] ");
            }
            if (useFileName)
            {
                ss << PLOG_NSTR("[");
                ss << record.getFile();
                if (useLineNumber)
                {
                    ss << PLOG_NSTR("@") << record.getLine();
                }
                ss << PLOG_NSTR("] ");
            }
            ss << record.getMessage() << PLOG_NSTR("\n");

            return ss.str();
        }
    };

    class CustomLogFormatterFile : public CustomLogFormatterImpl<true, true, true, true, true, true, true> {};
    class CustomLogFormatterConsole : public CustomLogFormatterImpl<false, false, true, false, false, false, false> {};
}

#endif // INCLUDED_RHODEUS_LOGFORMATTER_HPP
