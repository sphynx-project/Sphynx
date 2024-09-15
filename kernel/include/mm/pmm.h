// This file is part of the Sphynx OS
// It is released under the MIT license -- see LICENSE
// Written by: Kevin Alavik.

#pragma once

#include <lib/std/types.h>

void PmmInitialize();
void *PmmRequestPages(usize numPages);
void PmmFreePages(void *ptr, usize numPages);