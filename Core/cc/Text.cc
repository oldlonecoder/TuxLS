#include <TuxLS/Core/Encodings/Text.h>
#include <TuxLS/Core/Rem.h>

namespace Tux
{

    // Referentiel des tokens du processeur :
    Text::TokenInfo::List Text::TokenInfo::Referential =
    {
        {Text::TokenInfo::Type::Punctuation,    Text::TokenInfo::Mnemonic::OpenTag,     Text::TokenInfo::OpenSeq, 0},
        {Text::TokenInfo::Type::Punctuation,    Text::TokenInfo::Mnemonic::ClosingTag,  Text::TokenInfo::CloseSeq,0},
        {Text::TokenInfo::Type::Punctuation,    Text::TokenInfo::Mnemonic::ArgSeq,      Text::TokenInfo::ArgSeq,0},
        {Text::TokenInfo::Type::Punctuation,    Text::TokenInfo::Mnemonic::ArgSep,      Text::TokenInfo::ArgSep,0},
        {Text::TokenInfo::Type::Punctuation,    Text::TokenInfo::Mnemonic::Eos,         Text::TokenInfo::EoSt,0},
        {Text::TokenInfo::Type::AttrCmd,        Text::TokenInfo::Mnemonic::Color,       Text::TokenInfo::ColorPair,2},
        {Text::TokenInfo::Type::AttrCmd,        Text::TokenInfo::Mnemonic::Fg,          Text::TokenInfo::Fg,1},
        {Text::TokenInfo::Type::AttrCmd,        Text::TokenInfo::Mnemonic::Bg,          Text::TokenInfo::Bg,1},
        {Text::TokenInfo::Type::AttrCmd,        Text::TokenInfo::Mnemonic::Ic,          Text::TokenInfo::Icon,1},
        {Text::TokenInfo::Type::AttrCmd,        Text::TokenInfo::Mnemonic::Br,          Text::TokenInfo::LineBreak,0},
        {Text::TokenInfo::Type::Accent,         Text::TokenInfo::Mnemonic::AccentSeq,   Text::TokenInfo::Accent,1}
        //...
    };

    // Parsers:




    Text::Text()
    {

    }

    Text::Text(const Text& other)
    {

    }

    Text::Text(const String& Str) : String(Str) {}

    Text::Text(String&& Str) noexcept : String(Str) {}

    Text::Text(const std::string& aStr) : String(aStr) {}

    Text::Text(std::string&& aStr) :String(aStr) {}

    Text::Text(const char* aStr) : String(aStr) {}

    Text::~Text()
    {

    }



    Text::TokenInfo Text::TokenInfo::Scan(const char* Start)
    {
        //Rem::Debug(SourceLocation) << " Entering with Start :[" << *Start << "] -> '" << Start << "':";
        if (!*Start)
            return {};
        
        for (auto TokenRef : Text::TokenInfo::Referential)
        {
            TokenRef.Loc.SBegin = Start;
            while (isspace(*Start)) ++Start;

            auto crs = Start;
            const char* rtxt = TokenRef.L;
            auto sz = std::strlen(rtxt);

            if (toupper(*crs) != toupper(*rtxt)) continue;

            while (*rtxt && *crs && (toupper(*crs) == toupper(*rtxt))) { ++crs; ++rtxt; }
            Rem::Debug(SourceName) << "Token.L = [" << Color::Yellow << TokenRef.L << Color::Reset << "]:";
            if (!*rtxt)
            {   // fin de Token.L :
                if (TokenRef.T == TokenInfo::Type::AttrCmd)
                {
                    // Si le mot continu alors ce n'est pas un Token:
                    if (*crs)
                    {   // *crs n'est pas un type d'espace:
                        if (isalnum(*crs))
                            continue;
                    }
                } // Il reste les autres non-espace comme les ponctuations, symboles...
                --crs; // Replacer crs sur le dernier caractere du Token.
                Rem::Debug(SourceName) << " Scanned to :" << Color::Yellow << *crs;
                TokenRef.Loc.Begin = Start;
                TokenRef.Loc.End = crs; // Fin du Token

                return TokenRef; // On le retourne.... doh!
            }
        }
        Rem::Debug(SourceName) << " '" << *Start << "' not a TokenRef";
        return {}; // crs ( ou Start )  n'est pas sur un token du referentiel.
    }


