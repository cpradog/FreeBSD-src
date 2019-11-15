/*-
 * SPDX-License-Identifier: BSD-2-Clause-FreeBSD
 *
 * Copyright (c) 2019 Mitchell Horne <mhorne@FreeBSD.org>
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

#include <sys/cdefs.h>
__FBSDID("$FreeBSD$");

#include <sys/param.h>
#include <sys/systm.h>
#include <sys/types.h>

#include <machine/md_var.h>
#include <machine/sbi.h>

u_long sbi_spec_version;
u_long sbi_impl_id;
u_long sbi_impl_version;

static struct sbi_ret
sbi_get_spec_version(void)
{
	return (SBI_CALL0(SBI_EXT_ID_BASE, SBI_BASE_GET_SPEC_VERSION));
}

static struct sbi_ret
sbi_get_impl_id(void)
{
	return (SBI_CALL0(SBI_EXT_ID_BASE, SBI_BASE_GET_IMPL_ID));
}

static struct sbi_ret
sbi_get_impl_version(void)
{
	return (SBI_CALL0(SBI_EXT_ID_BASE, SBI_BASE_GET_IMPL_VERSION));
}

static struct sbi_ret
sbi_get_mvendorid(void)
{
	return (SBI_CALL0(SBI_EXT_ID_BASE, SBI_BASE_GET_MVENDORID));
}


static struct sbi_ret
sbi_get_marchid(void)
{
	return (SBI_CALL0(SBI_EXT_ID_BASE, SBI_BASE_GET_MARCHID));
}

static struct sbi_ret
sbi_get_mimpid(void)
{
	return (SBI_CALL0(SBI_EXT_ID_BASE, SBI_BASE_GET_MIMPID));
}

void
sbi_init(void)
{
	struct sbi_ret sret;

	/*
	 * Get the spec version. For legacy SBI implementations this will
	 * return an error, otherwise it is guaranteed to succeed.
	 */
	sret = sbi_get_spec_version();
	if (sret.error != 0) {
		/* We are running a legacy SBI implementation. */
		sbi_spec_version = 0;
		return;
	}

	/* Set the SBI implementation info. */
	sbi_spec_version = sret.value;
	sbi_impl_id = sbi_get_impl_id().value;
	sbi_impl_version = sbi_get_impl_version().value;

	/* Set the hardware implementation info. */
	mvendorid = sbi_get_mvendorid().value;
	marchid = sbi_get_marchid().value;
	mimpid = sbi_get_mimpid().value;

	/*
	 * Probe for legacy extensions. Currently we rely on all of them
	 * to be implemented, but this is not guaranteed by the spec.
	 */
	KASSERT(sbi_probe_extension(SBI_SET_TIMER) != 0,
	    ("SBI doesn't implement sbi_set_timer()"));
	KASSERT(sbi_probe_extension(SBI_CONSOLE_PUTCHAR) != 0,
	    ("SBI doesn't implement sbi_console_putchar()"));
	KASSERT(sbi_probe_extension(SBI_CONSOLE_GETCHAR) != 0,
	    ("SBI doesn't implement sbi_console_getchar()"));
	KASSERT(sbi_probe_extension(SBI_CLEAR_IPI) != 0,
	    ("SBI doesn't implement sbi_clear_ipi()"));
	KASSERT(sbi_probe_extension(SBI_SEND_IPI) != 0,
	    ("SBI doesn't implement sbi_send_ipi()"));
	KASSERT(sbi_probe_extension(SBI_REMOTE_FENCE_I) != 0,
	    ("SBI doesn't implement sbi_remote_fence_i()"));
	KASSERT(sbi_probe_extension(SBI_REMOTE_SFENCE_VMA) != 0,
	    ("SBI doesn't implement sbi_remote_sfence_vma()"));
	KASSERT(sbi_probe_extension(SBI_REMOTE_SFENCE_VMA_ASID) != 0,
	    ("SBI doesn't implement sbi_remote_sfence_vma_asid()"));
	KASSERT(sbi_probe_extension(SBI_SHUTDOWN) != 0,
	    ("SBI doesn't implement sbi_shutdown()"));
}
