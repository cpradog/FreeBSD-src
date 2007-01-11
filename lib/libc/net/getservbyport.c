/*
 * Copyright (c) 1983, 1993
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
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *	This product includes software developed by the University of
 *	California, Berkeley and its contributors.
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

#if defined(LIBC_SCCS) && !defined(lint)
static char sccsid[] = "@(#)getservbyport.c	8.1 (Berkeley) 6/4/93";
#endif /* LIBC_SCCS and not lint */
#include <sys/cdefs.h>
__FBSDID("$FreeBSD: src/lib/libc/net/getservbyport.c,v 1.7.2.1 2006/07/17 10:09:57 ume Exp $");

#include <netdb.h>
#include <string.h>
#include "netdb_private.h"

int
getservbyport_r(int port, const char *proto, struct servent *sptr,
    char *buffer, size_t buflen, struct servent **result)
{
	struct servent se;
	struct servent_data *sed;
	int error;

	if ((sed = __servent_data_init()) == NULL)
		return (-1);

#ifdef YP
	sed->yp_port = port;
	sed->yp_proto = (char *)proto;
#endif

	__setservent_p(sed->stayopen, sed);
	while ((error = __getservent_p(&se, sed)) == 0) {
		if (se.s_port != port)
			continue;
		if (proto == 0 || strcmp(se.s_proto, proto) == 0)
			break;
	}
	if (!sed->stayopen)
		__endservent_p(sed);

#ifdef YP
	sed->yp_port = 0;
	sed->yp_proto = NULL;
#endif

	if (error != 0)
		return (-1);
	if (__copy_servent(&se, sptr, buffer, buflen) != 0)
		return (-1);
	*result = sptr;
	return (0);
}

struct servent *
getservbyport(int port, const char *proto)
{
	struct servdata *sd;
	struct servent *rval;

	if ((sd = __servdata_init()) == NULL)
		return (NULL);
	if (getservbyport_r(port, proto, &sd->serv, sd->data,
	    sizeof(sd->data), &rval) != 0)
		return (NULL);
	return (rval);
}
