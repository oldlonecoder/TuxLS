

#pragma once

#include <TuxLS/Ui/DllConfig.h>
#include <TuxLS/Core/String.h>


namespace Tux
{

#pragma region Point

struct UI_LIB Point
{

    int X = 0;
    int Y = 0;
    bool S = false; ///< Non-Set ( invalid )
    bool N = true;
    using List = std::vector<Point>;
    using Iterator = List::iterator;
    using ConstIterator = List::const_iterator;

    Point() = default;
    Point(const Point& PXY) : X(PXY.X), Y(PXY.Y), S(PXY.S) {}
    Point( int aX, int aY );

    Point(Point&& PXY)noexcept { X = PXY.X; Y = PXY.Y; }
    explicit Point ( bool ) {
        X = 0;
        Y = 0;
        N = true;
        S = false;
    }

    ~Point() = default;

    Point& operator -= ( const Point& XY )
    {
        if (!XY.S) return *this;
        X -= XY.X;
        Y -= XY.Y;
        return *this;
    }

    Point& operator += ( const Point& XY )
    {
        X += XY.X;
        Y += XY.Y;
        return *this;
    }

    Point operator+ ( const Point& P ) const
    {
        return {P.X + X, P.Y + Y };
    }

    Point operator- ( const Point& P ) const
    {
        return { X - P.X, Y - P.Y };
    }

    void operator() ( int NewX, int NewY ) {
        X = NewX;
        Y = NewY;
        S = true;
        N = false;
    }

    void move ( int dx, int dy ) {
        X += dx;
        Y += dy;
    }

    [[nodiscard]] std::string ToString() const;

    bool operator == ( const Point& XY ) const
    {
        return (X == XY.X && Y == XY.Y) && (N == XY.N);
    }

    Point& operator= ( const Point& XY ) {
        X = XY.X;
        Y = XY.Y;
        S = XY.S;
        N = !XY.N;
        return *this;
    }

    Point& operator= (Point& XY) {
        X  = XY.X;
        Y  = XY.Y;
        S  = XY.S;
        N  =  XY.N;
        return *this;
    }


    bool operator >= (const Point& XY) const{
        return X >= XY.X || Y >= XY.Y;
    }
    bool operator != ( const Point& XY )const {
        return XY.X != X || XY.Y != Y;
    }

    bool operator < ( const Point& XY ) const {
        return (X < XY.X ) || (Y < XY.Y );
    }

    bool operator > ( const Point& XY ) const {
        return (X > XY.X ) || (Y > XY.Y );
    }
    bool IsNull();
    void Invalidate();
    void Enable();

    /*!
     *   @breif operator boolean to check if this instance is nul ==> unset.
     *   @note boolean op = {X|Y == -1}
     */
    bool operator !() const
    {
        return (N || *this == Point::Nul);
    }

    Point& operator ++(){ ++X; return *this;}
    Point& operator ++(int){ ++Y; return *this;}
    Point& operator=(Point&& XY) noexcept
    {
        X = std::move(XY.X);
        Y = std::move(XY.Y);
        S = std::move(XY.S);
        return *this;
    }

    [[maybe_unused]] static Point Invalid;
    static Point Nul;
};

#pragma endregion Point

#pragma region Size
struct UI_LIB Size
{
    Point Min = {1,1};
    Point Max = {0,0};
    Point WH  = {0,0};

    Size() = default;
    Size(Point xy, Point mxy, Point xxy);
    Size(Point aXY);

    explicit operator bool() const { return WH.X > 0 && WH.Y > 0; }
    Size& operator=(const Size&)=default;
    Size& operator=(Point xy);
    Size& SetMin(Point xy);
    Size& SetMax(Point xy);
    std::string ToString() const;
};

#pragma endregion Size

#pragma region Rectangle


struct UI_LIB Rectangle
{
    Point A;
    Point B;
    Size S;
    bool NullInstance = true;

    using List = std::vector<Rectangle>;
    using Iterator   = std::vector<Rectangle>::iterator;
    using ConstIterator = std::vector<Rectangle>::const_iterator;
    //using int_has_t = std::map<int, Rectangle>;

    Rectangle();
    Rectangle ( const Point& A, int w, int h );
    Rectangle ( const Point& A, const Size& _size );
    Rectangle(const Size aSZ)
    {
        Assign({ 0,0 }, aSZ);
    }

    ~Rectangle();

    [[nodiscard]] bool Valid() const
    {
        return S.operator bool();
    }

    void operator()( int a, int b, int c, int d )
    {
        Assign(a,b,c,d);
    }
    bool operator[](const Point& xy)const
    {
        return Contains(xy);
    }

    Rectangle operator +( const Rectangle& R)
    {
        Rectangle RR = (*this) + R.A;
        return RR;
    }

    Rectangle operator +( Point xy) const{
        return {{A.X + xy.X, A.Y + xy.Y}, S};
    }

    bool operator()(const Rectangle* r) const{
        // calling A(&B) is to verify if the entire B is inside this Rectangle boundaries.
        return  Contains(r->A) && Contains(r->B);
    }
    bool operator != (const Rectangle& r) const{
        return S.WH != r.S.WH;
    }
    Rectangle   operator -( Point xy) const {
        Rectangle R(A, S);
        R -= xy;
        return R;
    }

