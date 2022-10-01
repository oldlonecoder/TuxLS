#include <TuxLS/Ui/VDC.h>



namespace Tux
{

Tux::VDC::VDC(Object* aOwner, const Size& aSZ) :
    _Owner(aOwner), WH(aSZ)
{
}

Tux::VDC::~VDC()
{
    if (_Block) delete[] _Block;
}

Expect<VDC::Cell::Type*> VDC::New(const Size& aSZ)
{
    if (aSZ)
    {
        WH = aSZ;
        if (_Block)
            delete[] _Block;
    }

    if(!WH)
        throw Rem::Error(SourceName) << " Cannot allocate Cell::Type Bloc using undefined Geometry.";

    _Block = new Cell::Type[WH.WH.X * WH.WH.Y + WH.WH.X];

    try { Clear(_Attr); }
    catch (Rem& R) { throw R; }

    return _Block;
}


Expect<> VDC::Delete()
{
    return Rem::Code::Implement;
}

Expect<> VDC::Clear()
{
    if (!_Block)
        throw Rem::Fatal(SourceName) << " Attempt to clear a null VDC";

    //Rem::Debug(SourceLocation) << " Clear " << Color::Yellow << sizeof(_Block) << "bytes;";
    auto* C = _Block;
    int blk = WH.WH.X * WH.WH.Y;;
    for (int X = 0; X < blk; X++) *C++ = _Attr;
    return Rem::Code::Accepted;
}

Expect<> VDC::Clear(const Rectangle& aSubR)
{
    Rectangle B;
    if (aSubR)
        B = Rectangle(WH) & aSubR;
    else
        return Clear();

    if(!_Block)
        throw Rem::Fatal(SourceLocation) << " Attempt to clear a null VDC";


    if (!B)
        return Rem::Code::Rejected;

    for (int Y = B.A.Y; Y <= B.B.Y; Y++)
    {
        Cell::Type* C = At({B.A.X,Y});
        for (int x = 0; x < B.Width(); x++) *C++ = _Attr;
    }
    return Rem::Code::Accepted;
}

Expect<> VDC::Clear(VDC::Cell::Type aAttr, const Rectangle& aSubR)
{
    auto A = _Attr;
    _Attr = aAttr;
    Clear(aSubR);
    _Attr = A;
    return Rem::Code::Accepted;
}


inline VDC::Cell::Type* VDC::At(Point XY)
{
    if (!Rectangle(WH).Contains(XY))
        throw Rem::Exception(SourceLocation) << Rem::Code::Rejected << " [" << Color::Yellow << XY.ToString() << Color::Reset << "]";
    return _Block + (XY.Y * WH.WH.X) + XY.X;
}

int VDC::Width()
{
    return WH.WH.X;
}

int VDC::Height()
{
    return WH.WH.Y;
}

std::string VDC::Cell::Details()
{
    String Str = "Cell Details%s[Char:'%s%d%s'/Fg:'%s%s%s'/Bg:'%s%s%s']";
    Str << Color::White << Color::Yellow << (int)Char() << Color::White
        << Color::Yellow << Color::Name(Fg()) << Color::White
        << Color::Yellow << Color::Name(Bg()) << Color::White;

    return Str();
}


#pragma region CellCode



VDC::Cell::Cell(VDC::Cell::Type aCell)
{
    C = aCell;
}

VDC::Cell::Cell(VDC::Cell::Type* aCell)
{
    C = *aCell;
}

VDC::Cell& VDC::Cell::SetFg(Color::Type aFg)
{
    C = (C & ~FGMask) | (static_cast<Cell::Type>(aFg) << FGShift);
    return *this;
}

VDC::Cell& VDC::Cell::SetBg(Color::Type aBg)
{
    C = (C & ~BGMask) | (static_cast<Cell::Type>(aBg) << BGShift);
    return *this;
}

VDC::Cell& VDC::Cell::SetAttribute(VDC::Cell::Type aAttr)
{
    C = (C & ~AttrMask) | aAttr;
    return *this;
}

VDC::Cell& VDC::Cell::operator=(VDC::Cell::Type aC)
{
    C = aC;
    return *this;
}

VDC::Cell& VDC::Cell::operator=(VDC::Cell::Type* aC)
{
    C = *aC;
    return *this;
}

Color::Type VDC::Cell::Fg()
{
    return static_cast<Color::Type>((C & FGMask) >> FGShift);
}

Color::Type VDC::Cell::Bg()
{
    return static_cast<Color::Type>((C & BGMask) >> BGShift);
}
char VDC::Cell::Char()
{
    return static_cast<char>(C & CharMask);
}

uint16_t VDC::Cell::Attributes()
{
    return (C & AttrMask) >> ATShift;
}

VDC::Cell& VDC::Cell::operator<<(char aCharacter)
{
    C = (C & ~CharMask) | aCharacter;
    return *this;
}

VDC::Cell& VDC::Cell::operator<<(Icon::Type aGlyph)
{
    C = (C & ~(Cell::AttrMask | Cell::CharMask)) | Cell::UGlyph | aGlyph;
    return *this;
}

VDC::Cell& VDC::Cell::operator<<(Tux::Accent::Type aAccent)
{
    C = (C & ~(Cell::UGlyph | Cell::CharMask)) | Cell::Accent | aAccent;
    return *this;
}


VDC::Cell& VDC::Cell::ResetAttributes(VDC::Cell::Type Bits)
{
    C = (C & (CharMask | UGlyph)) | Bits;
    return *this;
}

Icon::Type VDC::Cell::IconID()
{
    if (!(C & Cell::UGlyph)) return Icon::NullPtr;
    auto Ic = C & Cell::CharMask;
    if (Ic > 59) Ic = Icon::Bug;
    return  Ic;
}


Accent::Type VDC::Cell::AccentID()
{
    auto AID = C & Cell::CharMask;
    if (AID > Accent::Ucirc) return Accent::Agrave;
    return static_cast<Accent::Type>(AID);
}


std::string VDC::Cell::RenderColors()
{
    using Ansi = Tux::Attr<Tux::Color::Format::Ansi256>;
    std::string str;
    str += Ansi::Bg(Bg()) += Ansi::Fg(Fg());
    return str;
}

#pragma endregion CellCode

}
