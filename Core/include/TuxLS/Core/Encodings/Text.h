#pragma once

#include <TuxLS/Core/Encodings/TextAttr.h>
#include <TuxLS/Core/String.h>
#include <TuxLS/Core/Encodings/Icons.h>
#include <TuxLS/Core/Encodings/Accent.h>
#include <TuxLS/Core/Expect.h>

namespace Tux
{
    /*!
        * @todo write docs
        *
        <p>
            Test Sample:
        </p>
        @code Expect<> Test::TestText()
        {
            Text txt;
            txt = " <FB: Reset; F:Yellow; Icon:Home;> Salut <FB:Reset White;  f:Yellow;> &agrave; <FB:Reset;>Toi!";
            txt.Compile();

            return Message::Code::Ok;
        }
        @endcode

    */
    class CORE_LIB Text : public String
    {
    public:

        /**
            * Default constructor
            */
        Text();

        /**
            * Copy constructor
            *
            * @param other TODO
            */
        Text(const Text& other);
        Text(const String& Str);
        Text(String&& Str) noexcept;
        Text(const std::string& aStr);
        Text(std::string&& aStr);
        Text(const char* aStr);
        /**
            * Destructor
            */
        ~Text() override;
        Expect<> Compile();

        struct CORE_LIB TokenInfo
        {
            enum class Type : uint8_t
            {
                Accent,
                AttrCmd,
                Identifier,
                Punctuation,
                Justify,
                Unset,
                Eof
            };

            enum class Mnemonic : uint16_t
            {
                Fg,
                Bg,
                Color,
                Ic,
                ArgSeq,
                ArgSep,
                Identifier,
                OpenTag,
                ClosingTag,
                Br,
                Eos,
                AccentSeq,
                JustifySeq, ///< " ... |- "
                Left,
                Top,
                Right,
                Bottom,
                Center,
                HVCenter,
                Null
                // ...
            };

            struct LocationData
            {
                const char* Begin = nullptr;
                const char* End = nullptr;
                int Index = -1;
            }Loc;

            using Lexem                       = const char*;
            static constexpr Lexem Fg        = "FG";
            static constexpr Lexem Bg        = "BG";
            static constexpr Lexem ColorPair  = "COLOR";
            static constexpr Lexem Icon      = "ICON";
            static constexpr Lexem OpenSeq    = "<";
            static constexpr Lexem CloseSeq   = ">";
            static constexpr Lexem ArgSeq     = ":";
            static constexpr Lexem ArgSep     = ",";
            static constexpr Lexem EoSt       = ";";
            static constexpr Lexem LineBreak  = "BR";
            static constexpr Lexem Accent     = "&";
            static constexpr Lexem Left       = "[";
            static constexpr Lexem Right      = "]";
            static constexpr Lexem Top        = "^";
            static constexpr Lexem Bottom     = "_";
            static constexpr Lexem Center     = "-"; // ex.: "<^-..." Top | Center 
            static constexpr Lexem HVCenter   = "+";
            static constexpr Lexem Null       = "Unset";

            TokenInfo::Type T = TokenInfo::Type::Unset;
            TokenInfo::Mnemonic M = TokenInfo::Mnemonic::Null;
            TokenInfo::Lexem L = nullptr;

            std::string operator()();

            static TokenInfo Scan(const char* Start);

            using List = std::vector <TokenInfo>;
            static List Referential;

            TokenInfo() = default;

            TokenInfo(TokenInfo::Type aType, TokenInfo::Mnemonic aMenomic, TokenInfo::Lexem aLexem,int nParams);
            TokenInfo(TokenInfo&&)noexcept;
            TokenInfo(const TokenInfo&) = default;

            TokenInfo& operator=(TokenInfo&& r) noexcept
            {
                T = r.T;
                M = r.M;
                L = r.L;
                Loc = std::move(r.Loc);
                return *this;
            }
            TokenInfo& operator=(const TokenInfo& r)
            {
                T = r.T;
                M = r.M;
                L = r.L;
                Loc = r.Loc;
                return *this;
            }

