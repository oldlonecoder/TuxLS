#include <TuxLS/Ui/Elements/Element.h>


namespace Tux
{

Tux::Color::Set::Dictionary Element::List =
{
	{
		"Label",
		{
			{Color::White, Color::DarkBlue}, // Normal
			{Color::Grey23, Color::DarkBlue},  // Disable...
			{Color::Yellow, Color::Blue4},  // Disable...
		}
	},
	{
		"Widget",
		{
			{Color::White, Color::DarkBlue},
			{Color::White, Color::DarkBlue}
		}
	}
};



}
