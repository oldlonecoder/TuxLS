// TuxLSTests.cpp : Ce fichier contient la fonction 'main'. L'exécution du programme commence et se termine à cet endroit.
//

#include <iostream>
#include <TuxLS/Core/Encodings/TextAttr.h>
#include <TuxLS/Core/String.h>
#include <TuxLS/Core/Expect.h>
#include <TuxLS/Ui/Console.h>
#include <TuxLS/Ui/Elements/Label.h>
#include <TuxLS/Ui/Elements/Element.h>
#include <TuxLS/Core/Encodings/Accent.h>
#ifdef _WIN32
    #include <conio.h>
#else
#include <unistd.h>
#endif




using Tux::Rem;

Tux::Expect<> TestText()
{
    using Tux::Text;
    Text Txt = " Allo &agrave; vous tous! ";
    Txt.Compile();
    return Rem::Code::Ok;
}


Tux::Expect<> TestWidget()
{
    using Tux::Color;
    using Tux::Accent;
    Tux::Size WW = Tux::Console::Geometry();
    WW.WH.X -= 10;
    WW.WH.Y -= 4;
    Rem::Debug(SourceName) << "Bon maintenant, testons Widget et son Rendu dans la `New Console::RenderDC`:";
    Tux::Widget* W = new Tux::Widget(nullptr, Tux::WClass::TopLevel);
    W->SetGeometry(5, 2, WW.WH.X, WW.WH.Y);

    auto Painter = *W->PainterDC();
    Painter.GotoXY({1,1});
    Painter << "Je suis le TopLevel Widget ce texte &agrave; la position : {1, 1}.";

    Tux::Label* Lbl = new Tux::Label(W, " Bienvenue &agrave; TuxLS! Je suis un <F:Yellow; Icon:ArrowRight;> Tux::Label! <F: Yellow;> @{5,2} <FB: Reset;> ", Tux::Point(5,2));
    W->Draw();
    W->Update();
#ifdef _WIN32
    _sleep(500);
#else
    usleep(500);
#endif

    Lbl->SetState(Tux::State::Active);
    Lbl->Update();

    Tux::Object::Dispose(W);
    return Rem::Code::Ok;
}



int main()
{
    char dummy;
    Rem::Init();
    try {
        Tux::Console::SetAttr(Tux::VDC::Cell().SetColor(Tux::Color::Set{ Tux::Color::Yellow, Tux::Color::Blue }) << ' ');
        Tux::Console::Init();

        using Ansi = Tux::Attr<Tux::Color::Format::Ansi256>;
        using Html = Tux::Attr<Tux::Color::Format::Html>;

        (void)TestText();

        Tux::Console::Clr({ Tux::Color::Black, Tux::Color::Grey });

        Tux::Console::GotoXY({ 20,2 });
        Tux::Console::Instance() << Ansi::Bg(Ansi::Black) << "  Hello," << Ansi::Fg(Tux::Color::CadetBlue2) << " World!  \n" << Tux::Color::Ansi(Tux::Color::Reset);
        Tux::Console::GotoXY({ 20,3 });
        Tux::Console::Instance() << Ansi::Bg(Ansi::Black) << Tux::Icon::Windows << " & " << Tux::Icon::CLinux;

        auto R = TestWidget();

        if (!R)
            std::cout << " Ah! Expect<> TestConsole() ne retourne pas Rem:code!\n";

#ifdef _WIN32
        //std::cin >> dummy;
        while (!_kbhit()) _sleep(100);
        (void)_getch();
#else
        std::cin >> dummy;
#endif

        Tux::Object::ReleaseGC();

        Tux::Console::Terminate();
        Rem::Clear([](Rem& R) -> void
        {
            std::cout << R() << "\n";
        });
    }
    catch (Rem& REM)
    {
        Tux::Console::Terminate();
        char d;
        Rem::Clear([](Rem& R) -> void
            {
                std::cout << R() << "\n";
            });
    }
    return 0;
}