    Text::TokenInfo::TokenInfo(Text::TokenInfo::Type aType, Text::TokenInfo::Mnemonic aMnenomic, Text::TokenInfo::Lexem aLexem, int nParams) :
        T(aType), M(aMnenomic), L(aLexem),NParam(nParams)
    {

    }

    Text::TokenInfo::TokenInfo(Text::TokenInfo&& r) noexcept :
        T(r.T), M(r.M), L(r.L)
    {

        Loc = r.Loc;
        //Rem::Debug(SourceLocation) << " ->[" << (*this)() << "]";
    }

    std::string Text::TokenInfo::Mark(const char* Stream)
    {
        if (!*this)
            return " - Can't mark an invalid token...";
        const char* B = Loc.Begin;
        const char* E = Loc.End;
        while ((B >= Stream) && (*B != '\n')) --B;
        ++B;
        while (*E && (*E != '\n')) ++E;
        --E;

        std::string MStr = std::string(Loc.Begin - Stream, ' ');

        MStr += '^';
        String Str;
        Str << Color::Grey78 << std::string(B, E - B) << '\n' << Color::Yellow << MStr << Color::Reset;
        return Str();
    }

    Expect<> Text::Compile()
    {
        Text::Compiler Parse{ *this };
        auto A = Parse.Compile();

        Rem::Debug(SourceLocation) << " Number of compiled Attribute(s) :" << Attributes.size();
        for (auto A : Attributes)
        {
            Rem::Infomation() << A();
            Rem::Output() << Color::White << "Fg:" << Color::Yellow << static_cast<int>(A.Fg) << Color::White << ", Bg:" << Color::Yellow << static_cast<int>(A.Bg);
            Rem::Output() << Color::White << "Icon:" << Color::Yellow << static_cast<int>(A.Ic) << Color::Reset;
        }
        return Rem::Code::Accepted;
    }

    Expect<> Text::operator>>(std::string& Out)
    {
        
        const char* r = c_str();
        auto l = Length();
        const char* b = r;

        std::string AttrStr;

        for (auto const& A : Attributes)
        {
           
            while (r != A.Begin)
            {
                Out += *r;
                r++;
            }
            r += A().length();
            if(A.Assign.For && A.Assign.Bac)
            {
                Color::Set(A.Fg, A.Bg) >> AttrStr;
                Out += AttrStr;
            }
            else
            {

                if (A.Assign.Bac)
                {
                    AttrStr = F == Color::Format::Ansi256 ? Attr<Color::Format::Ansi256>::Bg(A.Bg) : Attr<Color::Format::Html>::Bg(A.Bg);
                    Out += AttrStr;
                }
                else
                if (A.Assign.For)
                {
                    AttrStr = F == Color::Format::Ansi256 ? Attr<Color::Format::Ansi256>::Fg(A.Fg) : Attr<Color::Format::Html>::Fg(A.Fg);
                    Out += AttrStr;
                }
            }
            
            if (A.Assign.Acc)
            {
                AttrStr = Tux::Accent::Data[A.Ac];
                Out += AttrStr;
            }
            else 
            {
                if (A.Assign.Ic)
                {
                    AttrStr = Tux::Icon::Data[A.Ic];
                    Out += AttrStr;
                }
                //...
            }
        }
        while ((r - b) < l)
        {
            Out += *r++;
        }
        return Rem::Code::Ok;
    }

    void Text::PushAttribute(Text::Attribute Attr)
    {
        Attributes.push_back(Attr);
    }

    Text::TokenInfo Text::Scan(Text::TokenInfo::Mnemonic M)
    {
        for (auto iM : Text::TokenInfo::Referential)
        {
            if (iM.M == M) return iM;
        }
        return {};
    }


    //     Text::Attribute::Attribute(const char* Start)
    //     {
    //         Begin = C = End = Start;
    //     }

