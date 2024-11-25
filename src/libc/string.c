#include <stdint.h>
#include <limits.h>
//EMAC:#include <assert.h>
#include <errno.h>
#include "libc/string.h"
#include "libc/printf.h"

#define wsize sizeof(word)
#define wmask (wsize - 1)

static const char* invalid_err = "No error information available.";

/*
 * sizeof(word) MUST BE A POWER OF TWO
 * SO THAT wmask BELOW IS ALL ONES
 */
typedef int word; /* "word" used for optimal copy speed */

#define E(a, b) ((unsigned char)a),
static const char errid[] = {
	#include "libc/__strerror.h"
};

static const char errmsg[] = {
	#include "libc/__strerror.h"
};

/*
 * Macros: loop-t-times; and loop-t-times, t>0
 */
// clang-format off
#define	TLOOP(s) if (t) TLOOP1(s)
#define	TLOOP1(s) do { s; } while (--t)

#define BITOP(a, b, op) \
	((a)[(size_t)(b) / (8 * sizeof *(a))] op(size_t) 1 << ((size_t)(b) % (8 * sizeof *(a))))

#define SS (sizeof(size_t))
#define ALIGN (sizeof(size_t))
#define ONES ((size_t)-1 / UCHAR_MAX)
#define HIGHS (ONES * (UCHAR_MAX / 2 + 1))
#define HASZERO(x) (((x)-ONES) & ~(x)&HIGHS)

static const unsigned long mask01 = 0x01010101;
static const unsigned long mask80 = 0x80808080;

#define LONGPTR_MASK (sizeof(long) - 1)

/*
 * Helper macro to return string length if we caught the zero
 * byte.
 */
#define testbyte(x)                                     \
	do                                                  \
	{                                                   \
		if(p[x] == '\0')                                \
			return ((uintptr_t)p - (uintptr_t)str + x); \
	} while(0)

/* === memcmp.c === */

int memcmp(const void* p1, const void* p2, size_t n)
{
	size_t i;

	/**
	 * p1 and p2 are the same memory? easy peasy! bail out
	 */
	if(p1 == p2)
	{
		return 0;
	}

	// This for loop does the comparing and pointer moving...
	for(i = 0; (i < n) && (*(const uint8_t*)p1 == *(const uint8_t*)p2);
		i++, p1 = 1 + (const uint8_t*)p1, p2 = 1 + (const uint8_t*)p2)
	{
		// empty body
	}

	// if i == length, then we have passed the test
	return (i == n) ? 0 : (*(const uint8_t*)p1 - *(const uint8_t*)p2);
}

/* === memset.c === */

// MUSL memset implementation:
// https://github.com/esmil/musl/blob/master/src/string/memset.c

void* memset(void* dest, int c, size_t n)
{
	unsigned char* s = dest;
	size_t k;

	/* Fill head and tail with minimal branching. Each
	 * conditional ensures that all the subsequently used
	 * offsets are well-defined and in the dest region. */

	if(!n)
	{
		return dest;
	}

	s[0] = s[n - 1] = (unsigned char)c;

	if(n <= 2)
	{
		return dest;
	}

	s[1] = s[n - 2] = (unsigned char)c;
	s[2] = s[n - 3] = (unsigned char)c;

	if(n <= 6)
	{
		return dest;
	}

	s[3] = s[n - 4] = (unsigned char)c;

	if(n <= 8)
	{
		return dest;
	}

	/* Advance pointer to align it at a 4-byte boundary,
	 * and truncate n to a multiple of 4. The previous code
	 * already took care of any head/tail that get cut off
	 * by the alignment. */

	k = -(uintptr_t)s & 3;
	s += k;
	n -= k;
	n &= (unsigned long)-4;
	n /= 4;

	// Cast to void first to prevent alignment warning
	uint32_t* ws = (uint32_t*)(void*)s;
	uint32_t wc = c & 0xFF;
	wc |= ((wc << 8) | (wc << 16) | (wc << 24));

	/* Pure C fallback with no aliasing violations. */
	for(; n; n--, ws++)
	{
		*ws = wc;
	}

	return dest;
}

/* === memcpy.c === */

