//
// Created by oldlonecoder on 2022-06-22.
//

#pragma once

#include <TuxLS/Core/DllConfig.h>
#include <iostream>
#include <cstring>
#include <sstream>
#include <iomanip>
#include <vector>
#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <ctime>
#include <cstdint>
#include <cmath>
#include <functional>
#include <string>
#include <string_view>

namespace Tux
{
using std::string;
using std::string_view;
using std::vector;

class  CORE_LIB CString
{
    string_view                         _mData;///< Private/Encapsulated std::string instance.
    [[maybe_unused]] static string_view _mDefaultTokenSeparators;
public:
    using Iterator = string_view::iterator;
    using List     = std::vector<string_view>;



    CString() = default;
    explicit CString(string_view);
    explicit CString(char* aStr);
    explicit CString(string_view, string_view Delim, bool KeepDelim);
    explicit CString(char* aStr, string_view Delim, bool KeepDelim);
    explicit CString(const std::string& aStr, string_view Delim, bool KeepDelim);

    CString(const char* aStr);
    explicit CString(std::string aStr);
    //CString(string&&);
    ~CString() = default;

    struct CORE_LIB Token
    {
        string_view::iterator mStart;
        string_view::iterator mEnd;
        string_view::iterator mSE;

        string operator()() const;
        string operator*() const;

        using List = std::vector<CString::Token>;
        using Iterator = CString::Token::List::iterator;
        [[maybe_unused]] [[nodiscard]] string Mark() const;

        int         mLine     = 1;
        int         mCol      = 1;
        std::size_t mPosition = 0;

        std::string Location();
    };
    struct CORE_LIB Config
    {
        CString::Token::List Collection;
        string_view Delim;
        bool Keep;
        size_t z;
    };


    std::size_t Tokenize(CString::Token::List &Collection, string_view aDelimiters = "", bool KeepAsWord = true) const;
    CString& operator>>(CString::Token::List& Collection);

    CString::Iterator ScanTo(CString::Iterator aStart, char c) const;

    struct  SPS
    {
        string_view::iterator mStart;
        string_view::iterator mPos;
        string_view::iterator mStop; /// ...

        int      mLine  = 1;
        int      mCol   = 1;
        uint64_t mIndex = 0;

        SPS() = default;
        ~SPS() = default;

        explicit SPS(string_view aStr);
        bool Skip();
        [[nodiscard]] bool End() const;
        bool operator++();
        bool operator++(int);
        void Reset(string_view aStr)
        {
            mPos   = mStart = aStr.begin();
            mLine  = mCol   = 1;
            mIndex = 0;
            mStop  = aStr.end();
        }
        SPS &operator>>(CString::Token &aWord);
    } _mCursor;

    string_view operator()(){ return _mData.data(); }
    Config mConfig;
    CString& operator=(string_view aStr);
    CString& operator=(const char* aStr);
};

} // Lsc

