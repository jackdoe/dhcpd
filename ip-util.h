
/*
 Copyright (c) 2000-2006 Dug Song <dugsong@monkey.org>
 All rights reserved, all wrongs reversed.
 
 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions
 are met:
 
 1. Redistributions of source code must retain the above copyright
 notice, this list of conditions and the following disclaimer.
 2. Redistributions in binary form must reproduce the above copyright
 notice, this list of conditions and the following disclaimer in the
 documentation and/or other materials provided with the distribution.
 3. The names of the authors and copyright holders may not be used to
 endorse or promote products derived from this software without
 specific prior written permission.
 
 THIS SOFTWARE IS PROVIDED ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES,
 INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY
 AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
 THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * ip.h
 *
 * Internet Protocol (RFC 791).
 *
 * Copyright (c) 2000 Dug Song <dugsong@monkey.org>
 *
 * $Id: ip.h,v 1.25 2005/02/16 22:02:45 dugsong Exp $
 */

/*
 * udp.h
 *
 * User Datagram Protocol (RFC 768).
 *
 * Copyright (c) 2000 Dug Song <dugsong@monkey.org>
 *
 * $Id: udp.h,v 1.8 2002/04/02 05:05:39 dugsong Exp $
 */
#define	 ip_cksum_carry(x) \
	    (x = (x >> 16) + (x & 0xffff), (~(x + (x >> 16)) & 0xffff))

void ip_checksum(void *buf, size_t len);
int ip_cksum_add(const void *buf, size_t len, int cksum);
