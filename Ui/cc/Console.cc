#if defined(_WIN32)
#define WIN32_LEAN_AND_MEAN
#define VC_EXTRALEAN
#   include <Windows.h>
#   include <stdlib.h>
#   include <io.h>
#   define write _write
#elif defined(__linux__)
#   include <sys/ioctl.h>
#   include <unistd.h>
#endif // Windows/Linux

#include <TuxLS/Ui/Console.h>
#include <TuxLS/Ui/UiDefs.h>

#include <string>

namespace Tux
{

CLASSNAME_IMPL(Console)

Console UI_LIB _iConsole;


Console::Console() : _mTerminal(_iConsole) {}

Console::~Console()
{
    //...
    delete Mem;
    //...

}


Expect<> Console::GetScreenSize()
{

#if defined(_WIN32)
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
    WH = { static_cast<int>(csbi.srWindow.Right - csbi.srWindow.Left + 1), static_cast<int>(csbi.srWindow.Bottom - csbi.srWindow.Top + 1) };
#elif defined(__linux__)
    struct winsize win;
    ioctl(fileno(stdout), TIOCGWINSZ, &win);
    WH = { static_cast<int>(win.ws_col), static_cast<int>(win.ws_row) };
#endif // Windows/Linux

    Rem::Debug(SourceLocation) << " ScreenSize: " << Color::Yellow << WH.ToString();
    return Rem::Code::Ok;
}

Expect<> Console::Init()
{

    auto R = _iConsole.GetScreenSize();
    if (!R)
        throw Rem::Fatal(SourceLocation) << Rem::Code::Rejected << " Getting screen/console dimensions...";

    std::cout << "\033[?1049h";
    Console::GotoXY({});
    CsrHide();

    //... To be continued

    return Rem::Code::Ok;
}

Console& Console::Instance()
{
    return _iConsole;
}

Console* Console::InstancePtr()
{
    return& _iConsole;
}

void Console::Clr(const Color::Set& C)
{
    String Str = "%s%s\033[2j";
    Str << Color::Ansi(C.Fg) << Color::AnsiBack(C.Bg);
}


void Console::SetAttr(VDC::Cell Cell)
{
    _iConsole.Attr = Cell;
    //Rem::Debug(SourceLocation) << _iConsole.Attr.Details();
}


int Console::Terminate()
{
    //int Count = Widget::EmptyGarbage();
    //Rem::Debug(SourceLocation) << " Widget release count : " << Color::Yellow << Count << Color::Reset;

    // Switch to normal screen:
    std::cout << "\033[0m\033[?1049l";
    CsrShow();
    return 0;
}

void Console::CsrHide()
{
    std::cout << "\033[?25l";
}

void Console::CsrShow()
{
    std::cout << "\033[?25h";
}

Tux::Size Console::Geometry()
{
    return _iConsole.WH;

}



/*!
    @brief Rendu sur la console en une seule methode pour
    &eacute;viter la surcharge d'appels et de constructions locales inutiles
    <ol>
        @li Input: VDC; (sous-)r&eacute;gion du VDC &agrave; exposer &agrave; la console.
        @li Determiner la g&eacute;ometrie du VCD ( Rectangle R: partie int&eacute;rieure expos&eacute;e du VDC ).
        @li Valider la partie visible de la sous-r&eacute;gion expos&eacute;e du VDC ( R & DC->Geometry(); ).
        @li ( Position du VDC dans la g&eacute;ometrie de la console ) - DC->Position() -> Toujours absolu ( donc Console:{0,0} + DC->Position());
        @li Intersection avec la G&eacute;ometrie de la console (valider la partie visibile du VDC : Copie locale de DC->Position())...
        @li Autre Rectangle locale r&eacute;sultant de l'intersection de la Console et de la partie visible du VDC. (CR = R + DC->Position(); );
        @li Boucle des lignes et colonnes expos&eacute;s du VDC;
    </ol>
    @author oldlonecoder, lussier.serge@gmail.com
*/
Expect<> Console::RenderDC(VDC* DC, Rectangle aR)
{
    Rectangle R = DC->Geometry(); // {0,0, DC->Width(), DC->Height()}

    if(aR)
    {
        R &= aR; // aR Intersecte avec R ?
        if (!R)
            Rem::Code::Rejected;
    }
    R = Rectangle({ 0,0 }, _iConsole.WH.WH) & R; // R intersected avec la Console ?
    if (!R)
        return Rem::Code::Rejected;

    Rem::Debug(SourceName) << " Dirty Rect: " << R.ToString();
    Rectangle CR = R + DC->Position(); // Positionner au DC.

    Rem::Debug(SourceName) << " Region du DC exposé:" << Color::Yellow << CR.ToString() << Color::Reset;

    VDC::Cell::Type* In = DC->At({ R.A });

    VDC::Cell Cell{ In };
    VDC::Cell PrevCell{ In };

    _iConsole << Cell.RenderColors();

    Point CXY{ CR.A };
    using Ansi = Tux::Attr<Tux::Color::Format::Ansi256>;

    for (int Y = 0; Y < R.Height(); Y++)
    {
        Console::GotoXY(CXY);
        In = DC->At({ R.A.X, R.A.Y+Y });

        for (int X = 0; X < R.Width(); X++)
        {
            Cell.C = *In;

            if (PrevCell.Bg() != Cell.Bg())
                _iConsole << Ansi::Bg(Cell.Bg());
            if (PrevCell.Fg() != Cell.Fg())
                _iConsole << Ansi::Fg(Cell.Fg());
            if (Cell.C & VDC::Cell::UGlyph)
            {
                auto Ic = Cell.IconID();
                write(1, Icon::Data[Ic], std::strlen(Icon::Data[Ic]));
            }
            if (Cell.C & VDC::Cell::Accent)
            {
                auto Acc = Cell.AccentID();
                write(1, Accent::Data[Acc], std::strlen(Accent::Data[Acc]));
            }
            else
                write(1, &Cell.C, 1);
            PrevCell.C = *In++;
        }
        ++CXY.Y;
    }
    return Expect<>();
}

Expect<> Console::RenderScanLine(VDC* DC, int LineNum, Rectangle aR)
{
    using Ansi = Tux::Attr<Tux::Color::Format::Ansi256>;
    Point Pt{ aR.A.X, aR.A.Y + LineNum };
    VDC::Cell::Type* C = DC->At(Pt);
    VDC::Cell::Type* P = C;
    VDC::Cell Cell = { *C };
    VDC::Cell PCell{ *P };
    _iConsole << Cell.RenderColors();
    for (int X = 1; X <= aR.Width(); X++)
    {
        PCell.C = *P;
        Cell.C = *C;
//        Rem::Debug() << "RenderScanLine(" << Color::Yellow << Pt.ToString() << Color::White << "):";
//        Rem::Output() << PCell.Details() << " <-> " << Cell.Details();
        if (PCell.Bg() != Cell.Bg())
            _iConsole << Ansi::Bg(Cell.Bg());
        if (PCell.Fg() != Cell.Fg())
            _iConsole << Ansi::Fg(Cell.Fg());
        if (Cell.C & VDC::Cell::UGlyph)
        {
            auto Ic =  Cell.IconID();
            write(1, Icon::Data[Ic], std::strlen(Icon::Data[Ic]));
        }
        else
            write(1,&Cell.C,1);
        P = C++;
    }

    //_iConsole << Color::Ansi(Color::Reset);
    //std::flush(std::cout);
    return Rem::Code::Accepted;
}

Expect<> Console::GotoXY(const Point& XY)
{
    std::cout << "\033[" << XY.Y << ';' << XY.X << "H";

    std::flush(std::cout);
    return Rem::Code::Accepted;
}

Console& Console::operator<<(const std::string& aStr)
{
    write(1, aStr.c_str(), aStr.length());
    return *this;
}


Console& Console::operator<<(char C)
{
    write(1, &C, 1);
    return *this;
}

Console& Console::operator<<(Icon::Type C)
{
    write(1, Icon::Data[C], std::strlen(Icon::Data[C]));
    return *this;
}


}