/*-
 * Copyright (c) 1990, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * This code is derived from software contributed to Berkeley by
 * Chris Torek.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

/*
 * Copy a block of memory, handling overlap.
 * This is the routine that actually implements
 * (the portable versions of) bcopy, memcpy, and memmove.
 */
void* memcpy(void* __restrict dst0, const void* __restrict src0, size_t length)
{
	char* dst = dst0;
	const char* src = src0;
	size_t t;

	if(length == 0 || dst == src)
	{
		/* nothing to do */
		goto done;
	}

	// clang-format on

	if((uintptr_t)dst < (uintptr_t)src)
	{
		/*
		 * Copy forward.
		 */
		t = (uintptr_t)src; /* only need low bits */
		if((t | (uintptr_t)dst) & wmask)
		{
			/*
			 * Try to align operands.  This cannot be done
			 * unless the low bits match.
			 */
			if((t ^ (uintptr_t)dst) & wmask || length < wsize)
			{
				t = length;
			}
			else
			{
				t = wsize - (t & wmask);
			}
			length -= t;
			TLOOP1(*dst++ = *src++);
		}
		/*
		 * Copy whole words, then mop up any trailing bytes.
		 */
		t = length / wsize;
		// Silence warning for alignment change by casting to void*
		TLOOP(*(word*)(void*)dst = *(const word*)(const void*)src; src += wsize; dst += wsize);
		t = length & wmask;
		TLOOP(*dst++ = *src++);
	}
	else
	{
		/*
		 * Copy backwards.  Otherwise essentially the same.
		 * Alignment works as before, except that it takes
		 * (t&wmask) bytes to align, not wsize-(t&wmask).
		 */
		src += length;
		dst += length;
		t = (uintptr_t)src;

		if((t | (uintptr_t)dst) & wmask)
		{
			if((t ^ (uintptr_t)dst) & wmask || length <= wsize)
			{
				t = length;
			}
			else
			{
				t &= wmask;
			}

			length -= t;
			TLOOP1(*--dst = *--src);
		}

		t = length / wsize;
		// Silence warning for alignment change by casting to void*
		TLOOP(src -= wsize; dst -= wsize; *(word*)(void*)dst = *(const word*)(const void*)src);
		t = length & wmask;
		TLOOP(*--dst = *--src);
	}
done:
	return (dst0);
}

/* === memmove.c === */

void* memmove(void* s1, const void* s2, size_t n)
{
	return memcpy(s1, s2, n);
}

/* === memrchr.c === */

// Imported from musl Libc

void* __memrchr(const void* /*m*/ /*m*/, int /*c*/ /*c*/, size_t /*n*/ /*n*/);

void* __memrchr(const void* m, int c, size_t n)
{
	const unsigned char* s = m;
	c = (unsigned char)c;

	while(n--)
	{
		if(s[n] == c)
		{
			return (void*)(uintptr_t)(s + n);
		}
	}

	return 0;
}


void* memchr(const void* src, int c, size_t n)
{
	const unsigned char* s = src;
	c = (unsigned char)c;

#ifdef __GNUC__
	for(; ((uintptr_t)s & ALIGN) && n && *s != c; s++, n--)
	{
	}

	if(n && *s != c)
	{
		typedef size_t __attribute__((__may_alias__)) word;
		const word* w;
		size_t k = ONES * (size_t)c;

		for(w = (const void*)s; n >= SS && !HASZERO(*w ^ k); w++, n -= SS)
		{
		}

		s = (const void*)w;
	}
#endif

	for(; n && *s != c; s++, n--)
	{
	}

	return n ? (void*)(uintptr_t)s : 0;
}

/* === memmem.c === */

/*-
 * Copyright (c) 2005 Pascal Gloor <pascal.gloor@spale.com>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote
 *    products derived from this software without specific prior written
 *    permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

/*
 * Find the first occurrence of the byte string s in byte string l.
 */
