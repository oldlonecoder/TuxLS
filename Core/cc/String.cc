
//
// Created by oldlonecoder on 21-11-20.
//



#ifdef _WIN32
#include <Winsock2.h>
#else
#include <arpa/inet.h>
#endif


#include <TuxLS/Core/String.h>


namespace Tux
{
using std::string;

string  String::_msNull = "";

std::ostream& operator<<(std::ostream& in, const String& _s)
{
    in << _s();
    return in;
}

String::String()
{
}

String::String(const char* a_str)
{
    //SOut << __PRETTY_FUNCTION__ << ":[\"" << a_str << "\"]:\n";
    _mStr = a_str;
}

String::String(std::string&& a_str)
{
    std::swap(_mStr, a_str);
    a_str.clear();
}

String::String(const string& a_str)
{
    //ffnl << a_str << "\n";
    _mStr = a_str;
}

String::String(const String& Str)
{
    //ffnl << Str._str << "\n";
    _mStr = Str._mStr;
}

String::String(String&& Str) noexcept
{
    //ffnl << Str._str << "\n";
    std::swap(_mStr, Str._mStr);
}

String::~String()
{
    //ffnl << _str << "\n";
    _mStr.clear();
}



//String& String::operator=(const String& a_str)
//{
//    //ffnl << a_str._str << "\n";
//    _D = a_str._D;
//    _ArgPosition = a_str._ArgPosition;
//    _Precision = a_str._Precision;
//    return *this;
//}

String& String::operator=(String&& a_str) noexcept
{
    //ffnl << a_str._str << "\n";
    std::swap(_mStr, a_str._mStr);
    _mArgPos = 0;
    return *this;
}

bool String::operator==(const String& a_str) const
{
    return _mStr == a_str._mStr;
}

String& String::operator=(string&& a_str)
{
    //ffnl << a_str << "\n";
    swap(_mStr, a_str);
    _mArgPos = 0;
    return *this;
}

String& String::operator<<(char c)
{
    if (scan_arg() == std::string::npos)
    {
        _mStr += c;
        return *this;
    }
    return _Format(c);
}

String& String::operator=(const char* a_str)
{
    if (a_str)
        _mStr = a_str;
    else
        _mStr = "";
    _mArgPos = 0;
    return *this;
}

String& String::operator<<(const string& a_str)
{
    //_scan_next_Arg();
    if (scan_arg() == std::string::npos)
    {
        _mStr += a_str;
        return *this;
    }
    return _Format(a_str);
}

String::List String::ToList(int argc, char** argv)
{
    List  args;
    for (int x = 0; x < argc; x++)
    {
        args.push_back(argv[x]);

    }
    return args;
}

String& String::operator<<(const String& a_str)
{
    //_scan_next_Arg();
    if (scan_arg() == std::string::npos)
    {
        std::ostringstream os;
        os << a_str();
        _mStr += os.str();
        return *this;
    }
    return _Format(a_str());
}

String& String::operator<<(const char* a_str)
{
    //_scan_next_Arg();
    if (scan_arg() == std::string::npos)
    {
        std::ostringstream os;
        os.precision(_mPrecision);
        os << a_str;
        _mStr += os.str();
        return *this;
    }
    return _Format(a_str);
}

String& String::operator+=(const String& a_atr)
{
    _mStr += a_atr._mStr;
    return *this;
}

String& String::operator+=(const string& a_atr)
{
    _mStr += a_atr;
    return *this;
}

String& String::operator+=(char c)
{
    _mStr += c;
    return *this;
}

String& String::operator+(char c)
{
    _mStr += c;
    return *this;
}

String& String::operator+(const String& a_atr)
{
    _mStr += a_atr._mStr;
    return *this;
}

String& String::operator+(const string& a_atr)
{
    _mStr += a_atr;
    return *this;
}

String& String::operator>>(string& arg_)
{
    arg_ = _mStr;
    return *this;
}

std::string String::MakeStr(const char* B, const char* E)
{
    std::string Str;
    const char* C = B;
    if ((!B) || (!E) || (!*B) || (!*E))
        return Str;
    while (C <= E)
        Str += *C++;

    return Str;
}

void String::Clear()
{
    _mStr.clear();
    _mArgPos = (string::size_type)0;
}

std::string String::datetime(const std::string& str_fmt)
{
    time_t rawtime;
    struct tm* timeinfo;
    char tmb[180];
    std::time(&rawtime);
    timeinfo = std::localtime(&rawtime);
    std::strftime(tmb, 60, str_fmt.c_str(), timeinfo);
    std::string _s = tmb;
    return tmb;
}

string::size_type String::scan_arg()
{
    _mArgPos = _mStr.find("%", 0);
    return _mArgPos;
}

void String::put_arg(const string& aStr)
{
    if (scan_arg() == std::string::npos)
    {
        _mStr.append(aStr);
        return;
    }
    String::_Format<const std::string&>(aStr);
}

// Ce qui fait royalement chier avec les iterateurs des stl, m'est que depuis linenum'iterateur, comment on accede � son conteneur ???????
bool String::SkipWS(string::iterator& pos)
{
    if (pos == _mStr.end()) // aucun moyen de savoir si linenum'it�rateur en est un de notre conteneur "_str" !!
        return false;
    while (isspace(*pos))
        ++pos;
    return true;
}

// Ce qui fait royalement chier avec les iterateurs des stl, m'est que depuis linenum'iterateur, comment on accede � son conteneur ???????
bool String::SkipWS(const char* pos)
{
    if (!pos)
        return false;
    while (pos && (*pos && isspace(*pos)))
        ++pos;
    return true;
}


string::const_iterator String::ScanTo(string::const_iterator start, char c) const
{
    string::const_iterator p = start;
    ++p;
    while ((p != _mStr.end()) && (*p != c))
        ++p;
    return p;
}

const char* String::ScanTo(const char* start, char c) const
{
    const char* p = start;
    if (!p)
        return nullptr;
    ++p;
    while (p && (*p != c))
        ++p;
    return p;
}


int String::filter(const String::List& a_exp)
{
    if (!a_exp.size())
        return 0;
    auto i = a_exp.cbegin();

    // array::front is the init of the match at the beginning of this string.
    // if ...front() is Empty, then there is no required match at the beginning of this string
    // so proceed to the next substring to find.
    // if the last is not Empty, then match the last substring.
    std::string::size_type pos = 0;
    if (!(*i).empty())
    {
        pos = _mStr.find((*i));
        if (pos != 0) // no match
            return false;
    }
    else;
    ++i;
    auto end = a_exp.cend();
    --end;
    if (!(*i).empty())
    {
        while (i < end)
        {
            if ((*i).empty())
            {
                ++i;
                continue;
            }
            pos = _mStr.find((*i), pos);
            if (pos != std::string::npos)
            {
                ++pos;
                ++i;
                continue;
            }
            return false;
        }
    }
    if (!(*end).empty())
    {
        std::size_t sz = (*end).size();
        pos = _mStr.find((*end), pos);
        if (pos != std::string::npos)
        {
            if ((pos + sz) != _mStr.size())
                return false;
        }
    }
    return true;
}

std::string String::SizeF(uint64_t sz)
{
    float fsize = (float)sz;
    String  us = "";
    int   currentunit = 0;
    String  units[] = { "B", "kb", "mb", "gb", "tb" };
    int   unitssize = sizeof(units) / sizeof(units[0]);

    while (fsize > 1024.0 && currentunit < unitssize - 1)
    {
        fsize /= 1024.0;
        currentunit++;
    }

    us = std::fixed;
    us += std::setprecision(2);
    us += fsize;
    us += " ";
    us += units[currentunit];

    return us;
}
std::string String::Extract(const std::string& first_lhs, const std::string& first_rhs)
{
    std::size_t lhs_pos = _mStr.find(first_lhs);
    if (lhs_pos == std::string::npos)
        return "";
    std::size_t rhs_pos = _mStr.find(first_rhs);
    if (rhs_pos == std::string::npos)
        return "";

    return _mStr.substr(lhs_pos, rhs_pos - lhs_pos);
}

/*!
 * @brief Try to extract the "template" type from the \c func_desc that normally shall be given by __PRETTY_FUNCTION__ at the call Location.
 *
 * template parameter assumed to be "T" ...for now.
 *
 * @param func_desc
 *
 * @note This function has no control of the proper format and (function) prototype data.
 * @return the resulting type text
 */
std::string String::type_of(string&& func_desc)// , const std::string& _T) -> _T is the class to search.
{
    //    String               text = std::move(func_desc);
    //    String::word::List w;
    //
    //    std::size_t count = text.words(w);
    //    for(auto    s: w)
    //    {
    //        std::cout << s();
    //    }
    //    std::cout << '\n';
    return "c to implement, please\n";
}



String& String::operator<<(Color::Type c)
{

    if (scan_arg() == std::string::npos)
    {
        //std::cerr << __FUNCTION__ << ":\n";
        _mStr += Color::Ansi(c);
        return *this;
    }
    return _Format(Color::Ansi(c));
}


}
