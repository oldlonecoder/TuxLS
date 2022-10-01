#include <TuxLS/Ui/Painter.h>
#include <TuxLS/Ui/Console.h>

namespace Tux
{


CLASSNAME_IMPL(Painter)

Painter::Painter(VDC* aOwner, Rectangle aR):
    DC(aOwner), _R(aR)
{
    Rem::Debug(SourceName) << Color::Fuchsia <<  " Receiving Geometry: " << Color::Yellow << _R.ToString();
    SetupGeometry();
    //...
}


// ------------------------------- Check Boundaries : ----------------------------------------------------------
Painter& Tux::Painter::operator<<(Color::Type aColorID)
{
    _Cell.SetFg(aColorID);
    return *this;
}

Painter& Tux::Painter::operator<<(Icon::Type IconID)
{
    VDC::Cell* C = (VDC::Cell*) Cursor;
    (*C) << IconID;
    ++Cursor;
    return *this;
}



Painter& Painter::operator << (Accent::Type aAcc)
{
    VDC::Cell* C = (VDC::Cell*)Cursor;
    (*C) << aAcc;
    ++Cursor;
    return *this;
}


Painter& Painter::operator<<(const std::string& aStr)
{
    Point P = CPos();
    auto c = aStr.begin();
    Rem::Debug() << "Painter <<  \"" << aStr << "\" @" << P.ToString() << " -> in " << _R.ToString();
    while (c != aStr.end() && (P.X <= _R.B.X))
    {
        *Cursor = (
            _Cell
            <<
            *c++
            ).C;
        //Rem::Debug() << VDC::Cell{ *Cursor }.Details() << " @{" << Color::Yellow << CPos().ToString() << Color::White << "}";
        ++Cursor;
        P.X++;
    }

    return *this;
}

Painter& Painter::operator<<(const String& aStr)
{
    return (* this) << aStr();
}

Painter& Painter::operator<<(const Point& XY)
{
    Point Pt = XY + _R.A; // + {0,0} by default;
    if (!_R.Contains(Pt))
        throw Rem::Exception(SourceLocation) << Rem::Code::End << " : " << XY.ToString() << " is out of range in " << _R.ToString();

    Cursor = (VDC::Cell::Type*)DC->At(Pt);
    return *this;
}

Painter& Painter::operator<<(const char* aStr)
{
    Point P = CPos();
    const char* c = aStr;
    Rem::Debug() << "Painter <<  \"" << aStr << "\" @" << P.ToString() << " -> in " << _R.ToString();
    while (*c && (P.X <= _R.B.X))
    {
        *Cursor = (_Cell << *c++).C;
        ++Cursor;
        P.X++;
    }

    return *this;
}


// ------------------------------- Check Boundaries - end----------------------------------------------------------


Painter& Painter::GotoXY(const Point& XY)
{
    //Rem::Debug(SourceLocation) << " @(" << XY.ToString() << "):";
    Point Pt = XY + _R.A; // + {0,0} by default;
    if (!_R.Contains(Pt))
        throw Rem::Exception(SourceLocation) << Rem::Code::End << " : " << XY.ToString() << " is out of range in " << _R.ToString();

    Cursor = (VDC::Cell::Type*)DC->At(Pt);
    //Rem::Debug(SourceLocation) << " :Real coords: " << CPos().ToString() << " <-> Pt{" << Pt.ToString() << "}";
    return *this;
}


Painter& Painter::ColorSet(const Color::Set& aSet)
{
    _Cell.SetFg(aSet.Fg);
    _Cell.SetBg(aSet.Bg);
    *Cursor = (* Cursor & ~VDC::Cell::CMask) | _Cell.C & VDC::Cell::CMask;
    return *this;
}

Expect<> Painter::SetupGeometry()
{
    Rem::Debug(SourceName) << " Painter DC's '" << Color::Chartreuse6 <<  "Geometry: " <<  DC->Geometry().ToString();

    _R = _R ? Rectangle({ 0,0 }, DC->Width(), DC->Height()) & _R : Rectangle({ 0,0 }, DC->Width(), DC->Height());
    if (!_R)
        throw Rem::Exception(SourceName) << ": " << Rem::Code::Endl << " - Attempt to < Contruct > a Painter object on invalid Geometry : " << _R.ToString();
    Rem::Debug(SourceName) << " Configured Geometry:" << Color::Yellow << _R.ToString() << Color::Reset << " :";
    Cursor = DC->At(_R.A);
    //Rem::Debug(SourceLocation) <<  VDC::Cell{ *Cursor }.Details();
    return Rem::Code::Ok;
}

Painter& Painter::Clear()
{
    auto* CC = Cursor;
    Cursor = DC->At(_R.A);
    _Cell << ' ';
    Rem::Debug(SourceName) << _Cell.Details();
    Rem::Output() << "Clearing subrect:" << _R.ToString();
    //int Area = _R.Area();
//    Rem::Output() << "-> " << Color::Yellow << Area << Color::White << " cells in block";
    for(int y = 0; y< _R.Height(); y++)
    {
        GotoXY({ 0,y });
        for (int X = 0; X < _R.Width(); X++) *Cursor++ = _Cell.C;
    }

    Cursor = CC;
    return *this;
}

//void Painter::Update()
//{
//    Console::RenderDC(DC,_R);
//}


Point Painter::CPos()
{
    auto* B = DC->Mem();
    int dx = static_cast<int>((Cursor - B));
    Point Pt = { dx % DC->Width(), dx / DC->Width() };
    Rem::Debug(SourceName) << " Cursor @" << Color::Yellow << Pt.ToString() << Color::Reset;
    return Pt;
}



Painter& Painter::SetBits(Justify::Type aBits)
{
    JBits = aBits;
    return *this;
}

Painter& Painter::SetHCenter(bool S)
{
    JBits = S ? JBits | Justify::HCenter : JBits & ~Justify::HCenter;
    return *this;
}

Painter& Painter::SetVCenter(bool S)
{
    JBits = S ? JBits | Justify::VCenter : JBits & ~Justify::VCenter;
    return  *this;
}

Painter& Painter::SetCenter(bool S)
{
    JBits = S ? JBits | Justify::HCenter | Justify::VCenter : JBits & ~(Justify::HCenter | Justify::VCenter);
    return  *this;
}

Painter& Painter::SetWordWrap(bool S)
{
    return  *this;
}

Painter& Painter::SetBg(Color::Type aBg)
{
    _Cell.SetBg(aBg);
    return *this;
}

Painter& Painter::SetFg(Color::Type aFg)
{
    _Cell.SetFg(aFg);
    return *this;
}


}