            operator bool() { return (L != nullptr) || (T != TokenInfo::Type::Unset); }

            std::string Mark(const char*);
            int NParam = 0;

        };

        struct CORE_LIB Attribute
        {
            Color::Type Fg  = Color::Black;
            Color::Type Bg  = Color::Black;
            Icon::Type Ic   = Icon::NullPtr;
            Accent::Type Ac = Accent::Err;

            const char* Begin = nullptr;
            const char* End   = nullptr;
            const char* C     = nullptr;

            Attribute() = default;
            Attribute(TokenInfo& Info);
            Attribute(Attribute&&) noexcept = default;
            Attribute(const Attribute&) = default;
            ~Attribute() = default;
            std::string Infos();

            Attribute& operator = (Attribute&&) noexcept = default;
            Attribute& operator = (const Attribute&) = default;
            using List = std::vector<Text::Attribute>;
        };

        void PushAttribute(Text::Attribute Attr);
    private:

        Text::Attribute::List Attributes;

        struct Compiler
        {
            enum class State : uint8_t {
                Out,        ///< In outside context state ( normal text ) - Begin Seq is Expected
                StartAttr,  ///< Enter Attributes context '<' - Entering into Seq state (inside) context - Requires Out current state
                EndAttr,    ///< EndSeq '<' Requires IntAttr current state; Switch to Out
                InAttr,     ///< in Attr Seq, requires BeginAttr current state.
                AttrSeq,    ///< Command Token requires InAttr current state.
                AttrArg,    ///< Command
                ArgSeq,     ///< Argument Seq, requires CmdToken current state.
                StartUTF,   ///< UTF-8 accent '&' requires Out current state.
                EndUTF,     ///< Set right after ParseAccentMnemonic.
                EndSeq
            };

           // State St = State::Out; unused
            const char* B = nullptr;
            const char* C = nullptr;
            const char* E = nullptr;

            TokenInfo::List Tokens; ///< Sous r&eacute;serve...Pas certain d'en avoir besoin.

            bool operator ++(); ///< Prefix
            bool operator ++(int); ///< Postfix
            Expect<> Skip();
            Text& TextRef;

            void CloseToken(TokenInfo& Info);
            Expect<> Compile();
            Expect<Text::Attribute> CompileAttribute();
            Expect<Text::Attribute> CompileAccent();
            Expect<Text::TokenInfo> Scan();

            Text::TokenInfo SkipToAttr();

            Expect<Color::Type> ColorID(TokenInfo& Token);
            Expect<Icon::Type> IconID(TokenInfo& Token);
            Text::TokenInfo ScanIdentifier();
            Expect<> SkipToken(Text::TokenInfo& Token);
            Compiler() = delete;
            Compiler(Text& aTextInstance);
            ~Compiler() = default;
            bool Eof();

            using ParserFnPtr = Expect<>(Text::Compiler::*)(Text::Attribute&);
            using ParsersTable = std::unordered_map<TokenInfo::Mnemonic, Text::Compiler::ParserFnPtr>;

            Expect<> ParseIcon(Text::Attribute& A); // Parser pour ':' + 'IconID' + ';'
            Expect<> ParseFg(Text::Attribute& A); // Parser pour ':' + 'ColorID' + ';'
            Expect<> ParseBg(Text::Attribute& A); // Parser pour ':' + 'ColorID' + ';'
            Expect<> ParseColor(Text::Attribute& A); // Parser pour ':' + 'ColorID' + ',' + 'ColorID' + ';'
            Expect<> ParseLineBreak(Text::Attribute& A); // 'BR'; ( <br; .. > ou <br>)
            Expect<> CloseAttribute(Text::Attribute& A); // 'BR'; ( <br; .. > ou <br>)
            Expect<> CheckEos(Text::Attribute& A);
            std::string Mark();
        };

        static Text::TokenInfo Scan(Text::TokenInfo::Mnemonic M);
    };
}