    std::string Text::TokenInfo::operator()()
    {
        if (L == nullptr)
            return { "null" };

        return { Loc.Begin, (std::size_t)(Loc.End - Loc.Begin) + 1 };
    }

    Text::Compiler::Compiler(Text& aTextInstance) :
        B(aTextInstance.c_str()), C(aTextInstance.c_str()), E(aTextInstance.c_str()), TextRef(aTextInstance)
    {
        E = B + aTextInstance.Length() - 1;
    }

    bool Text::Compiler::operator ++()
    {
        return false;
    }

    bool Text::Compiler::operator ++(int)
    {
        return false;
    }

    //Expect<> Text::Compiler::Skip()
    //{
    //    //Rem::Debug() << " Text::TextParser::Skip(" << Color::Yellow << *C << Color::Reset << "):";
    //    while (C <= E)
    //    {
    //        ++C;
    //        if (!isspace(*C))
    //            return Rem::Code::Accepted;
    //    }
    //    return Rem::Code::Eof;
    //}



    void Text::Compiler::CloseToken(TokenInfo& Info)
    {
        Info.Loc.Index = Info.Loc.End - B;
        auto sz = Info.Loc.End - Info.Loc.Begin;
        C += sz + 1;

        Rem::Debug(SourceName) << " Token :\n" << Info.Mark(B);

    }

    Expect<Text::TokenInfo> Text::Compiler::Scan()
    {
        auto Token = Text::TokenInfo::Scan(C);
        if (!Token)
        {
            auto R = ScanIdentifier();
            if (!R)
                return Rem(Rem::Type::Syntax) << Token.Mark(TextRef.c_str());
        }
        return Token;
    }


    /*!
        @brief Compile le texte pour creer les attributs :

        @code
            Compile :
            loop:
                SkipToAttr
                CompileAttribute
            loop
        @endcode
    */
    Expect<> Text::Compiler::Compile()
    {
        // Build tokens stream:
        while (!Eof())
        {

            auto Token = SkipToAttr();
            Text::Attribute Attr;
            Expect<Text::Attribute> A;
            if (!Token) return Rem::Code::Eof;

            Rem::Debug(SourceName) << Rem::Code::Endl << Token.Mark(B);


            if (Token.M == Text::TokenInfo::Mnemonic::AccentSeq)
            {
                Attr.Begin = Token.Loc.Begin;
                A = CompileAccent(Attr);
            }
            else if (Token.M == Text::TokenInfo::Mnemonic::OpenTag)
            {
                Attr.Begin = Token.Loc.Begin;
                A = CompileAttribute(Attr);
            }

            if (!A)
            {
                Rem::Debug(SourceName) << " No attribute. Skipping to next \"OpenTag | AccentSeq\"...";
                //Skip();
                continue;
            }
            TextRef.PushAttribute(Attr);

        }
        return Rem::Code::Accepted;
    }

