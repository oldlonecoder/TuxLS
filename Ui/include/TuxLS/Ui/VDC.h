#pragma once

#include <TuxLS/Ui/Object.h>
#include <TuxLS/Ui/Geometry.h>
#include <TuxLS/Core/Encodings/Accent.h>

namespace Tux
{

class UI_LIB VDC
{
    Object* _Owner = nullptr;
    Point Pos = { 0,0 }; ///< Position relative de ce Widget dans son parent sinon Pos == COffset si ce Widget est un TopLevel Absolu;
    Size WH;

    Rectangle Peer =  Rectangle::Nil; ///< Sous-Rectangle visible, � render � la console.

    friend class Console;
    friend class Painter;
public:
    using Type = uint32_t*;
    VDC() = default;
    VDC(Object* aOwner, const Size& aSize = {});

    ~VDC();

    struct UI_LIB Cell
    {
        using Type = uint32_t;

        // ============== Bitmasks =============================
        static constexpr uint32_t CharMask = 0x000000FF;
        static constexpr uint32_t FGMask = 0x0000FF00;
        static constexpr uint32_t BGMask = 0x00FF0000;
        static constexpr uint32_t CMask = 0x00FFFF00;
        static constexpr uint32_t AttrMask = 0xFF000000;
        // ==============Bit Shift ============================
        static constexpr int FGShift = 0x08;
        static constexpr int BGShift = 0x10;
        static constexpr int ATShift = 0x18;
        // ==================Attributes==========================
        static constexpr uint32_t  UGlyph   = 0x01000000;
        static constexpr uint32_t  Underline = 0x02000000;
        static constexpr uint32_t  Stroke   = 0x04000000;
        static constexpr uint32_t  Blink    = 0x08000000;
        static constexpr uint32_t  Accent   = 0x10000000;
        Cell::Type C = 0l;
        Cell() = default;
        Cell(Cell::Type aCell);
        Cell(Cell::Type* aCell);
        ~Cell() = default;

        Cell& SetFg(Color::Type aFg);
        Cell& SetBg(Color::Type aBg);
        Cell& SetAttribute(VDC::Cell::Cell::Type aAttr);

        Cell& operator=(VDC::Cell::Cell::Type aC);
        Cell& operator=(VDC::Cell::Cell::Type* aC);

        Color::Type Fg();
        Color::Type Bg();
        Cell& SetColor(Color::Set aSet)
        {
            SetFg(aSet.Fg);
            SetBg(aSet.Bg);
            return *this;
        }

        char Char();
        uint16_t Attributes();

        Cell& operator<<(char aCharacter);
        Cell& operator<<(Icon::Type aGlyph);
        Cell& operator<<(Tux::Accent::Type aAccent);
        Cell& ResetAttributes(Cell::Type Bits);
        Icon::Type IconID();
        Accent::Type AccentID();
        std::string RenderColors();
        operator std::string();
        std::string Details();
        Cell::Type& operator *() { return C; }
        Color::Set Colors() { return { Fg(),Bg() }; }

    };

    Cell::Type* _Block = nullptr;
    Cell::Type  _Attr = 0x20;
    Expect<VDC::Cell::Type*> New(const Size& aSZ = {});
    Expect<> Delete();

    Expect<> Clear();
    Expect<> Clear(const Rectangle& aSubR);
    Expect<> Clear(VDC::Cell::Type aAttr, const Rectangle& aSubR = Rectangle::Nil);

    Cell::Type& Attributes() { return _Attr; }
    Cell::Type* At(Point XY);
    Cell::Type* Mem() { return _Block; }
    Rectangle Geometry() { return Rectangle({ 0,0 }, WH.WH.X, WH.WH.Y); };
    int Width();
    int Height();
    const Point& Position() { return Pos;  }
    void SetPosition(const Point& aXY) { Pos = aXY; }

};
}

