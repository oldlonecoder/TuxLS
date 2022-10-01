#pragma once


#include <TuxLS/Core/Encodings/TextAttr.h>
#include <TuxLS/Ui/UiDefs.h>
#include <TuxLS/Ui/Geometry.h>



namespace Tux
{
/*!
 * @brief Structure et conteneur globales des &Eacute;l&eacute;ments d'interface-usager
 *
 * <h6> Contenant la configuration: </h6>
 *	<ul>
 *		@li Layouts
 *		@li Couleurs
 *		@li R&ecaute;f&ecaute;rentiels des glyphes
 *		@li Attributs
 *  </ul>

 * @author Serge Lussier (lussier.serge@gmail.com); oldlonecoder;
 *
 */
struct UI_LIB Element
{
	static Tux::Color::Set::Dictionary List; ///< =>  Element::List["Label"][Element::State::Normal];

};

}
