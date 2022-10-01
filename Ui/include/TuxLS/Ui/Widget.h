#pragma once
//#include <TuxLS/Ui/Object.h>
#include <TuxLS/Ui/Geometry.h>
//#include <TuxLS/Ui/VDC.h>
#include <TuxLS/Ui/Painter.h>


namespace Tux
{

class UI_LIB Widget : public Object
{
    Rectangle R;
    VDC* DC = nullptr;

    Point RelPos;

    DECLOBJ_ID


public:

    Widget();// = default;
    Widget(Object* aParent, WClass::Type F = 0);


    ~Widget() override;

    virtual Expect<> SetGeometry(int x, int y, int w, int h);
    virtual Expect<> SetGeometry(Point XY, int w, int h);
    virtual Expect<> SetGeometry(const Rectangle& aR);
    virtual Expect<Rectangle> Geometry();
    virtual Expect<Rectangle> RelativeGemometry();

    virtual bool Draw();
    virtual Expect<> Update();
    virtual Expect<> Update(Rectangle aR);
    void SetDefaultCellAttr(VDC::Cell C) { _DefaultAttr = C; }
    virtual Expect<Painter> PainterDC(Rectangle aR = Rectangle::Nil);
    Expect<> AppendChild(Widget* aObj);

    bool IsTopLevel() const { return _F & WClass::TopLevel; }
    bool IsChild() const { return _F & WClass::Child; }
    Point Position() { return RelPos; }
protected:

    WClass::Type _F = 0;
    VDC::Cell    _DefaultAttr = { 0x00F00020 }; // Char: ' ',  Fg:F0, Bg:00
    Point SetupPosition();
};

}