    /*!
        @brief Compiler une sequence d'attribut(s).

        Ici c'est moins &eacute;vident... On d&eacute;coupe ce qui est entre '<' et '>' en commandes d'attribut.
        //...
        // '<' 'Fg' ':' 'ColorID' ';'...'Color' : 'ColorID(Fg)' ',' 'ColorID(Bg)' ';' 'Reset' ';' '>'

        @note  "Lexing And Parsing... tout &ccedile;a en m&ecirc;me temps!".
    */
    Expect<Text::Attribute> Text::Compiler::CompileAttribute(Text::Attribute& Attr)
    {
        
        Text::TokenInfo Token;
        // On tokenize les elements d'attribut: ( Stop: ClosingTag('>') Token )
        std::map<Text::TokenInfo::Mnemonic, Text::Compiler::ParserFnPtr> Parsers =
        {
            {Text::TokenInfo::Mnemonic::Fg, &Text::Compiler::ParseFg},
            {Text::TokenInfo::Mnemonic::Bg, &Text::Compiler::ParseBg},
            {Text::TokenInfo::Mnemonic::Color, &Text::Compiler::ParseColor},
            {Text::TokenInfo::Mnemonic::Ic, &Text::Compiler::ParseIcon},
            //... Plus tard, on aura les Grilles et Fenetres
            {Text::TokenInfo::Mnemonic::ClosingTag, &Text::Compiler::CloseAttribute},
        };
        while (!Eof())
        {
            // on passe '<' 
            //Skip();
            Expect<> ER;
            // Expecting Text::TokenInfo::Mnemonic:
            Token = Text::TokenInfo::Scan(C);
            if (!Token)
                return Rem::Syntax(SourceName) << " Expected ACM (Attribute Command Mnemonic) Token" << Rem::Code::Endl << Mark();

            // Ici c'est obligatoire de faire une boucle qui teste explicitement les mnemonics sp&eacute;cifiques
            // parce qu'on ne prend en charge tous les mnemonics.
            for (auto [M, Fn] : Parsers)
            {
                if (M != Token.M) continue;
                SkipToken(Token);
                ER = (this->*Fn)(Attr);
                if (!ER)
                    return ER();
                break;
            }
            if (Token.M == Text::TokenInfo::Mnemonic::ClosingTag)
            {
                SkipToken(Token);
                Attr.End = Token.Loc.End;
                return Attr; // Peut &ecirc;tre vide si on es sur "<>"
            }
        }
        return Rem::Syntax(SourceName) << " Unexpected end of stream in Attribute parsing";
    }



    /*!
        @brief Cr&eacute;&eacute; un Attribute de texte Accent.

        @note Encoder un Accent est le plus facile, donc pas besoin d'accumuler les tokens ici: juste "parser" '&' + Id + ';' c'est tout!
    */
    Expect<Text::Attribute> Text::Compiler::CompileAccent(Text::Attribute& Attr)
    {

        Text::TokenInfo Token;
        //Skip();
        if(Eof())
            return Rem(Rem::Type::Syntax) << " Expected identifier.";

        Token = ScanIdentifier();
        if (!Token)
        {
            return Rem::Syntax(SourceName) << " Expected identifier." << Rem::Code::Endl << Mark();
        }

        Rem::Debug(SourceName) << " Identifier: " << Token() << " :";

        Accent::Type T = Accent::Code(Token());
        if (T == Accent::Err)
            return Rem::Syntax(SourceName) << " Unknown Code token(identifier) " << Rem::Code::Endl << Token.Mark(B);

        Attr = Text::Attribute(Token);
        Attr.Ac = T;
        Attr.Assign.Acc = 1;
        //Mandatory expect ';'
        (void)SkipToken(Token);
        if(CheckEos(Attr)) return Attr;
        return Rem::Syntax(SourceName) << " Expected Eos ';' (End Of Statement token)." << Rem::Code::Endl << Mark();
    }

    /*!
        @brief Tokenize jusqu'&agrave; '>' ou ';', puis compiler l'attribut.
    */

    Text::TokenInfo Text::Compiler::SkipToAttr()
    {
        Text::TokenInfo Token;

        while (!Eof())
        {
            auto c = toupper(*C);
            if (c != TokenInfo::OpenSeq[0])
            {
                if (c != TokenInfo::Accent[0])
                {
                    ++C;
                    continue;
                }
                Token.T = Text::TokenInfo::Type::Accent;
                Token.L = Text::TokenInfo::Accent;
                Token.M = Text::TokenInfo::Mnemonic::AccentSeq;
            }
            else
            {
                Token.T = Text::TokenInfo::Type::AttrCmd;
                Token.L = Text::TokenInfo::OpenSeq;
                Token.M = Text::TokenInfo::Mnemonic::OpenTag;
            }
            Token.Loc.Begin = Token.Loc.End = C;
            Token.Loc.Index = C - TextRef.c_str();
            SkipToken(Token);
            return Token;
        }
        return {};
    }

    bool Text::Compiler::Eof() { return C > E; }