void* memmem(const void* l, size_t l_len, const void* s, size_t s_len)
{
	const char* cur;
	const char* last;
	const char* cl = (const char*)l;
	const char* cs = (const char*)s;

	/* we need something to compare */
	if(l_len == 0 || s_len == 0)
	{
		return NULL;
	}

	/* "s" must be smaller or equal to "l" */
	if(l_len < s_len)
	{
		return NULL;
	}

	/* special case where s_len == 1 */
	if(s_len == 1)
	{
		return memchr(l, (int)*cs, l_len);
	}

	/* the last position where its possible to find "s" in "l" */
	last = cl + l_len - s_len;

	for(cur = cl; cur <= last; cur++)
	{
		if(cur[0] == cs[0] && memcmp(cur, cs, s_len) == 0)
		{
			return (void*)(uintptr_t)cur;
		}
	}

	return NULL;
}

/* === strlen.c === */

/*-
 * Copyright (c) 2009 Xin LI <delphij@FreeBSD.org>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

/*
 * Portable strlen() for 32-bit and 64-bit systems.
 *
 * Rationale: it is generally much more efficient to do word length
 * operations and avoid branches on modern computer systems, as
 * compared to byte-length operations with a lot of branches.
 *
 * The expression:
 *
 *	((x - 0x01....01) & ~x & 0x80....80)
 *
 * would evaluate to a non-zero value iff any of the bytes in the
 * original word is zero.  However, we can further reduce ~1/3 of
 * time if we consider that strlen() usually operate on 7-bit ASCII
 * by employing the following expression, which allows false positive
 * when high bit of 1 and use the tail case to catch these case:
 *
 *	((x - 0x01....01) & 0x80....80)
 *
 * This is more than 5.2 times as fast as the raw implementation on
 * Intel T7300 under long mode for strings longer than word length.
 */

/* Magic numbers for the algorithm */

size_t strlen(const char* str)
{
	const char* p;
	const unsigned long* lp;

	/* Skip the first few bytes until we have an aligned p */
	for(p = str; (uintptr_t)p & LONGPTR_MASK; p++)
	{
		if(*p == '\0')
		{
			return ((uintptr_t)p - (uintptr_t)str);
		}
	}

	/* Scan the rest of the string using word sized operation */
	// Cast to void to prevent alignment warning
	for(lp = (const unsigned long*)(const void*)p;; lp++)
	{
		if((*lp - mask01) & mask80)
		{
			p = (const char*)(lp);
			testbyte(0);
			testbyte(1);
			testbyte(2);
			testbyte(3);
#if(LONG_BIT >= 64)
			testbyte(4);
			testbyte(5);
			testbyte(6);
			testbyte(7);
#endif
		}
	}

	/* NOTREACHED */
	// return (0);
}

/* === strnlen.c === */

/*
 * Copyright (c) 2010 Todd C. Miller <Todd.Miller@courtesan.com>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

/*	$OpenBSD: strnlen.c,v 1.6 2015/08/31 02:53:57 guenther Exp $	*/

size_t strnlen(const char* str, size_t maxlen)
{
	const char* cp;

	for(cp = str; maxlen != 0 && *cp != '\0'; cp++, maxlen--)
	{
		;
	}

	return (size_t)(cp - str);
}

/* === strcpy.c === */

/*
 * Copyright (c) 2011 Apple, Inc. All rights reserved.
 *
 * @APPLE_LICENSE_HEADER_START@
 *
 * This file contains Original Code and/or Modifications of Original Code
 * as defined in and that are subject to the Apple Public Source License
 * Version 2.0 (the 'License'). You may not use this file except in
 * compliance with the License. Please obtain a copy of the License at
 * http://www.opensource.apple.com/apsl/ and read it before using this
 * file.
 *
 * The Original Code and all software distributed under the License are
 * distributed on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER
 * EXPRESS OR IMPLIED, AND APPLE HEREBY DISCLAIMS ALL SUCH WARRANTIES,
 * INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT OR NON-INFRINGEMENT.
 * Please see the License for the specific language governing rights and
 * limitations under the License.
 *
 * @APPLE_LICENSE_HEADER_END@
 */

char* strcpy(char* __restrict dst, const char* __restrict src)
{
	const size_t length = strlen(src);
	//  The stpcpy() and strcpy() functions copy the string src to dst
	//  (including the terminating '\0' character).
	memcpy(dst, src, length + 1);
	//  The strcpy() and strncpy() functions return dst.
	return dst;
}

/* === strncpy.c === */

