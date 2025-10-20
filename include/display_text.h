#pragma once

#include <Arduino.h>
#include <pgmspace.h>

// Font data and text rendering utilities
namespace DisplayText {

// 5x7 font table adapted from the Adafruit GFX library (MIT License).
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
extern const uint8_t kFont5x7[95][5] PROGMEM;
extern const size_t kFontGlyphCount;

}  // namespace DisplayText
