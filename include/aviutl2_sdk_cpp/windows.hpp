#pragma once

#ifndef NOMINMAX
#define NOMINMAX
#endif

#include <Windows.h>

#ifdef min
#error Detected Windows min macro. \
A header included before aviutl2_sdk_cpp included Windows.h without NOMINMAX. \
Define NOMINMAX globally or before any Windows SDK headers are included.
#endif

#ifdef max
#error Detected Windows max macro. \
A header included before aviutl2_sdk_cpp included Windows.h without NOMINMAX. \
Define NOMINMAX globally or before any Windows SDK headers are included.
#endif
