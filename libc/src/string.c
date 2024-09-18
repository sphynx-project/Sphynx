// This file is part of the Sphynx OS
// It is released under the MIT license -- see LICENSE
// Written by: Kevin Alavik.

#include <stddef.h>
#include <string.h>

void *memcpy(void *dest, const void *src, size_t n)
{
	unsigned char *d = dest;
	const unsigned char *s = src;
	while (n--) {
		*d++ = *s++;
	}
	return dest;
}

void *memset(void *s, int c, size_t n)
{
	unsigned char *p = s;
	while (n--) {
		*p++ = (unsigned char)c;
	}
	return s;
}

size_t strlen(const char *s)
{
	const char *p = s;
	while (*p) {
		p++;
	}
	return p - s;
}

int strcmp(const char *s1, const char *s2)
{
	while (*s1 && (*s1 == *s2)) {
		s1++;
		s2++;
	}
	return *(unsigned char *)s1 - *(unsigned char *)s2;
}

char *strcpy(char *dest, const char *src)
{
	char *d = dest;
	while ((*d++ = *src++)) {
		;
	}
	return dest;
}

char *strcat(char *dest, const char *src)
{
	char *d = dest;
	while (*d) {
		d++;
	}
	while ((*d++ = *src++)) {
		;
	}
	return dest;
}