   Expect<> Text::Compiler::ParseIcon(Text::Attribute& A)
   {
       auto Token = Text::TokenInfo::Scan(C);
       if (Token.T != Text::TokenInfo::Type::Punctuation)
           return Rem::Syntax(SourceName) << " Expected Punctuation token ':'" << Mark();
       SkipToken(Token);
       Token = ScanIdentifier();
       Rem::Debug(SourceName) << " Icon ID : '" << Token() << "' :";
        auto R = IconID(Token);
        if (!R)
        {
            Rem::Syntax(SourceName) << Rem::Code::NotExist << " Icon ID:\n" << Token.Mark(B);
            return {};
        }
        A.Ic = *R;
        A.Assign.Ic = 1;
        return CheckEos(A);
    }



    Expect<> Text::Compiler::ParseFg(Text::Attribute& A)
    {
        // C sur 'Fg'; ( Consomm&eacute; )
        // Attendus :  ':' , 'ColorID', '; | >';
        
        auto Token = Text::TokenInfo::Scan(C);
        if ((Token.T != TokenInfo::Type::Punctuation) || (Token.L != Text::TokenInfo::ArgSeq))
            return Rem::Syntax(SourceName) << " Expected token ':' " << Rem::Code::Endl << Mark();

        //Skip();
        Token = ScanIdentifier();
        if (!Token)
            return Rem::Syntax(SourceName) << " Expected Identifier token." << Rem::Code::Endl << Mark();

        auto R = ColorID(Token);
        if (!R) return R() << " in ParseFg";
        A.Fg = *R;
        A.Assign.For = 1;
        Rem::Output() << " Compiler::ParseFg - Token:" << Rem::Code::Endl << Token.Mark(B);
        return CheckEos(A);
    }

    Expect<> Text::Compiler::ParseBg(Text::Attribute& A)
    {
        // C sur 'Fg'; ( Consomm&eacute; )
        // Attendus :  ':' , 'ColorID', '; | >';
        auto Token = Text::TokenInfo::Scan(C);
        if ((Token.T != TokenInfo::Type::Punctuation) || (Token.L != Text::TokenInfo::ArgSeq))
            return Rem::Syntax(SourceName) << " Expected token ':'\n" << Mark();

        //Skip();
        Token = ScanIdentifier();
        if (!Token)
            return Rem::Syntax(SourceName) << " Expected Identifier token." << Rem::Code::Endl << Mark();

        auto R = ColorID(Token);
        if (!R) return R() << " in ParseFg";
        A.Bg = *R;
        A.Assign.Bac = 1;
        Rem::Output() << " Compiler::ParseFg - Token:" << Rem::Code::Endl << Token.Mark(B);

        return CheckEos(A);
    }

   std::string Text::Compiler::Mark()
   {
       const char* Left = C;
       const char* Right = C;
       while ((Left >= B) && (*Left != '\n')) --Left;
       ++Left;
       while (Right && (*Right != '\n')) ++Right;
       --Right;

       std::string MStr = std::string(Right - Left, ' ');

       MStr += '^';
       String Str;
       Str << Color::Grey78 << std::string(Left, Right - Left) << '\n' << Color::Yellow << MStr << Color::Reset;
       return Str();
   }



    


    Expect<> Text::Compiler::ParseColor(Text::Attribute& A)
    {

        auto Token = Text::TokenInfo::Scan(C);

        if (Token.M != Text::TokenInfo::Mnemonic::ArgSeq)
            return Rem::Syntax(SourceName) << Rem::Code::Unexpected << Rem::Code::Endl << Token.Mark(B);

        SkipToken(Token);
        
        Token = ScanIdentifier();
        if(!Token)
            return Rem::Syntax(SourceName) << " Expected Identifier token." << Rem::Code::Endl << Mark();

        auto R = ColorID(Token);
        if (!R) return R();

        A.Fg = *R;
        A.Assign.For = 1;
        // Ici on doit verfifer si on a une virgule ou eos ou closing tag;
        // Expect "," | ';' | '>'.
        
        Token = Text::TokenInfo::Scan(C);
        if ((Token.M != Text::TokenInfo::Mnemonic::ArgSep) && (Token.M != Text::TokenInfo::Mnemonic::Eos) && (Token.M != Text::TokenInfo::Mnemonic::ClosingTag))
            return Rem::Syntax(SourceName) << "Expected ',' (arg separator) or eos (';') or closing tag ('>') " << Rem::Code::Endl << Token.Mark(B);
        
        if ((Token.M == Text::TokenInfo::Mnemonic::Eos) || (Token.M == Text::TokenInfo::Mnemonic::ClosingTag))
        {
            if (A.Fg == Color::Reset)
            {
                A.Bg = A.Fg;
                if(Token.M == Text::TokenInfo::Mnemonic::Eos)
                    SkipToken(Token);

                return Rem::Code::Accepted;
            }

        }
        // Ici on a obligatoirement argsep:

        SkipToken(Token);
        
        Token = ScanIdentifier();
        //...
        R = ColorID(Token);
        if (!R) return R();
        A.Bg = *R;
        A.Assign.Bac = 1;
        return CheckEos(A);
    }



