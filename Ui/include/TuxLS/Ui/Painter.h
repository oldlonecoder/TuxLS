#pragma once

#include <TuxLS/Ui/VDC.h>
#include <TuxLS/Ui/UiDefs.h>

namespace Tux
{

class UI_LIB Painter
{
    VDC* DC = nullptr;
    Rectangle _R;   ///< Geometrie interne. (Celui du DC ou un sous-rectangle de DC)
    VDC::Cell _Cell;
    VDC::Cell::Type* Cursor = nullptr;
    bool FWordWrap = false;

    Point CPos();
    Justify::Type JBits = 0;
    DECLOBJ_ID

public:

    Painter() = default;
    ~Painter() = default;

    Painter(VDC* aOwner, Rectangle aR = Rectangle::Nil);
    Painter(Painter&&) = default;
    Painter(const Painter&) = default;

    Painter& operator = (Painter&&) = default;
    Painter& operator = (const Painter&) = default;



    Painter& operator << (Color::Type aColorID);
    Painter& operator << (Icon::Type aIconID);
    Painter& operator << (const std::string& aStr);
    Painter& operator << (const String& aStr);
    Painter& operator << (const Point& XY);
    Painter& operator << (const char* aStr);
    Painter& operator << (Accent::Type aAcc);


    Painter& GotoXY(const Point& XY);
    Painter& Home() { return GotoXY({}); }

    template<typename T> Painter& operator << (const T& aStr)
    {
        std::ostringstream Out;
        Out << aStr;
        return (*this) << Out.str();
    }

    Painter& ColorSet(const Color::Set& aSet);

    Expect<> SetupGeometry();
    Painter& Clear();
   // void Update();
    Justify::Type& JustifyBits() { return JBits; }
    Painter& SetBits(Justify::Type aBits);// { Bits = aBits; return *this; }
    Painter& SetHCenter(bool S);// { Bits = S ? (Bits & ~HCenter) | HCenter : (Bits & ~HCenter); return *this; }
    Painter& SetVCenter(bool S);
    Painter& SetCenter(bool S);
    Painter& SetWordWrap(bool S);
    Painter& SetBg(Color::Type aBg);
    Painter& SetFg(Color::Type aFg);
};
}

