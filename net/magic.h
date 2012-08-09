/*
 * magic.h
 * Copyright (c) 2012  Canonical LTD.
 * Author: Justin L Werner <justin.werner@caononical.com>
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 *
 */
#ifndef _NET_MAGIC_H
#define _NET_MAGIC_H

#if defined(CONFIG_BOOTP_VENDOREX) && defined(CONFIG_BOOTP_VENDOREX_PXE_SHARED)
#ifndef UCHAR_MAX
# define UCHAR_MAX ((size_t)0xff)
#endif

#define PXE_CONFIG_FILE  "pxeconfigfile"
#define PXE_PATH_PREFIX  "pxepathprefix"  /* see "bootfile" in common/cmd_pxe.c */
#define PXE_REBOOT_TIME  "pxereboottime"

extern void dhcp_vendorex_opts_done(void);

#endif

#endif
