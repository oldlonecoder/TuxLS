//
// Created by oldlonecoder on 21-11-20.
//

#include <TuxLS/Core/Rem.h>
#include <TuxLS/Core/CString.h>

namespace Tux
{

int Rem::_mcIndent = 0;

Rem::List MessagesList;

std::vector<std::string_view> Rem::types_text =
{
    {"error"},
    {"warning"},
    {"information"},
    {"exception"},
    {"fatal"},
    {"status"},
    {"debug"},
    {"     "}, // Output
    {"comment"},
    {"syntax error"} };

std::vector<std::string_view> Rem::codes_text =
{
    {"hello"},
    {"ok"},
    {"rejected"},
    {"accepted"},
    {"end of file"},
    {"end of stream"},
    {"null pointer"},
    {"not empty"},
    {"empty"},
    {"unexpected"},
    {"expected"},
    {"\n"}, // dummy string because ...
    {"begin"},
    {"end"},
    {"implement"},
    {"exist"},
    {"not exist"}

};

std::vector<std::string> Rem::types_ansi256_attr;
std::vector<std::string> Rem::codes_ansi256_attr;

Rem::Rem(const Rem& r)
{
    Ic = r.Ic;
    _mText = r._mText;
    _mType = r._mType;
    _mCode = r._mCode;
    _mSrc = r._mSrc;
    _mComponents = r._mComponents;
}

Rem::Rem(Rem&& r) noexcept
{
    Ic = r.Ic;
    _mSrc = std::move(r._mSrc);
    _mText = std::move(r._mText);
    _mComponents = std::move(r._mComponents);
    _mType = r._mType;
    _mCode = r._mCode;
}

Rem::~Rem()
{
    _mText.Clear();
    _mComponents.clear();
}

Rem::Rem(Rem::Type type_, const Source::Location& src)
{
    _mType = type_;
    _mSrc = src;
    if (!src._FunctionName.empty())
    {
        // Use "src_funcname" coorectly soon...;
        ++Rem::_mcIndent;
        //...
    }
}

Rem::Rem(Icon::T aIcon, Rem::Type type_, const Source::Location& src)
{
    Ic = aIcon;
    _mType = type_;
    _mSrc = src;
    if (!src._FunctionName.empty())
    {
        // Use "src_funcname" coorectly soon...;
        ++Rem::_mcIndent;
        //...
    }

}

Rem& Rem::operator<<(Rem::Code c_)
{
    String str;
    int i = static_cast<int>(c_);
    str << Rem::codes_ansi256_attr[i] << codes_text[i].data();
    _mComponents.emplace_back(str());
    return *this;
}

void Rem::InitCodes()
{
    if (!Rem::types_ansi256_attr.empty())
        return;
    Rem::types_ansi256_attr =
    {
        Color::Ansi(Color::Red4),       // err,
        Color::Ansi(Color::Yellow),         // warning,
        Color::Ansi(Color::BlueViolet),     // info,
        Color::Ansi(Color::Cyan3),          // exception,
        Color::Ansi(Color::DeepSkyBlue7),   // fatal,
        Color::Ansi(Color::DarkSlateGray1), // status,
        Color::Ansi(Color::LightCoral),     // debug,
        Color::Ansi(Color::White),          // output,
        Color::Ansi(Color::Grey0),          // comment,
        Color::Ansi(Color::DeepPink1),      // syntax
    };

    Rem::codes_ansi256_attr =
    {
        Color::Ansi(Color::Lime),            // hello,
        Color::Ansi(Color::LightCyan3),      // ok,
        Color::Ansi(Color::Red3),            // rejected,
        Color::Ansi(Color::LightSeaGreen),   // accepted,
        Color::Ansi(Color::LightPink4),      // eof,
        Color::Ansi(Color::LightPink3),      // eos,
        Color::Ansi(Color::Grey0),           // null_ptr,
        Color::Ansi(Color::LightGoldenrod2), // not_empty,
        Color::Ansi(Color::Grey),            // empty,
        Color::Ansi(Color::LightSteelBlue3), // unexpected,
        Color::Ansi(Color::LightSteelBlue3), // expected,
        Color::Ansi(Color::Reset),           // endl,
        Color::Ansi(Color::Reset),           // begin,
        Color::Ansi(Color::Reset),           // end,
        Color::Ansi(Color::White),           // implement,
        Color::Ansi(Color::LightPink3),      // exist,
        Color::Ansi(Color::LightPink4),      // not exist,
        Color::Ansi(Color::White),           //_file_,
        Color::Ansi(Color::SkyBlue3),        //_function_,
        Color::Ansi(Color::SkyBlue3),        //_fn_,
        Color::Ansi(Color::White),           //_line_
    };
}

std::string Rem::operator()()
{
    return CC();
}

/*
 *
 *  <div class="nom de la classe"...> contenu </div>
 *  <span class="nom de la classe"...> contenu </span>
 *  <i class="icone id-icon">text content</i>
 *  <script> code javascript </script>
 */

std::string Rem::CC()
{
    String str;
    str += Rem::types_ansi256_attr[static_cast<int8_t>(_mType)];
    if (Ic)
    {
        str += Color::Ansi(Color::Yellow);
        str += Ic;
    }
    str += Rem::types_ansi256_attr[static_cast<int8_t>(_mType)];
    str += Rem::types_text[static_cast<int8_t>(_mType)];
    str += Color::Ansi(Color::White);
    str += ' ';

    if (!_mSrc._Filename.empty())
    {
        // str += '\n';
        str += Rem::codes_ansi256_attr[static_cast<int16_t>(Rem::Code::File)];
        CString fname = _mSrc._Filename.c_str();
        CString::Token::List words;
#ifndef _WIN32
        if (fname.Tokenize(words, "/") > 0)
#else
        if (fname.Tokenize(words, "\\") > 0)
#endif
            str += words.back()();
        else
            str += fname();
        str += ' ';
    }
    if (_mSrc._LineNo > 0)
    {
        str << ": " << Color::White << _mSrc._LineNo << ' ' << Rem::codes_ansi256_attr[static_cast<int16_t>(Rem::Code::Line)] << ' ';
    }

    if (!_mSrc._FunctionName.empty())
    {
        str += Rem::codes_ansi256_attr[static_cast<int16_t>(Rem::Code::Function)];
        str += _mSrc._FunctionName;
        str += ": ";
    }

    for (auto const& s : Rem::_mComponents)
    {
        str += s;
        // str << ' ';
    }
    _mText = str;
    return str();
}

Rem& Rem::operator<<(const String& txt_)
{
    _mComponents.push_back(txt_());
    return *this;
}

Rem& Rem::operator<<(const std::string& txt_)
{
    _mComponents.push_back(txt_);
    return *this;
}


void Rem::Init()
{ // ...
    Rem::InitCodes();
}
std::string Rem::CodeText(Rem::Code c)
{
    String str;
    if (c == Rem::Code::Enter)
    {
        ++Rem::_mcIndent;
        return "";
        //...
    }
    else if (c == Rem::Code::Leave)
    {
        --Rem::_mcIndent;
        return "";
        //...
    }

    str << Rem::codes_ansi256_attr[static_cast<int8_t>(c)];
    str << Rem::codes_text[static_cast<int8_t>(c)];
    return str;
}

std::string Rem::TypeText(Rem::Type t)
{
    std::string str;
    str += Rem::types_ansi256_attr[static_cast<int8_t>(t)];
    str += Rem::types_text[static_cast<int8_t>(t)];
    return str;
}

Rem& Rem::Error(Source::Location&& SL)
{
    MessagesList.push_back({ Rem::Type::Err, std::move(SL) });
    return MessagesList.back();
}

Rem& Rem::Warning(Source::Location&& SL)
{
    MessagesList.push_back({ Icon::CWarning ,Rem::Type::Warning, std::move(SL) });
    return MessagesList.back();
}

Rem& Rem::Infomation(Source::Location&& SL)
{
    MessagesList.push_back({ Icon::CInfo, Rem::Type::Info, std::move(SL) });
    return MessagesList.back();
}

Rem& Rem::Exception(Source::Location&& SL)
{
    MessagesList.push_back({ Rem::Type::Exception, std::move(SL) });
    return MessagesList.back();
    ;
}

Rem& Rem::Fatal(Source::Location&& SL)
{
    MessagesList.push_back({ Rem::Type::Fatal, std::move(SL) });
    return MessagesList.back();
}

Rem& Rem::Status(Source::Location&& SL)
{
    MessagesList.push_back({ Rem::Type::Status, std::move(SL) });
    return MessagesList.back();
}

Rem& Rem::Debug(Source::Location&& SL)
{
    MessagesList.push_back({ Icon::CBug, Rem::Type::Debug, std::move(SL) });
    return MessagesList.back();
}

Rem& Rem::Output(Source::Location&& SL)
{
    MessagesList.push_back({ Rem::Type::Output, std::move(SL) });
    return MessagesList.back();
}

Rem& Rem::Comment(Source::Location&& SL)
{
    MessagesList.push_back({ Rem::Type::Comment, std::move(SL) });
    return MessagesList.back();
}

Rem& Rem::Syntax(Source::Location&& SL)
{
    MessagesList.push_back({ Rem::Type::Syntax, std::move(SL) });
    return MessagesList.back();
}

int Rem::Clear(std::function<void(Rem& M)> aFp)
{
    int sz = MessagesList.size();
    if (MessagesList.empty())
        return sz;
    for (auto& M : MessagesList)
        if (aFp)
            aFp(M);
    MessagesList.clear();
    return sz;
}

}
