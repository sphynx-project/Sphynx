// This file is part of the Sphynx OS
// It is released under the MIT license -- see LICENSE
// Written by: Kevin Alavik.

#include <stdlib.h>
#include <syscall.h>

void exit(u8 exitCode)
{
	sys_exit(exitCode);
}