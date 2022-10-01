#pragma once

#include <TuxLS/Ui/DllConfig.h>

#include <vector>
#include <TuxLS/Core/Expect.h>
#include <TuxLS/Ui/UiDefs.h>

namespace Tux
{

class UI_LIB Object
{
    DECLOBJ_ID
public:



    using List = std::vector<Object*>;
    using Iterator = List::iterator;

    Object(Object&&) = delete;
    Object(const Object&) = delete;

    Object();// {}
    Object(Object* aParent);

    virtual ~Object(); // Rien a faire ici

    virtual Expect<> SetParent(Object* aParent);
    virtual Expect<> AppendChild(Object* aWidget);
    virtual Expect<Object::Iterator> QueryChild(Object* aWidget);
    virtual Expect<> Detach();
    virtual Expect<> DetachChild(Object* O);

    template<typename T = Object> T* CastTo()
    {
        return dynamic_cast<T*>(this);
    }

    template<typename T = Object> T* Parent()
    {
        Object* O = this;
        while (O->mParent)
        {
            O = O->mParent;
            T* D = dynamic_cast<T*>(O);
            if(D) return D;
        }
        return nullptr;
    }


    static void Dispose(Object* O);
    static int  ReleaseGC();
    State::Type CurrentState() { return State; }
    virtual State::Type SetState(State::Type St);
private:

    Object* mParent = nullptr;
    List    mChildList;
    static List mGCChildList;

protected:

    State::Type State = State::Normal;

    Object::List& Children() { return mChildList; }
};


}

