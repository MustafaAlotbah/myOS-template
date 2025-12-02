/**
 * @file Color.cpp
 * @brief Predefined color palette definitions
 *
 * @author Mustafa Alotbah
 * @copyright myOS Project
 */

#include <myOS/Color.h>

namespace myOS {

// ============================================================================
// GRAYSCALE PALETTE
// ============================================================================
const Color Color::White(255, 255, 255);
const Color Color::Gray100(245, 245, 245);
const Color Color::Gray200(238, 238, 238);
const Color Color::Gray300(224, 224, 224);
const Color Color::Gray400(189, 189, 189);
const Color Color::Gray500(158, 158, 158);
const Color Color::Gray600(117, 117, 117);
const Color Color::Gray700(97, 97, 97);
const Color Color::Gray800(66, 66, 66);
const Color Color::DarkGray(33, 33, 33);
const Color Color::DarkerGray(18, 18, 18);
const Color Color::DarkestGray(10, 10, 10);
const Color Color::Black(2, 2, 2);

// ============================================================================
// PRIMARY COLORS
// ============================================================================
const Color Color::Red(244, 67, 54);
const Color Color::Green(76, 175, 80);
const Color Color::Blue(33, 150, 243);

// ============================================================================
// SECONDARY COLORS
// ============================================================================
const Color Color::Yellow(255, 235, 59);
const Color Color::Cyan(0, 188, 212);
const Color Color::Magenta(233, 30, 99);

// ============================================================================
// DARK VARIANTS
// ============================================================================
const Color Color::DarkRed(183, 28, 28);
const Color Color::DarkGreen(27, 94, 32);
const Color Color::DarkBlue(13, 71, 161);
const Color Color::DarkerRed(92, 6, 16);

// ============================================================================
// LIGHT VARIANTS
// ============================================================================
const Color Color::LightRed(239, 154, 154);
const Color Color::LightGreen(165, 214, 167);
const Color Color::LightBlue(144, 202, 249);
const Color Color::LighterBlue(187, 222, 251);

// ============================================================================
// OTHER COLORS
// ============================================================================
const Color Color::Orange(255, 152, 0);
const Color Color::Purple(156, 39, 176);
const Color Color::Pink(236, 64, 122);
const Color Color::Brown(121, 85, 72);
const Color Color::Teal(0, 150, 136);

} // namespace myOS