/*
 * Copyright (c) 2011 Apple, Inc. All rights reserved.
 *
 * @APPLE_LICENSE_HEADER_START@
 *
 * This file contains Original Code and/or Modifications of Original Code
 * as defined in and that are subject to the Apple Public Source License
 * Version 2.0 (the 'License'). You may not use this file except in
 * compliance with the License. Please obtain a copy of the License at
 * http://www.opensource.apple.com/apsl/ and read it before using this
 * file.
 *
 * The Original Code and all software distributed under the License are
 * distributed on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER
 * EXPRESS OR IMPLIED, AND APPLE HEREBY DISCLAIMS ALL SUCH WARRANTIES,
 * INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT OR NON-INFRINGEMENT.
 * Please see the License for the specific language governing rights and
 * limitations under the License.
 *
 * @APPLE_LICENSE_HEADER_END@
 */

#include <string.h>

char* strncpy(char* __restrict dst, const char* __restrict src, size_t maxlen)
{
	const size_t srclen = strnlen(src, maxlen);
	if(srclen < maxlen)
	{
		//  The stpncpy() and strncpy() functions copy at most maxlen
		//  characters from src into dst.
		memcpy(dst, src, srclen);
		//  If src is less than maxlen characters long, the remainder
		//  of dst is filled with '\0' characters.
		memset(dst + srclen, 0, maxlen - srclen);
	}
	else
	{
		//  Otherwise, dst is not terminated.
		memcpy(dst, src, maxlen);
	}

	//  The strcpy() and strncpy() functions return dst.
	return dst;
}

/* === strstr.c === */

/*
 * strstr.c --
 *
 *	Source code for the "strstr" library routine.
 *
 * Copyright (c) 1988-1993 The Regents of the University of California.
 * Copyright (c) 1994 Sun Microsystems, Inc.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 * RCS: @(#) $Id: strstr.c,v 1.1.1.3 2003/03/06 00:09:04 landonf Exp $
 */

/*
 *----------------------------------------------------------------------
 *
 * strstr --
 *
 *	Locate the first instance of a substring in a string.
 *
 * Results:
 *	If string contains substring, the return value is the
 *	location of the first matching instance of substring
 *	in string.  If string doesn't contain substring, the
 *	return value is 0.  Matching is done on an exact
 *	character-for-character basis with no wildcards or special
 *	characters.
 *
 * Side effects:
 *	None.
 *
 *----------------------------------------------------------------------
 */

#include <string.h>

char* strstr(const char* string, const char* substring)
{
	const char* a;
	const char* b;

	/* First scan quickly through the two strings looking for a
	 * single-character match.  When it's found, then compare the
	 * rest of the substring.
	 */

	b = substring;

	if(*b == 0)
	{
		return (char*)(uintptr_t)string;
	}

	for(; *string != 0; string += 1)
	{
		if(*string != *b)
		{
			continue;
		}

		a = string;

		while(1)
		{
			if(*b == 0)
			{
				return (char*)(uintptr_t)string;
			}

			if(*a++ != *b++)
			{
				break;
			}
		}

		b = substring;
	}

	return NULL;
}

/* === strnstr.c === */

/*-
 * Copyright (c) 2001 Mike Barcroft <mike@FreeBSD.org>
 * Copyright (c) 1990, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * This code is derived from software contributed to Berkeley by
 * Chris Torek.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#if 0 // PJ: not needed
#if defined(LIBC_SCCS) && !defined(lint)
static char sccsid[] = "@(#)strstr.c	8.1 (Berkeley) 6/4/93";
#endif /* LIBC_SCCS and not lint */
#include <sys/cdefs.h>
__FBSDID("$FreeBSD: src/lib/libc/string/strnstr.c,v 1.5 2009/02/03 17:58:20 danger Exp $");
#endif

#include <string.h>

/*
 * Find the first occurrence of find in s, where the search is limited to the
 * first slen characters of s.
 */
char* strnstr(const char* s, const char* find, size_t slen)
{
	char c;

	if((c = *find++) != '\0')
	{
		size_t len = strlen(find);
		do
		{
			char sc;
			do
			{
				if(slen-- < 1 || (sc = *s++) == '\0')
				{
					return (NULL);
				}
			} while(sc != c);
			if(len > slen)
			{
				return (NULL);
			}
		} while(strncmp(s, find, len) != 0);
		s--;
	}
	return ((char*)(uintptr_t)s);
}

