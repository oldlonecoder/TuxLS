#include <TuxLS/Ui/Object.h>



namespace Tux
{

CLASSNAME_IMPL(Object)


Object::List Object::mGCChildList = Object::List();

Object::Object()
{
    Rem::Debug() << " Creating instance of " << Class();
}

Object::Object(Object* aParent):
mParent(aParent)
{
    Rem::Debug(SourceName) << " Creating instance of " << Class() << " Parent == " << (aParent? aParent->Class() : "nullptr");

    if (mParent)
    {
        Rem::Output() << " Setting Parent of Class: " << mParent->Class();
        mParent->AppendChild(this);
    }
}

Object::~Object()
{
    Rem::Debug() << " Delete instance of " << Class();
}




Expect<> Object::Detach()
{
    if (mParent)
        mParent->DetachChild(this);
    return Rem::Code::Ok;
}

Expect<> Object::DetachChild(Object* W)
{
    if (Iterator Child; (Child = *QueryChild(W)) != mChildList.end())
        mChildList.erase(Child);
    else
        return Rem::Code::NotExist;

    return Rem::Code::Ok;
}

void Object::Dispose(Object* W)
{
    Object::mGCChildList.push_back(W);
}

Expect<> Object::SetParent(Object* aParent)
{
    mParent = aParent;
    if (aParent)
    {
        aParent->AppendChild(this);
    }
    return Rem::Code::Ok;
}

Expect<> Object::AppendChild(Object* aWidget)
{
    if (*QueryChild(aWidget) == mChildList.end())
    {
        Rem::Debug(SourceName) << Class() << " Adding the Object...";
        mChildList.emplace_back(aWidget);
        return Rem::Code::Accepted;
    }
    return Rem::Code::Exist;
}

Expect<Object::Iterator> Object::QueryChild(Object* aWidget)
{
    if (mChildList.empty())
        return mChildList.end();

    return std::find(mChildList.begin(), mChildList.end(), aWidget);
}

int Object::ReleaseGC()
{
    auto c = Object::mGCChildList.size();
    if(Object::mGCChildList.empty()) return 0;
    for(auto* O : Object::mGCChildList) delete O;
    Object::mGCChildList.clear();
    return c;
}

State::Type Object::SetState(State::Type St)
{
    State = St;
    return State;
}


}