    Expect<> Text::Compiler::ParseLineBreak(Text::Attribute& A)
    {

        return Rem::Code::Implement;
    }

    Expect<> Text::Compiler::CloseAttribute(Text::Attribute& A)
    {
        //TextRef.PushAttribute(A);
        return Rem::Code::Accepted;
    }

    Expect<> Text::Compiler::CheckEos(Text::Attribute& A)
    {
        //Skip();
        auto Token = Text::TokenInfo::Scan(C);
        if ( (!Token) || ((Token.M != Text::TokenInfo::Mnemonic::Eos) && (Token.M != Text::TokenInfo::Mnemonic::ClosingTag)))
            return Rem::Syntax(SourceName) << " Expected ';'" << Rem::Code::Endl << Token.Mark(B);

        if (Token.M == Text::TokenInfo::Mnemonic::ClosingTag)
        {
           
            return Rem::Code::Accepted;
        }
        SkipToken(Token);
        A.End = Token.Loc.End;
        return Rem::Code::Accepted;
    }


    Text::TokenInfo Text::Compiler::ScanIdentifier()
    {
        const char* Sc = C;
        while (isspace(*Sc)) ++Sc;
        C = Sc;
        if (!isalpha(*Sc) && (*Sc != '_'))
        {
            Rem::Syntax(SourceName) << Rem::Code::Expected << " Identifier. Got " << *Sc << " instead.";
            return {};
        }

        Text::TokenInfo Token;
        while (*Sc && (isalnum(*Sc) || (*Sc == '_'))) ++Sc;
        --Sc;
        Token.Loc.Begin = C;
        Token.Loc.End = Sc;
        Token.L = C;
        Token.T = TokenInfo::Type::Identifier;
        return Token;
    }

    Expect<> Text::Compiler::SkipToken(Text::TokenInfo& Token)
    {
        C = Token.Loc.End;
        C++;
        return Rem::Code::Accepted;
    }


    Expect<Color::Type> Text::Compiler::ColorID(TokenInfo& Token)
    {
        auto Str = Token();
        Color::Type Colr = Color::Scan(Str);
        if (Colr == Color::Reset)
        {
            if (Str != "Reset")
                return Rem::Error() << " Expected Color::Type name (strict case match). Got '" << Color::Yellow << Str << Color::White << "' instead:" << Rem::Code::Endl
                << Token.Mark(B);
        }
        SkipToken(Token);
        return Colr;
    }



    Expect<Icon::Type> Text::Compiler::IconID(TokenInfo& Token)
    {
        auto Str = Token();
        Icon::Type IconId = Icon::Scan(Str);
        if (IconId == Icon::NullPtr)
            return Rem::Error() << " Expected Icon::Type name. Got '" << Color::Yellow << Str << Color::White << "' instead:" << Rem::Code::Endl << Token.Mark(B);
        SkipToken(Token);
        return IconId;
    }


    Text::Attribute::Attribute(TokenInfo& aInfo) : Begin(aInfo.Loc.Begin-1), End(aInfo.Loc.End), C(nullptr) {}

    std::string Text::Attribute::operator()() const
    {
        if (End)
        {
            std::string Str{ Begin, size_t(End - Begin) + 1 };
            return Str;
        }
        else
        {
            std::string Str{ Begin };
            return Str;
        }
    }

    std::string Text::Attribute::Infos()
    {
        return "implement";
    }



}
