#pragma once

#include <windef.h>

VOID* FindPattern( VOID* ImageBase, UINT64 ImageSize, const UINT8* Pattern, UINT64 PatternSize );
