#pragma once
#include <cstdint>

namespace Tux
{
namespace Justify
{
using Type = uint8_t;
static constexpr uint8_t HCenter = 0x01;
static constexpr uint8_t VCenter = 0x02;
static constexpr uint8_t Center = 0x03;
static constexpr uint8_t WordWRap = 0x08;
static constexpr uint8_t Prefix = 0x10;
static constexpr uint8_t Suffix = 0x20; 
}

namespace Direction
{
using Type = int8_t;
static constexpr int8_t Left = -1;
static constexpr int8_t Right = 1;
static constexpr int8_t Up = 2;
static constexpr int8_t Down = -2;
}


namespace WClass
{
using Type = uint16_t;
static constexpr Type TopLevel = 0x0001;
static constexpr Type Floating = 0x0002;
static constexpr Type Child = 0x0004;
static constexpr Type Parent = 0x0008;
static constexpr Type Caption = 0x0010;
static constexpr Type Frame = 0x0020;
static constexpr Type Input = 0x0040;

}
// ...
namespace State
{
	using Type = uint8_t;
	static constexpr Type Normal  = 0x00;
	static constexpr Type Disable = 0x01;
	static constexpr Type Active  = 0x02;
	static constexpr Type Focus   = 0x04;
	static constexpr Type Error   = 0x08;
	static constexpr Type Warning = 0x10;
	static constexpr Type Illegal = 0x20;
}
}