/* === strchrnul.c === */

char* __strchrnul(const char* s, int c)
{
	const size_t* w;
	size_t k;
	c = (unsigned char)c;

	if(!c)
	{
		return (char*)(uintptr_t)s + strlen(s);
	}

	for(; (uintptr_t)s % ALIGN; s++)
	{
		if(!*s || *(const unsigned char*)s == c)
		{
			return (char*)(uintptr_t)s;
		}
	}

	k = ONES * (unsigned long)c;

	for(w = (const void*)s; !HASZERO(*w) && !HASZERO(*w ^ k); w++)
	{
		;
	}
	for(s = (const void*)w; *s && *(const unsigned char*)s != c; s++)
	{
		;
	}

	return (char*)(uintptr_t)s;
}

/* === strcspn.c === */

size_t strcspn(const char* s, const char* c)
{
	const char* a = s;
	size_t byteset[32 / sizeof(size_t)];

	if(!c[0] || !c[1])
	{
		return (uintptr_t)(__strchrnul(s, *c) - a);
	}

	memset(byteset, 0, sizeof byteset);
	for(; *c && BITOP(byteset, *(const unsigned char*)c, |=); c++)
	{
		{
			;
		}
	}
	for(; *s && !BITOP(byteset, *(const unsigned char*)s, &); s++)
	{
		{
			;
		}
	}
	return (uintptr_t)s - (uintptr_t)a;
}

/* === strncmp.c === */

/*
 * PJ: my own strncmp implementation
 *
 * strncmp with short-circuit support: very common when you have const strings
 * combined by the compiler.
 * Otherwise we compare the strings as normal
 * We bail out when s1 ends (null-term) or n bytes have been analyzed
 */

int strncmp(const char* s1, const char* s2, size_t n)
{
	int r = -1;

	if(s1 == s2)
	{
		// short circuit - same string
		return 0;
	}

	// iterate through strings until they don't match, s1 ends, or n == 0
	for(; n && *s1 == *s2; ++s1, ++s2, n--)
	{
		if(*s1 == 0)
		{
			r = 0;
			break;
		}
	}

	// handle case where we didn't break early - set return code.
	if(n == 0)
	{
		r = 0;
	}
	else if(r != 0)
	{
		r = *s1 - *s2;
	}

	return r;
}

/* === strdup.c === */

/*
 * Copyright (c) 1988, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#if 0
#if defined(LIBC_SCCS) && !defined(lint)
static char sccsid[] = "@(#)strdup.c	8.1 (Berkeley) 6/4/93";
#endif /* LIBC_SCCS and not lint */
#include <sys/cdefs.h>
__FBSDID("$FreeBSD: src/lib/libc/string/strdup.c,v 1.6 2009/02/03 17:58:20 danger Exp $");
#endif

char* strdup(const char* str)
{
	char* copy = NULL;

	/*if(str)
	{
		size_t len = strlen(str) + 1;

		if((copy = malloc(len)) == NULL)
		{
			return (NULL);
		}

		memcpy(copy, str, len);
	}*/

	return (copy);
}

/* === strndup.c === */

/*      $NetBSD: strndup.c,v 1.3 2007/01/14 23:41:24 cbiere Exp $       */

/*
 * Copyright (c) 1988, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#if 0
#include <sys/cdefs.h>
__FBSDID("$FreeBSD: src/lib/libc/string/strndup.c,v 1.1 2008/12/06 09:37:54 kib Exp $");
#endif

char* strndup(const char* str, size_t n)
{
	char* copy = NULL;

	/*if(str && n)
	{
		size_t len;

		for(len = 0; len < n && str[len]; len++)
		{
		}

		if((copy = malloc(len + 1)) == NULL)
		{
			return (NULL);
		}

		memcpy(copy, str, len);
		copy[len] = '\0';
	}*/

	return copy;
}

/* === strchr.c === */

// Imported from musl Libc

char* strchr(const char* s, int c)
{
	char* r = __strchrnul(s, c);
	return *(unsigned char*)r == (unsigned char)c ? r : 0;
}

