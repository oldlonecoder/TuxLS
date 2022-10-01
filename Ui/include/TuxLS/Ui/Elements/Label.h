#pragma once

#include <TuxLS/Ui/Widget.h>
#include <TuxLS/Core/Encodings/Text.h>

namespace Tux{

class UI_LIB Label : public Widget
{
    String          mText;
    // Text         mText; ///< Contient tous les Text::Attributes pre-analys&eacute;s donc pr&ecirc;ts &agrave; &ecirc;tre rendus
    Justify::Type   JBits = Justify::Center;   ///< Par defaut, le texte est aligne au centre horiz. et verticale
    Justify::Type   IcAlign = Justify::Prefix; ///< Par defaut, l'icone est a gauche
    Icon::Type      IconID = Icon::NullPtr;   ///< Par defaut Pas d'icone evidement
    DECLOBJ_ID
public:

    Label();
    Label(Widget* aParent, String&& aText, const Point& aXY = { 0,0 });
    bool Draw() override;
    ~Label() override;
    State::Type SetState(State::Type St) override;
};
}

