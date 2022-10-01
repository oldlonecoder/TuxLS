//
// Created by oldlonecoder on 2022-07-06.
//

//#ifndef LSC_MESSAGE_H
//#define LSC_MESSAGE_H
#pragma once
#include <TuxLS/Core/String.h>
#include <TuxLS/Core/Encodings/Icons.h>

namespace Tux
{
namespace Source
{

struct CORE_LIB Location
{
    std::string _FunctionName;
    std::string _Filename;
    int _LineNo;
};

}

#define SourceLocation                   \
    {                                    \
        __PRETTY_FUNCTION__, __FILE__, __LINE__ \
    }
#define SourceLongName                    \
    {                                     \
        __PRETTY_FUNCTION__, "", __LINE__ \
    }
#define SourceName                       \
    {                                    \
        __FUNCTION__, __FILE__, __LINE__ \
    }
#define SourceFile                       \
    {                                    \
        __FUNCTION__, __FILE__, __LINE__ \
    }

class CORE_LIB Rem
{
    String _mText;
    Source::Location _mSrc = {};
    Color::Format _mFormat = Color::Format::Ansi256;
    Icon::T Ic = nullptr;

    std::vector<std::string> _mComponents;
    static int _mcIndent;

public:
    using List = std::vector<Rem>;

    enum class Type : int8_t
    {
        Err,
        Warning,
        Info,
        Exception,
        Fatal,
        Status,
        Debug,
        Output,
        Comment,
        Syntax
    };

    enum class Code : uint16_t
    {
        Hello,
        Ok,
        Rejected,
        Accepted,
        Eof,
        Eos,
        NullPtr,
        NotEmpty,
        Empty,
        Unexpected,
        Expected,
        Endl,
        Begin,
        End,
        Implement,
        Exist,
        NotExist,
        File,
        Function,
        ShortFunction,
        Line,
        Enter, // indent
        Leave  // unindent
    };

    using collection = std::vector<Rem>;

    Rem() = default;
    Rem(const Rem& r);
    Rem(Rem&& r) noexcept;

    Rem(Rem::Type type_, const Source::Location& src = {});
    Rem(Icon::T aIcon, Rem::Type type_, const Source::Location& src = {});
    ~Rem();

    Rem& operator=(const Rem&) = default;
    Rem& operator=(Rem&&) noexcept = default;

    Rem& operator<<(Rem::Code c_);
    Rem& operator<<(const String& txt_);
    Rem& operator<<(const std::string& txt_);

    template <typename T> Rem& operator<<(const T& arg_)
    {
        String str;
        str << arg_;
        _mComponents.push_back(str());
        return *this;
    }

    std::string operator()();
    std::string CC();
    std::string Text() { return _mText(); }
    static void Init();
    Rem::Code MessageCode() { return _mCode; }
    Rem::Type MessageType() { return _mType; }
    static std::string CodeText(Rem::Code c);
    static std::string TypeText(Rem::Type t);
    Rem::Code CodeEnum() { return _mCode; }
    static void InitCodes();

    //-----------------------------------------------------------------------------------
#pragma region STATIC_CONSTRUCTS_TYPES
    static Rem& Error(Source::Location&& SL = {});
    static Rem& Warning(Source::Location&& SL = {});
    static Rem& Infomation(Source::Location&& SL = {});
    static Rem& Exception(Source::Location&& SL = {});
    static Rem& Fatal(Source::Location&& SL = {});
    static Rem& Status(Source::Location&& SL = {});
    static Rem& Debug(Source::Location&& SL = {});
    static Rem& Output(Source::Location&& SL = {});
    static Rem& Comment(Source::Location&& SL = {});
    static Rem& Syntax(Source::Location&& SL = {});
#pragma endregion STATIC_CONSTRUCTS_TYPES

#pragma region STATIC_CONSTRUCT_CODES
    //... Not useed yet
#pragma endregion STATIC_CONSTRUCTS_CODES
        //-----------------------------------------------------------------------------------

    static int Clear(std::function<void(Rem& M)> aFp);

private:
    Rem::Code _mCode = Rem::Code::Hello;
    Rem::Type _mType = Rem::Type::Output;

    static std::vector<std::string_view> types_text;
    static std::vector<std::string_view> codes_text;
    static std::vector<std::string> types_ansi256_attr;
    static std::vector<std::string> codes_ansi256_attr;
    friend class Logger;
};

} // Lsc

//#endif //LSC_MESSAGE_H