/* === strrchr.c === */

// Imported from musl Libc

extern void* __memrchr(const void*, int, size_t);

char* strrchr(const char* s, int c)
{
	return __memrchr(s, c, strlen(s) + 1);
}

/* === strcat.c === */

/*
 * Copyright (c) 2011 Apple, Inc. All rights reserved.
 *
 * @APPLE_LICENSE_HEADER_START@
 *
 * This file contains Original Code and/or Modifications of Original Code
 * as defined in and that are subject to the Apple Public Source License
 * Version 2.0 (the 'License'). You may not use this file except in
 * compliance with the License. Please obtain a copy of the License at
 * http://www.opensource.apple.com/apsl/ and read it before using this
 * file.
 *
 * The Original Code and all software distributed under the License are
 * distributed on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER
 * EXPRESS OR IMPLIED, AND APPLE HEREBY DISCLAIMS ALL SUCH WARRANTIES,
 * INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT OR NON-INFRINGEMENT.
 * Please see the License for the specific language governing rights and
 * limitations under the License.
 *
 * @APPLE_LICENSE_HEADER_END@
 */

char* strcat(char* __restrict dst, const char* __restrict src)
{
	const size_t dstlen = strlen(dst);
	const size_t srclen = strlen(src);
	//  The strcat() and strncat() functions append a copy of the null-
	//  terminated string src to the end of the null-terminated string dst,
	//  then add a terminating '\0'.  The string dst must have sufficient
	//  space to hold the result.
	memcpy(dst + dstlen, src, srclen + 1);
	//  The strcat() and strncat() functions return dst.
	return dst;
}

/* === strncat.c === */

/*
 * Copyright (c) 2011 Apple, Inc. All rights reserved.
 *
 * @APPLE_LICENSE_HEADER_START@
 *
 * This file contains Original Code and/or Modifications of Original Code
 * as defined in and that are subject to the Apple Public Source License
 * Version 2.0 (the 'License'). You may not use this file except in
 * compliance with the License. Please obtain a copy of the License at
 * http://www.opensource.apple.com/apsl/ and read it before using this
 * file.
 *
 * The Original Code and all software distributed under the License are
 * distributed on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER
 * EXPRESS OR IMPLIED, AND APPLE HEREBY DISCLAIMS ALL SUCH WARRANTIES,
 * INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT OR NON-INFRINGEMENT.
 * Please see the License for the specific language governing rights and
 * limitations under the License.
 *
 * @APPLE_LICENSE_HEADER_END@
 */

char* strncat(char* __restrict dst, const char* __restrict src, size_t maxlen)
{
	const size_t dstlen = strlen(dst);
	const size_t srclen = strnlen(src, maxlen);
	//  The strcat() and strncat() functions append a copy of the null-
	//  terminated string src to the end of the null-terminated string dst,
	//  then add a terminating '\0'.  The string dst must have sufficient
	//  space to hold the result.
	//
	//  The strncat() function appends not more than maxlen characters
	//  from src, and then adds a terminating '\0'.
	const size_t cpylen = srclen < maxlen ? srclen : maxlen;
	memcpy(dst + dstlen, src, cpylen);
	dst[dstlen + cpylen] = '\0';
	//  The strcat() and strncat() functions return dst.
	return dst;
}

/* === strtok.c === */

/*-
 * Copyright (c) 1998 Softweyr LLC.  All rights reserved.
 *
 * strtok_r, from Berkeley strtok
 * Oct 13, 1998 by Wes Peters <wes@softweyr.com>
 *
 * Copyright (c) 1988, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notices, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notices, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY SOFTWEYR LLC, THE REGENTS AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL SOFTWEYR LLC, THE
 * REGENTS, OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
 * TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

char* __strtok_r(char* /*s*/ /*s*/, const char* /*delim*/ /*delim*/, char** /*last*/ /*last*/);

