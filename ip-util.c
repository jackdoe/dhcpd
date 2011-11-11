#include "dhcpd.h"
#include "ip-util.h"
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
 
 * ip-util.c
 *
 * Copyright (c) 2002 Dug Song <dugsong@monkey.org>
 *
 * $Id: ip-util.c,v 1.9 2005/02/17 02:55:56 dugsong Exp $
 */

void ip_checksum(void *buf, size_t len) {
	struct ip *ip;
	int hl, off, sum;

	if (len < sizeof(struct ip))
		return;
	
	ip = (struct ip *)buf;
	hl = ip->ip_hl << 2;
	ip->ip_sum = 0;
	sum = ip_cksum_add(ip, hl, 0);
	ip->ip_sum = ip_cksum_carry(sum);

	off = htons(ip->ip_off);
	
	if ((off & IP_OFFMASK) != 0 || (off & IP_MF) != 0)
		return;
	
	len -= hl;
	
	if (ip->ip_p == IPPROTO_UDP) {
		struct udphdr *udp = (struct udphdr *)((u_char *)ip + hl);

		if (len >= sizeof(struct udphdr)) {
			udp->uh_sum = 0;
			sum = ip_cksum_add(udp, len, 0) + htons(ip->ip_p + len);
			sum = ip_cksum_add(&ip->ip_src, 8, sum);
			udp->uh_sum = ip_cksum_carry(sum);
			if (!udp->uh_sum)
				udp->uh_sum = 0xffff;	/* RFC 768 */
		}
	}
}

int ip_cksum_add(const void *buf, size_t len, int cksum)
{
	uint16_t *sp = (uint16_t *)buf;
	int n, sn;
	
	sn = len / 2;
	n = (sn + 15) / 16;

	/* XXX - unroll loop using Duff's device. */
	switch (sn % 16) {
	case 0:	do {
		cksum += *sp++;
	case 15:
		cksum += *sp++;
	case 14:
		cksum += *sp++;
	case 13:
		cksum += *sp++;
	case 12:
		cksum += *sp++;
	case 11:
		cksum += *sp++;
	case 10:
		cksum += *sp++;
	case 9:
		cksum += *sp++;
	case 8:
		cksum += *sp++;
	case 7:
		cksum += *sp++;
	case 6:
		cksum += *sp++;
	case 5:
		cksum += *sp++;
	case 4:
		cksum += *sp++;
	case 3:
		cksum += *sp++;
	case 2:
		cksum += *sp++;
	case 1:
		cksum += *sp++;
		} while (--n > 0);
	}
	if (len & 1)
		cksum += htons(*(u_char *)sp << 8);

	return (cksum);
}
