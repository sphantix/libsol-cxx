/*
 * File Name: strconv.h
 * Author: Sphantix
 * Mail: sphantix@gmail.com
 * Created Time: Thu 03 Dec 2015 08:39:34 AM CST
 */

#ifndef __STRCONV_H__
#define __STRCONV_H__

#include <string>
#include <cctype>
#include <cstddef>

namespace sol
{
    namespace string
    {
        inline std::string upper(const std::string& s)
        {
            std::string upper(s);

            for (std::size_t i = 0; i < s.length(); ++i)
                upper[i] = std::toupper(upper[i]);

            return upper;
        }

        inline std::string lower(const std::string& s)
        {
            std::string lower(s);

            for (std::size_t i = 0; i < s.length(); ++i)
                lower[i] = std::tolower(lower[i]);

            return lower;
        }

        inline std::string trim(const std::string& s)
        {
            if (s.length() == 0)
                return s;

            std::size_t beg = s.find_first_not_of("\a\b\f\n\r\t\v");
            std::size_t end = s.find_last_not_of("\a\b\f\n\r\t\v");

            if (beg == std::string::npos)
                return "";

            return std::string(s, beg, end-beg+1);
        }

        inline std::string& replace_all(std::string& context, const std::string& from, const std::string& to)
        {
            std::size_t here = 0;
            std::size_t found;

            while ((found = context.find(from, here) != std::string::npos))
            {
                context.replace(found, from.size(), to);
                here = found + to.size();
            }

            return context;
        }
    }
}

#endif /* __STRCONV_H__ */
