#pragma once
#include <TuxLS/Ui/DllConfig.h>
#include <TuxLS/Ui/VDC.h>
#include <TuxLS/Ui/Object.h>

namespace Tux
{

/**
  * @brief Screen Console IO Buffer
  *
  *
  * @author oldlonecoder. lussier.serge@gmail.com
  */
class UI_LIB Console
{
    // No need for a full Rectangle functions here. Just need the screen/console dimensions. Just create inline Rectangle for computations
    Size WH;
    Expect<> GetScreenSize();
    Console& _mTerminal;

    VDC* Mem = nullptr;
    VDC::Cell Attr;
    DECLOBJ_ID
public:
    //...
    Console();
    ~Console();
    static Expect<> Init();
    int Width() const { return WH.WH.X; }
    int Height() const { return WH.WH.Y; }
    Size Dimensions() const { return WH; }
    static int Terminate();
    static Expect<> RenderDC(VDC* DC, Rectangle aR = Rectangle::Nil);
    static Expect<> RenderScanLine(VDC* DC, int LineNum, Rectangle aR);
    static Expect<> GotoXY(const Point& XY);
    static void SetAttr(VDC::Cell Cell);
    static Console& Instance();
    static Console* InstancePtr();
    static void Clr(const Color::Set& C);
    static void CsrHide();
    static void CsrShow();
    static Tux::Size Geometry();
    Console& operator << (const std::string& aStr);
    Console& operator<<(char C);
    Console& operator<<(Icon::Type C);
    Console& operator<<(Color::Type C);

};

}