    auto Area() {
        return S.WH.X * S.WH.Y;
    }

    bool operator >= (const Rectangle& R) const{
        return S.WH >= R.S.WH;
    }

    explicit operator  bool() const {
        return  S.operator bool();
    }

    bool operator == ( const Rectangle& R ) const
    {
        return A == R.A && B==R.B;
    }

    Rectangle operator& ( const Rectangle& R ) const  {
        return Inter ( R );
    }
    Rectangle& operator&= ( const Rectangle& R )  {
        *this = Inter ( R );
        return *this;
    }
    Rectangle operator | ( const Rectangle& R ) {
        return Union ( R );
    }

    Rectangle& operator |= (const Rectangle& R) {
        *this = Union(R);
        return *this;
    }
    Rectangle& operator += ( const Rectangle& R ) {
        Move ( R.A );
        return *this;
    }
    Rectangle& operator -= ( const Rectangle& R ) {
        Move ( -R.A.X, -R.A.Y );
        return *this;
    }
    Rectangle& operator -= ( const Point& XY ) {
        Move ( -XY.X, -XY.Y );
        return *this;
    }
    Rectangle& operator += ( const Point& XY ) {
        Move ( XY );
        return *this;
    }
    bool operator &&(const Point& XY) const
    {
        return Contains(XY);
    }

    [[nodiscard]] bool Contains ( int px, int py ) const {
        return Contains ( {px,py} );
    }
    bool Contains ( const Point& p ) const
    {
        return ( ( p.X >= A.X ) && ( p.X <= B.X ) && ( p.Y >= A.Y ) && ( p.Y <= B.Y ) );
    }
    Rectangle Inter ( const Rectangle& ) const;

    void Assign ( int a_, int b_, int w_, int h_ )
    {
        A.X = a_;
        A.Y = b_;
        B.X = A.X + w_ -1;
        B.Y = A.Y + h_ -1;
        S = { w_,h_ };
        (void)Valid();
    }

    void Assign ( const Point& pa, const Point& pb )
    {
        A = pa;
        B = pb;
        S = { B.X - A.X + 1, B.Y - A.Y + 1 };
        (void)Valid();
    }
    Rectangle Union ( const Rectangle& r );

    void Assign(const Point& pa, const Size& z)
    {
        A = pa;
        B.X = A.X + z.WH.X -1;
        B.Y = A.Y + z.WH.Y -1;
        S = z;
    }

    void ResizeTo ( const Point& xy );
    void Resize ( const Point& dxy );
    void BRResize(const Point& dxy);
    void MoveTo ( const Point& xy )
    {
        A.X = xy.X;
        A.Y = xy.Y;
        B.X =  A.X + S.WH.X -1;
        B.Y =  A.Y + S.WH.Y -1;
    }

    void Move ( const Point& dxy )
    {
        A.X += dxy.X;
        A.Y += dxy.Y;
        B.X += dxy.X;
        B.Y += dxy.Y;
        // no validation check needed here...
    }

    void Move ( int dx, int dy ) {
        A.X += dx;
        A.Y += dy;
        B.X += dx;
        B.Y += dy;
    }

    void MoveTo ( int A, int B ) {
        MoveTo ( Point { A,B } );
    }

    int Height() const  {
        return S.WH.Y;
    }

    [[nodiscard]] Size RSZ() const  {
        return S;
    }

    [[nodiscard]] int Width() const {
        return S.WH.X;
    }
    [[nodiscard]] std::string ToString() const;



    void ResizeAndCenter ( const Point& dxy )
    {
        Assign(A.X+ (-1*dxy.X), A.Y+ (-1*dxy.Y), S.WH.X+dxy.X*2, S.WH.Y+dxy.Y*2);
    }


    /*!
     *        \fn stdui::Rectangle::bottomleft()
     */
    [[nodiscard]] Point BottomLeft() const
    {
        return { A.X, B.Y };
    }

    /*!
     *        \fn stdui::Rectangle::topright()
     */
    [[nodiscard]] Point TopRight() const
    {
        return { B.X, A.Y };
    }

    static bool Intersect(Rectangle& rR, const Size& WH);
    static Rectangle Nil;
    static Rectangle Void;
};

#pragma endregion Rectangle

#pragma region WinBuffer

struct UI_LIB WinBuffer
{
    String* Buffer = nullptr;
    Point cxy;
    Rectangle r;
    Size sz;

    WinBuffer& Goto(int x, int y);
    WinBuffer& operator << (const Point& xy) { return Goto(xy.X, xy.Y); }
    void Set(int w, int h);

    WinBuffer& operator ++();
    WinBuffer& operator ++(int);
    WinBuffer& operator --();
    WinBuffer& operator --(int);
    WinBuffer& TPut(const std::string& txt);

    void Clear();
    void Release();
    template<typename T> WinBuffer& operator << (T v)
    {
        String str;
        str << v;
        return TPut(str());
    }

    std::string Details();
    operator std::string();
};

#pragma endregion WinBuffer

}