char* __strtok_r(char* s, const char* delim, char** last)
{
	char* spanp;
	char* tok;
	int c;
	int sc;

	if(s == NULL && (s = *last) == NULL)
	{
		return (NULL);
	}

/*
 * Skip (span) leading delimiters (s += strspn(s, delim), sort of).
 */
cont:
	c = *s++;
	for(spanp = (char*)(uintptr_t)delim; (sc = *spanp++) != 0;)
	{
		if(c == sc)
		{
			goto cont;
		}
	}

	if(c == 0)
	{ /* no non-delimiter characters */
		*last = NULL;
		return (NULL);
	}
	tok = s - 1;

	/*
	 * Scan token (scan for delimiters: s += strcspn(s, delim), sort of).
	 * Note that delim must have one NUL; we stop if we see that, too.
	 */
	for(;;)
	{
		c = *s++;
		spanp = (char*)(uintptr_t)delim;
		do
		{
			if((sc = *spanp++) == c)
			{
				if(c == 0)
				{
					s = NULL;
				}
				else
				{
					s[-1] = '\0';
				}
				*last = s;
				return (tok);
			}
		} while(sc != 0);
	}
	/* NOTREACHED */
}

char* strtok(char* s, const char* delim)
{
	static char* last;

	return (__strtok_r(s, delim, &last));
}

/* === strxfrm.c === */

size_t strxfrm(char* restrict dest, const char* restrict src, size_t n)
{
	size_t l = strlen(src);
	if(n > l)
	{
		strcpy(dest, src);
	}

	return l;
}

/* === strspn.c === */

size_t strspn(const char* s, const char* c)
{
	const char* a = s;
	size_t byteset[32 / sizeof(size_t)] = {0};

	if(!c[0])
	{
		return 0;
	}

	if(!c[1])
	{
		for(; *s == *c; s++)
		{
			;
		}

		return (uintptr_t)s - (uintptr_t)a;
	}

	for(; *c && BITOP(byteset, *(const unsigned char*)c, |=); c++)
	{
		;
	}

	for(; *s && BITOP(byteset, *(const unsigned char*)s, &); s++)
	{
		;
	}

	return (uintptr_t)s - (uintptr_t)a;
}

/* === strpbrk.c === */

char* strpbrk(const char* s, const char* b)
{
	s += strcspn(s, b);
	return *s ? (char*)(uintptr_t)s : 0;
}

/* === strcoll.c === */

int strcoll(const char* l, const char* r)
{
	return strcmp(l, r);
}

/* === strcmp.c === */

/*
 * PJ: my own strcmp implementation
 *
 * strcmp with short-circuit support: very common when you have const strings
 * combined by the compiler.
 * Otherwise we compare the strings as normal.
 * We bail out when s1 ends (null-term)
 */

int strcmp(const char* s1, const char* s2)
{
	int r = -1;

	if(s1 == s2)
	{
		// short circuit - same string
		return 0;
	}

	// iterate through strings until they don't match or s1 ends (null-term)
	for(; *s1 == *s2; ++s1, ++s2)
	{
		if(*s1 == 0)
		{
			r = 0;
			break;
		}
	}

	// handle case where we didn't break early - set return code.
	if(r != 0)
	{
		r = *(const char*)s1 - *(const char*)s2;
	}

	return r;
}

/* === strerror.c === */

// Modeled after musl libc with the use of the errid[] and errmsg[] trick

char* strerror(int err_no)
{
	const char* s = NULL;
	int i;

	for(i = 0; errid[i] && errid[i] != err_no; i++)
	{
	}

	if(errid[i] != err_no)
	{
		s = invalid_err;
	}
	else
	{
		for(s = errmsg; i; s++, i--)
		{
			for(; *s; s++)
			{
			}
		}
	}

	return (char*)(uintptr_t)s;
}

/* === strerror_r.c === */

int strerror_r(int err_no, char* buffer, size_t buffer_size)
{
	int r = 0;
	char* err_msg = strerror(err_no);
	size_t length = strlen(err_msg);

	//EMAC:assert(buffer);

	if(length >= buffer_size)
	{
		if(buffer_size)
		{
			// -1 so we don't copy an extra byte...
			memcpy(buffer, err_msg, buffer_size - 1);
			// since we will null terminate the string
			buffer[buffer_size - 1] = 0;
		}

		r = ERANGE;
	}
	else
	{
		// +1 for null termination
		memcpy(buffer, err_msg, length + 1);
	}

	return r;
}