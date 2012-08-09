/*
 * magic.c
 * Copyright (c) 2012  Canonical LTD.
 * Author: Justin L Werner <justin.werner@caononical.com>
 * See: RFC 5071
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

#include <common.h>
#include <command.h>
#include <net.h>
#include "bootp.h"

#include <linux/compiler.h>

#if defined(CONFIG_BOOTP_VENDOREX) && defined(CONFIG_BOOTP_VENDOREX_PXE_SHARED)
#include "magic.h"

#define OPT_MAGIC            208
#define OPT_CONFIG_FILE      209
#define OPT_PATH_PREFIX      210
#define OPT_REBOOT_TIME      211

#define OPT_MAGIC_LEN        4
#define OPT_REBOOT_TIME_LEN  4


static const u8 opt_magic_string[] = {0xf1, 0x0, 0x74, 0x7e};

static int     opt_magic_seen       = 0;
static u8      opt_config_file_value[UCHAR_MAX + 1];
static u8      opt_path_prefix_value[UCHAR_MAX + 1];
static size_t  opt_reboot_time = 0;

static u8 *parse_magic(u8 *);
static u8 *parse_config_file(u8 *);
static u8 *parse_path_prefix(u8 *);
static u8 *parse_reboot_time(u8 *);


/*
 * Init/done processing any vendor extension options that we care
 * about here.  It is possible (at least in my env) for the PXE options
 * processing to occur more than once.
 */
void
dhcp_vendorex_opts_done(void)
{
	opt_magic_seen = 0;
	memset(opt_config_file_value, 0, sizeof(opt_config_file_value));
	memset(opt_path_prefix_value, 0, sizeof(opt_path_prefix_value));
	opt_reboot_time = 0;
}

/*
 * Add vendor option(s) to the request
 * - add own opts and return e as ptr just beyond them
 *
 * For our common PXE MAGIC implementation, we don't add anything: it's a noop
 */
u8 *
dhcp_vendorex_prep (u8 *e)
{
	return  e;
}

/*
 * Parse vendor options from a response
 * - e points into the bp_vend[] array, which is OPT_SIZE
 *   returns next 'e' if recognized and accepted, NULL otherwise
 */
u8 *
dhcp_vendorex_proc (u8 *e)
{
	switch (*e) {
	case OPT_MAGIC:
		printf("INFO: DHCP PXE vendorex PXE MAGIC option encountered\n");
		e = parse_magic(++e);
		break;

	case OPT_CONFIG_FILE:
		printf("INFO: DHCP PXE vendorex PXE ConfigFile option encountered\n");
		e = parse_config_file(++e);
		break;

	case OPT_PATH_PREFIX:
		printf("INFO: DHCP PXE vendorex PXE PathPrefix option encountered\n");
		e = parse_path_prefix(++e);
		break;

	case OPT_REBOOT_TIME:
		printf("INFO: DHCP PXE vendorex RebootTime option encountered\n");
		e = parse_reboot_time(++e);
		break;

	default:
		e = NULL;
		break;
	}

	return  e;
}

static u8 *
parse_magic(u8 *e)
{
	size_t len = *e++;

	if (opt_magic_seen) {
		printf("INFO: DHCP PXE vendorex MAGIC option already seen\n");
	}

	if (len != OPT_MAGIC_LEN) {
		e = NULL;
		printf("ERROR: DHCP PXE vendorex MAGIC has invalid length [%u]\n", len);

	} else if (memcmp(e, opt_magic_string, sizeof(opt_magic_string))) {
		e = NULL;
		opt_magic_seen = 0;
		printf("ERROR: DHCP PXE vendorex MAGIC invalid magic value %02x:%02x:%02x:%02x\n",
			(unsigned int) e[0], (unsigned int) e[1], (unsigned int) e[2], (unsigned int) e[3]);

	} else {
		e += sizeof(opt_magic_string);
		opt_magic_seen = 1;
		printf("INFO: DHCP PXE vendorex MAGIC confirmed\n");
	}

	return  e;
}

static u8 *
parse_config_file(u8 *e)
{
	size_t len = *e++;

	if (! opt_magic_seen) {
		printf("ERROR: DHCP PXE vendorex ConfigFile without MAGIC\n");
		e = NULL;

	} else if (len == 0) {
		printf("WARNING: DHCP PXE vendorex ConfigFile string has zero length, ignoring\n");

	} else {
		memset(opt_config_file_value, 0, sizeof(opt_config_file_value));
		memcpy(opt_config_file_value, e, len);
		e += len;
		opt_config_file_value[len] = 0;
		setenv(PXE_CONFIG_FILE, (char *)opt_config_file_value);
		printf("INFO: DHCP PXE vendorex ConfigFile is [%s]\n", opt_config_file_value);
	}

	return  e;
}

static u8 *
parse_path_prefix(u8 *e)
{
	size_t len = *e++;

	if (! opt_magic_seen) {
		printf("ERROR: DHCP PXE vendorex ConfigFile without MAGIC\n");
		e = NULL;

	} if (len == 0) {
		printf("WARNING: DHCP PXE vendorex PathPrefix string has zero length, ignoring\n");

	} else {
		memset(opt_path_prefix_value, 0, sizeof(opt_path_prefix_value));
		memcpy(opt_path_prefix_value, e, len);
		e += len;
		opt_path_prefix_value[len] = 0;
		setenv(PXE_PATH_PREFIX, (char *)opt_path_prefix_value);
		printf("INFO: DHCP PXE vendorex PathPrefix is [%s]\n", opt_path_prefix_value);
	}

	return  e;
}

static u8 *
parse_reboot_time(u8 *e)
{
	size_t len = *e++;
	char   bfr[12];

	if (! opt_magic_seen) {
		printf("ERROR: DHCP PXE vendorex ConfigFile without MAGIC\n");
		e = NULL;

	} else if (len != OPT_REBOOT_TIME_LEN) {
		e += len;
		printf("WARNING: DHCP PXE vendorex RebootTime has invalid length [%u], ignoring\n", len);

	} else {
		unsigned long  value;

		memcpy(&value, e, OPT_REBOOT_TIME_LEN);
		opt_reboot_time = (size_t) ntohl(value);
		e += OPT_REBOOT_TIME_LEN;

		sprintf(bfr, "%u", opt_reboot_time);
		setenv(PXE_REBOOT_TIME, bfr);

		printf("INFO: DHCP PXE vendorex RebootTime is [%u]\n", opt_reboot_time);

#if defined(CONFIG_BOOT_RETRY_TIME)
		if (opt_reboot_time < CONFIG_BOOT_RETRY_MIN) {
			opt_reboot_time = CONFIG_BOOT_RETRY_MIN;
			sprintf(bfr, "%u", opt_reboot_time);
			setenv(PXE_REBOOT_TIME, bfr);
			printf("INFO: DHCP PXE vendorex RebootTime adjusted up to allowable minimum value [%d]\n",
				CONFIG_BOOT_RETRY_MIN);
		}
		reset_cmd_timeout();
#else
		printf("INFO: DHCP PXE vendorex RebootTime is N/A and ignored due to u-boot configs\n");
#endif
	}

	return  e;
}
#else
const int no_arm_vendorex_options = 1;
#endif
