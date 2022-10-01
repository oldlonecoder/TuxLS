#include <TuxLS/Ui/Geometry.h>


namespace Tux
{


#pragma region Point

[[maybe_unused]] Point Point::Invalid((bool) true);

Point Point::Nul((bool) true);


Point::Point(int px, int py) : X(px), Y(py), S(true), N(false)
{

}


std::string Point::ToString() const
{
    std::ostringstream _if;
    _if << '(' << X << ',' << Y << ')';
    return _if.str();
}

/*!
 *    \fn Point::isNull()
 */
bool Point::IsNull()
{
    return !N;
}

/*!
 *    \fn Point::invalidate()
 */
void Point::Invalidate()
{
    N = true;
}

/*!
 *    \fn Point::enable()
 */
void Point::Enable()
{
    N = false;
}

#pragma endregion Point

#pragma region Size

Size::Size(Point xy, Point mxy, Point xxy) :
    WH(std::move(xy)), Min(std::move(mxy)), Max(std::move(xxy))
{}

Size::Size(Point aXY):
    WH({aXY.X,aXY.Y})
{

}

Size &Size::operator=(Point xy)
{
    WH = xy;
    ///@todo check limits!!!!
    return *this;
}
Size &Size::SetMin(Point xy)
{
    Min = xy;
    return *this;
}
Size &Size::SetMax(Point xy)
{
    Max = xy;
    return *this;
}
std::string Size::ToString() const
{
    String Str = "%d,%d";
    Str << WH.X <<  WH.Y;
    return Str();
}


#pragma endregion Size


#pragma region Rectangle
Rectangle Rectangle::Nil;
Rectangle Rectangle::Void;

Rectangle::Rectangle()
{

}

Rectangle::Rectangle(const Point &a_, int w, int h)
{
    A = a_;
    B = {A.X + w - 1, A.Y + h - 1};
    S = {w, h};
}

Rectangle::Rectangle(const Point &a_, const Size &s_)
{
    S = s_;
    A = a_;
    B = {A.X + S.WH.X - 1, A.Y + S.WH.Y - 1};
}

Rectangle::~Rectangle()
{
}

Rectangle Rectangle::Inter(const Rectangle &rec) const
{
    // intersect self with <r>
    Rectangle r = Rectangle::Nil;
    Point       _a, _b;
    _a = {A.X <= rec.A.X ? rec.A.X : A.X, A.Y <= rec.A.Y ? rec.A.Y : A.Y};
    _b = {B.X <= rec.B.X ? B.X : rec.B.X, B.Y <= rec.B.Y ? B.Y : rec.B.Y};

    bool in = Contains(_a) || Contains(_b);
    if(!in)
        return r;
    r.Assign(_a, _b);
    return r;
}

/*!
 * @brief creates A Rectangle from the union between this Rectangle and the given one
 * @param Rectangle r the given Rectangle to make the union with this Rectangle
 * @return new Rectangle as the result of the union
 */
Rectangle Rectangle::Union(const Rectangle &r)
{
    int       xx1, h, yy1, w;
    Rectangle rec;
    xx1 = r.A.X < A.X ? r.A.X : A.X;
    w   = r.S.WH.X > S.WH.X ? r.S.WH.X : S.WH.X;
    yy1 = r.A.Y < A.Y ? r.A.Y : A.Y;
    h   = r.S.WH.Y > S.WH.Y ? r.S.WH.Y : S.WH.Y;

    rec.Assign(xx1, yy1, w, h);
    return rec;

}

void Rectangle::ResizeTo(const Point &xy)
{
    Assign(A.X, A.Y, xy.X, xy.Y);
}

void Rectangle::Resize(const Point &dxy)
{
    Assign(A.X, A.Y, dxy.X, dxy.Y);
}

void Rectangle::BRResize(const Point &dxy)
{
    Assign(A.X, A.Y, S.WH.X + dxy.X, S.WH.Y + dxy.Y);
}

std::string Rectangle::ToString() const
{
    String str;
    //str << "rect[" << A.X << ", " << A.Y << ", : " << S.WH.X << '/' << B.X << ", " << S.WH.Y << '/' << B.Y << ']';
    str << "rect[" << A.X << ", " << A.Y << ", : " << S.WH.X << ", " << S.WH.Y <<  ']';
    return str();
}

bool Rectangle::Intersect(Rectangle& rR, const Size& WH)
{
    Rectangle R = Rectangle({ 0,0 }, WH.WH.X, WH.WH.Y);
    rR = R & rR;
    return rR.Valid();
}


#pragma endregion Rectangle
#pragma region WinBuffer


//- -------------------------------- WinBuffer ------------------------------------------------------------


WinBuffer &WinBuffer::Goto(int x, int y)
{
    cxy = {x, y};
    return *this;
}

void WinBuffer::Set(int w, int h)
{
    r  = {{0, 0}, w, h};
    sz = r.S; // sous reserve  : pour fin de limite
    Clear();
}

WinBuffer &WinBuffer::operator++()
{
    if(cxy.X >= r.S.WH.X)
    {
        if(cxy.Y <= r.S.WH.X)
        {
            cxy.Y++;
            cxy.X = 0;
        }
    }
    else
        cxy.X++;

    return *this;
}

WinBuffer &WinBuffer::operator++(int)
{
    if(cxy.X >= r.S.WH.X)
    {
        if(cxy.Y <= r.S.WH.X)
        {
            cxy.Y++;
            cxy.X = 0;
        }
    }
    else
        cxy.X++;
    return *this;
}

WinBuffer &WinBuffer::operator--()
{

    return *this;
}

WinBuffer &WinBuffer::operator--(int)
{
    return *this;
}

/// <summary>
/// Serge Lussier
/// </summary>
/// <param name="txt"></param>
/// <returns></returns>
WinBuffer &WinBuffer::TPut(const std::string &txt)
{
    int line_width = r.Width() - cxy.X;
    int ln         = txt.length();

    int dx = line_width <= ln ? line_width : ln;

    std::string::iterator crs = Buffer->_mStr.begin() + cxy.Y * r.Width() + cxy.X;
    auto                  p   = txt.begin();

    for(int x = 0; x < dx; x++) *crs++ = *p++;

    cxy.X += line_width;
    return *this;
}

void WinBuffer::Clear()
{
    if(!Buffer)
        Buffer = new String;
    else
        Buffer->Clear();

    Buffer->_mStr = std::string(r.Width() * r.Height(), ' ');
    //std::cerr << __PRETTY_FUNCTION__ << "contents: '" << Buffer->_d << "'\n";
}

void WinBuffer::Release()
{
    delete Buffer;
}

std::string WinBuffer::Details()
{
    String str = "WinBuffer details:\n";
    str << r.ToString() << " cursor: " << cxy.ToString();
    return str();
}

WinBuffer::operator std::string()
{
    std::string str;
    if(!Buffer)
        return "oops! there is no such defined and initialised area!";
    str += '\n';
    for(int l = 0; l < r.Height(); l++)
    {
        for(int c = 0; c < r.Width(); c++)
            str += *(Buffer->Begin() + (l * r.Width() + c));
        str += '\n';
    }
    return str;
}

#pragma endregion Rectangle


}
