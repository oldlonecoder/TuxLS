#include <TuxLS/Ui/Widget.h>
#include <TuxLS/Ui/Console.h>
#include <TuxLS/Ui/Elements/Element.h>

namespace Tux
{

CLASSNAME_IMPL(Widget)


Widget::Widget():Object()
{
}

Widget::Widget(Object* aParent, WClass::Type F):Object(aParent), _F(F)
{
    if (aParent)
    {
        _F |= WClass::Child;
       Parent<Widget>()->_F |= WClass::Parent;
    }
   _DefaultAttr.SetColor(Element::List["Widget"][State::Normal]);
}



Widget::~Widget()
{
    if(_F & WClass::TopLevel)
    {
        Rem::Debug(SourceName) << " Deleting VDC here.";
        if (DC) delete DC;
    }
}


Expect<> Widget::SetGeometry(int x, int y, int w, int h)
{
    R.Assign(x, y,  w,h );

    if (!IsTopLevel())
        DC = Parent<Widget>() ? Parent<Widget>()->DC : nullptr;

    if(!DC)
    {
        DC = new VDC(this, R.S);
        DC->_Attr = _DefaultAttr.C;
        DC->New();
        DC->SetPosition(R.A);
        //return Rem::Code::Ok;
    }
    Rem::Debug(SourceName) << Class() << " Position of this widget in DC: " << SetupPosition().ToString();

    return Rem::Code::Ok;
}


Expect<> Widget::SetGeometry(Point XY, int w, int h)
{
    return SetGeometry(XY.X,XY.Y,w,h);
}

Expect<> Widget::SetGeometry(const Rectangle& aR)
{
    return SetGeometry(aR.A.X, aR.A.Y, aR.S.WH.X, aR.S.WH.Y);
}

bool Widget::Draw()
{
    DC->Clear(R);
    Rem::Debug(SourceName) << Color::Cyan3 << Class() << Color::White << " > " << Children().size() << " children:";
    for (auto* Obj : Children())
    {
        Widget* W = Obj->CastTo<Widget>();
        Rem::Output() << Color::Cyan3 << Obj->Class() << Color::White << ":";
        if (W)
        {
            Rem::Output() << Color::White << " Proceed to call "
                << Color::LightGreen2 << W->Class()
                << Color::White << "::"
                << Color::Cyan3 << "Draw"
                << Color::White << "():";
            W->Draw();
        }
    }
    return true;
}

Expect<> Widget::Update()
{
    return Console::RenderDC(DC);

}

Expect<> Widget::Update(Rectangle aR)
{
    return Console::RenderDC(DC, aR);
}

Expect<> Widget::AppendChild(Widget* aObj)
{
    if(!aObj)
        throw Rem::Exception(SourceName) << " Attempt to append a child widget with nullptr value.";

    Object::AppendChild(aObj);
    _F |= WClass::Parent;
    //Widget* W = aObj->CastTo<Widget>();

    //Widget* W = (Widget*)aObj;
    if (!aObj)
        throw Rem::Exception(SourceName) << " Attempt to append a child widget with wrong pointer type.";

    aObj->_F |= WClass::Child;
    return Rem::Code::Accepted;
}

Point Widget::SetupPosition()
{
    if (IsTopLevel()) return R.A;
    Widget* W = Parent<Widget>();
    RelPos = R.A;
    Rem::Debug(SourceName) << Color::Cyan3 << Class() << " Relative Position:" << Color::Yellow << RelPos.ToString() << Color::Reset;
    if (W && !W->IsTopLevel())
    {
        RelPos = R.A + W->RelPos;
        Rem::Debug(SourceName) << Color::Cyan3 << Class() << "New Offset Position:" << Color::Yellow << RelPos.ToString() << Color::Reset;
    }

    return RelPos;
}


Expect<Painter> Widget::PainterDC(Rectangle aR)
{
    if (!aR)
        return Painter(DC, *RelativeGemometry()).ColorSet(_DefaultAttr.Colors());

    return Painter(DC, aR+RelPos).ColorSet(_DefaultAttr.Colors());
}


Expect<Rectangle> Widget::Geometry()
{
    return R;
}

Expect<Rectangle> Widget::RelativeGemometry()
{
    Rem::Debug(SourceName) << Color::Cyan3 <<  Class() << " DCPos: " << Color::Yellow << RelPos.ToString() << Color::Reset;
    return Rectangle(RelPos, R.Width(), R.Height());
}


}
