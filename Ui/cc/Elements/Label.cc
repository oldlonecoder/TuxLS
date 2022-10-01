#include <TuxLS/Ui/Elements/Label.h>
#include <TuxLS/Ui/Elements/Element.h>


namespace Tux
{
CLASSNAME_IMPL(Label)


Label::Label() : Widget()
{
    _DefaultAttr.SetColor(Element::List["Label"][State::Normal]);
}


Label::Label(Widget* aParent, String&& aText, const Point& aXY) : Widget(aParent), mText(std::move(aText))
{
    auto s = mText.Length();
    /// @todo Create Tux::Text and scan mText to get final geometry.
    SetGeometry(aXY, mText.Length(), 1); // Temporaire...
    _DefaultAttr.SetColor(Element::List["Label"][State::Normal]);
    Rem::Debug(SourceName) << Color::Cyan3 << Class() << "-> '"
        << Color::Yellow << mText
        << Color::Reset << "' DCPos:"
        << Color::Yellow << Position().ToString()
        << Color::Reset;
}

Label::~Label()
{
    mText.Clear();
}

State::Type Label::SetState(State::Type St)
{
    Object::SetState(St);
    _DefaultAttr.SetColor(Element::List["Label"][St]);
    Draw();
    return St;
}



bool Label::Draw()
{
    Rem::Debug() << Color::Orange5 << Class() << Color::White << "::" << Color::CadetBlue2 << "Draw" << Color::Reset << ":";
    auto Pen = *PainterDC();
    Pen.Clear();
    Pen.Home();
    Pen << mText;
    return true;
}


}