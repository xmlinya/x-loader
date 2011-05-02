/*
 * Copyright 2010-2011 Calxeda, Inc.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU General Public License,
 * version 2, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include <common.h>
#include <command.h>
#include <malloc.h>
#include <linux/string.h>
#include <linux/ctype.h>
#include <errno.h>

#define MAX_TFTP_PATH_LEN 127

/*
 * Returns the ethaddr environment variable formated with -'s instead of :'s
 */
static void format_mac_pxecfg(char **outbuf)
{
	char *p, *ethaddr;

	*outbuf = NULL;

	ethaddr = getenv("ethaddr");

	if (!ethaddr)
		return;

	*outbuf = strdup(ethaddr);

	if (*outbuf == NULL)
		return;

	for (p = *outbuf; *p; p++)
		if (*p == ':')
			*p = '-';
}

/*
 * Returns the directory the file specified in the bootfile env variable is
 * in.
 */
static char *get_bootfile_path(void)
{
	char *bootfile, *bootfile_path, *last_slash;

	bootfile = getenv("bootfile");

	if (bootfile == NULL) {
		printf("Missing bootfile environment variable.\n");
		return NULL;
	}

	bootfile_path = strdup(bootfile);

	if (bootfile_path == NULL) {
		printf("oom\n");
		return NULL;
	}

	last_slash = strrchr(bootfile_path, '/');

	if (last_slash == NULL) {
		printf("Invalid bootfile path (%s), no '/' found\n",
				bootfile_path);

		return NULL;
	}

	*last_slash = '\0';

	return bootfile_path;
}

static int get_relfile(char *file_path, void *file_addr)
{
	char *bootfile_path;
	size_t path_len;
	char bootfile[MAX_TFTP_PATH_LEN+1];
	char addr_buf[10];
	char *tftp_argv[] = {"tftp", NULL, NULL, NULL};

	bootfile_path = get_bootfile_path();

	if (bootfile_path == NULL) {
		printf("No bootfile path in environment.\n");
		return -ENOENT;
	}

	path_len = strlen(bootfile_path) + strlen(file_path) + 1;

	if (path_len > MAX_TFTP_PATH_LEN) {
		printf("Base path too long (%s/%s)\n",
				bootfile_path, file_path);

		free(bootfile_path);
		return -ENAMETOOLONG;
	}

	sprintf(bootfile, "%s/%s", bootfile_path, file_path);

	free(bootfile_path);

	printf("Retreiving file: %s\n", bootfile);

	sprintf(addr_buf, "%p", file_addr);

	tftp_argv[1] = addr_buf;
	tftp_argv[2] = bootfile;

	if (do_tftpb(NULL, 0, 3, tftp_argv)) {
		printf("File not found\n");
		return -ENOENT;
	}

	return 1;
}

static int get_pxecfg_file(char *file_path, void *file_addr)
{
	unsigned long config_file_size;
	int err;

	err = get_relfile(file_path, file_addr);

	if (err < 0)
		return err;

	config_file_size = simple_strtoul(getenv("filesize"), NULL, 16);
	*(char *)(file_addr + config_file_size) = '\0';

	return 1;
}


#define MAX_CFG_PATHS	12

/*
 * Returns a list of paths according to the pxelinux rules for paths to a
 * config file.  The base of each path is the same -
 * bootfilepath/pxelinux.cfg/uniquepart. bootfilepath is the directory
 * bootfile (from the dhcp request) was in, joined with '/pxelinux.cfg'.  The
 * unique part of each file comes from these things, in this order:
 *
 * pxeuuid env variable verbatim, if defined
 * ethaddr env variable, reformatted with -'s instead of :'s
 * hexadecimal formatted IP address
 * hexadecimal IP address with last hex digit removed
 * repeat removing another character until none are left
 * the string 'default'
 *
 * see http://syslinux.zytor.com/wiki/index.php/PXELINUX
 */
static char **pxecfg_paths(char *uuid_str, char *mac_addr)
{
	char **ret_array;
	char ip_addr[9];
	int x, end_masks, mask_pos;
	size_t base_len = strlen("pxelinux.cfg/");

	sprintf(ip_addr, "%08X", ntohl(NetOurIP));

	ret_array = malloc(MAX_CFG_PATHS * sizeof(char *));

	if (ret_array == NULL) {
		printf("oom\n");
		return NULL;
	}

	for (x = 0; x < MAX_CFG_PATHS; x++)
		ret_array[x] = NULL;

	x = 0;

	if (uuid_str) {
		if (base_len + strlen(uuid_str) > MAX_TFTP_PATH_LEN)
			printf("uuid_str is too long, skipping\n");
		else  {
			ret_array[x] = malloc(MAX_TFTP_PATH_LEN + 1);
			if (!ret_array[x])
				goto oom_error;

			sprintf(ret_array[x++], "pxelinux.cfg/%s", uuid_str);
		}
	}

	if (mac_addr) {
		if (base_len + strlen(mac_addr) > MAX_TFTP_PATH_LEN)
			printf("ethaddr is too long, skipping\n");
		else {
			ret_array[x] = malloc(MAX_TFTP_PATH_LEN + 1);
			if (!ret_array[x])
				goto oom_error;

			sprintf(ret_array[x++], "pxelinux.cfg/%s", mac_addr);
		}
	}

	for (end_masks = x + 8, mask_pos = 7; x < end_masks; x++, mask_pos--) {
		ret_array[x] = malloc(MAX_TFTP_PATH_LEN + 1);
		if (!ret_array[x])
			goto oom_error;

		sprintf(ret_array[x], "pxelinux.cfg/%s", ip_addr);

		ip_addr[mask_pos] = '\0';
	}

	ret_array[x] = malloc(MAX_TFTP_PATH_LEN + 1);
	if (!ret_array[x])
		goto oom_error;

	strcpy(ret_array[x++], "pxelinux.cfg/default");

	return ret_array;

oom_error:
	printf("oom\n");

	if (ret_array) {
		while (ret_array[x])
			free(ret_array[x++]);

		free(ret_array);
	}

	return NULL;
}

/*
 * Follows pxelinux's rules to download a pxecfg file from a tftp server.  The
 * file is stored at the location given by the pxecfg_addr environment
 * variable, which must be set.
 */
static int get_pxecfg(int argc, char * const argv[])
{
	char *mac_addr, *pxecfg_ram, **cfg_paths = NULL;
	int i, cfg_file_found = 0, err;
	void *pxecfg_addr;

	pxecfg_ram = getenv("pxecfg_ram");

	if (!pxecfg_ram) {
		printf("Missing pxecfg_ram\n");
		return 1;
	}

	pxecfg_addr = (void *)simple_strtoul(pxecfg_ram, NULL, 16);

	format_mac_pxecfg(&mac_addr);

	cfg_paths = pxecfg_paths(getenv("pxeuuid"), mac_addr);

	if (cfg_paths == NULL)
		return 1;

	for (i = 0; cfg_paths[i]; i++) {
		err = get_pxecfg_file(cfg_paths[i], pxecfg_addr);
		if (err > 0) {
			cfg_file_found = 1;
			break;
		}
	}

	for (i = 0; cfg_paths[i]; i++)
		free(cfg_paths[i]);

	if (!cfg_file_found)
		return 1;

	printf("Config file found!\n");

	return 0;
}

struct pxecfg_menu_item {
	char *name;
	char *kernel;
	char *append;
	char *initrd;
	int attempted;
	int localboot;
	struct pxecfg_menu_item *next;
};

struct pxecfg_menu {
	char *default_label;
	int timeout;
	char *title;
	int prompt;
	struct pxecfg_menu_item *labels;
};

static void add_label(struct pxecfg_menu *menu, struct pxecfg_menu_item *item)
{
	struct pxecfg_menu_item *iter;

	if (menu->labels == NULL) {
		menu->labels = item;
	} else {
		iter = menu->labels;

		while (iter->next != NULL)
			iter = iter->next;

		iter->next = item;
	}

	item->kernel = NULL;
	item->append = NULL;
	item->initrd = NULL;
	item->next = NULL;
	item->localboot = 0;
	item->attempted = 0;
}

struct pxecfg_menu_item *label_by_name(struct pxecfg_menu_item *head,
					char *name)
{
	struct pxecfg_menu_item *iter_label = head;

	while (iter_label)
		if (!strcmp(name, iter_label->name))
			return iter_label;
		else
			iter_label = iter_label->next;

	return NULL;
}

enum token_type {
	T_EOL,
	T_STRING,
	T_EOF,
	T_MENU,
	T_TITLE,
	T_TIMEOUT,
	T_LABEL,
	T_KERNEL,
	T_APPEND,
	T_INITRD,
	T_LOCALBOOT,
	T_DEFAULT,
	T_PROMPT,
	T_INCLUDE,
	T_INVALID
};

struct token {
	char *val;
	enum token_type type;
};

enum lex_state {
	L_NORMAL = 0,
	L_KEYWORD,
	L_SLITERAL
};

static const struct token keywords[] = {
	{"menu", T_MENU},
	{"title", T_TITLE},
	{"timeout", T_TIMEOUT},
	{"default", T_DEFAULT},
	{"prompt", T_PROMPT},
	{"label", T_LABEL},
	{"kernel", T_KERNEL},
	{"localboot", T_LOCALBOOT},
	{"append", T_APPEND},
	{"initrd", T_INITRD},
	{"include", T_INCLUDE},
	{NULL, T_INVALID}
};

static char *get_string(char **p, struct token *t, char delim, int lower)
{
	char *b, *e;
	size_t len, i;

	b = e = *p;

	while (*e) {
		if ((delim == ' ' && isspace(*e)) || delim == *e)
			break;
		e++;
	}

	len = e - b;

	t->val = malloc(len + 1);
	if (!t->val) {
		printf("oom\n");
		return NULL;
	}

	for (i = 0; i < len; i++, b++) {
		if (lower)
			t->val[i] = tolower(*b);
		else
			t->val[i] = *b;
	}

	t->val[len] = '\0';

	*p = e;

	t->type = T_STRING;

	return t->val;
}

static void get_keyword(struct token *t)
{
	int i;

	for (i = 0; keywords[i].val; i++) {
		if (!strcmp(t->val, keywords[i].val)) {
			t->type = keywords[i].type;
			break;
		}
	}
}

static void get_token(char **p, struct token *t, enum lex_state state)
{
	char *c = *p;

	t->type = T_INVALID;

	/* eat non EOL whitespace */
	while (*c == ' ' || *c == '\t')
		c++;

	/* eat comments */
	if (*c == '#')
		while (*c && *c != '\n')
			c++;

	if (*c == '\n') {
		t->type = T_EOL;
		c++;
	} else if (*c == '\0') {
		t->type = T_EOF;
		c++;
	} else if (state == L_SLITERAL)
		get_string(&c, t, '\n', 0);
	else if (state == L_KEYWORD) {
		get_string(&c, t, ' ', 1);
		get_keyword(t);
	}

	*p = c;
}

static void eol_or_eof(char **c)
{
	while (**c && **c != '\n')
		(*c)++;
}

static int parse_sliteral(char **c, char **dst)
{
	struct token t;
	char *s = *c;

	get_token(c, &t, L_SLITERAL);

	if (t.type != T_STRING) {
		printf("Expected string literal: %.*s\n", (int)(*c - s), s);
		return -EINVAL;
	}

	*dst = t.val;

	return 1;
}

static int parse_integer(char **c, int *dst)
{
	struct token t;
	char *s = *c;

	get_token(c, &t, L_SLITERAL);

	if (t.type != T_STRING) {
		printf("Expected string: %.*s\n", (int)(*c - s), s);
		return -EINVAL;
	}

	*dst = (int)simple_strtoul(t.val, NULL, 10);

	free(t.val);

	return 1;
}

static int parse_pxecfg_top(char *p, struct pxecfg_menu *m, int nest_level);

static int handle_include(char **c, char *base, struct pxecfg_menu *m,
						int nest_level)
{
	char *include_path;
	int err;

	err = parse_sliteral(c, &include_path);

	if (err < 0) {
		printf("Expected include path\n");
		return err;
	}

	err = get_pxecfg_file(include_path, base);

	if (err < 0) {
		printf("Couldn't get %s\n", include_path);
		return err;
	}

	return parse_pxecfg_top(base, m, nest_level);
}

static int parse_menu(char **c, struct pxecfg_menu *m, char *b, int nest_level)
{
	struct token t;
	char *s = *c;
	int err;

	get_token(c, &t, L_KEYWORD);

	switch (t.type) {
	case T_TITLE:
		err = parse_sliteral(c, &m->title);
		break;

	case T_INCLUDE:
		err = handle_include(c, b + strlen(b) + 1, m,
						nest_level + 1);
		break;

	default:
		printf("Ignoring malformed menu command: %.*s\n",
				(int)(*c - s), s);
	}

	if (err < 0)
		return err;

	eol_or_eof(c);

	return 1;
}

static int parse_label_menu(char **c, struct pxecfg_menu *m,
				struct pxecfg_menu_item *i)
{
	struct token t;
	char *s;

	s = *c;

	get_token(c, &t, L_KEYWORD);

	switch (t.type) {
	case T_DEFAULT:
		if (m->default_label != NULL)
			free(m->default_label);

		m->default_label = i->name;
		break;
	default:
		printf("Ignoring malformed menu command: %.*s\n",
				(int)(*c - s), s);
	}

	eol_or_eof(c);

	return 0;
}

static int parse_label(char **c, struct pxecfg_menu *m)
{
	struct token t;
	char *s;
	struct pxecfg_menu_item *i;
	int err;

	i = malloc(sizeof(*i));

	if (!i)
		return -ENOMEM;

	err = parse_sliteral(c, &i->name);

	if (err < 0) {
		printf("Expected label name\n");
		return -EINVAL;
	}

	add_label(m, i);

	while (1) {
		s = *c;
		get_token(c, &t, L_KEYWORD);

		err = 0;
		switch (t.type) {
		case T_MENU:
			err = parse_label_menu(c, m, i);
			break;

		case T_KERNEL:
			err = parse_sliteral(c, &i->kernel);
			break;

		case T_APPEND:
			err = parse_sliteral(c, &i->append);
			break;

		case T_INITRD:
			err = parse_sliteral(c, &i->initrd);
			break;

		case T_LOCALBOOT:
			err = parse_integer(c, &i->localboot);
			break;

		case T_EOL:
			break;

		/*
		 * A label ends when we either get to the end of a file, or
		 * get some input we otherwise don't have a handler defined
		 * for.
		 */
		default:
			/* put it back */
			*c = s;
			return 1;
		}

		if (err < 0)
			return err;
	}
}

#define MAX_NEST_LEVEL 16

static int parse_pxecfg_top(char *p, struct pxecfg_menu *m, int nest_level)
{
	struct token t;
	char *s, *b;
	int err;

	b = p;

	if (nest_level > MAX_NEST_LEVEL) {
		printf("Maximum nesting exceeded\n");
		return -EMLINK;
	}

	while (1) {
		s = p;

		get_token(&p, &t, L_KEYWORD);

		err = 0;
		switch (t.type) {
		case T_MENU:
			err = parse_menu(&p, m, b, nest_level);
			break;

		case T_TIMEOUT:
			err = parse_integer(&p, &m->timeout);
			break;

		case T_LABEL:
			err = parse_label(&p, m);
			break;

		case T_DEFAULT:
			err = parse_sliteral(&p, &m->default_label);
			break;

		case T_PROMPT:
			err = parse_integer(&p, &m->prompt);
			break;

		case T_EOL:
			break;

		case T_EOF:
			return 1;

		default:
			printf("Ignoring unknown command: %.*s\n",
							(int)(p - s), s);
			eol_or_eof(&p);
		}

		if (err < 0)
			return err;
	}
}

static struct pxecfg_menu *parse_pxecfg(char *menucfg)
{
	struct pxecfg_menu *menu;

	menu = malloc(sizeof(*menu));

	if (menu == NULL) {
		printf("oom\n");
		return NULL;
	}

	menu->timeout = -1;
	menu->prompt = 0;
	menu->default_label = NULL;
	menu->title = NULL;
	menu->labels = NULL;

	if (parse_pxecfg_top(menucfg, menu, 1) < 0) {
		free(menu);
		return NULL;
	}

	return menu;
}

static int perform_localboot(struct pxecfg_menu_item *label)
{
	char *localcmd, *dupcmd;
	int ret;

	localcmd = getenv("localcmd");

	if (!localcmd) {
		printf("localcmd not defined\n");
		return -ENOENT;
	}

	/*
	 * We must dup the environment variable value here, because getenv
	 * returns a pointer directly into the environment, and the contents
	 * of the environment might shift during execution of a command.
	 */
	dupcmd = strdup(localcmd);

	if (!dupcmd) {
		printf("oom\n");
		return -ENOMEM;
	}

	if (label->append)
		setenv("bootargs", label->append);

	printf("running: %s\n", dupcmd);

	ret = run_command2(dupcmd, 0);

	free(dupcmd);

	return ret;
}

static int get_relfile_envaddr(char *file_path, char *envaddr_name)
{
	void *file_addr;
	char *envaddr;

	envaddr = getenv(envaddr_name);

	if (!envaddr) {
		printf("missing: %s\n", envaddr_name);
		return -ENOENT;
	}

	file_addr = (void *)simple_strtoul(envaddr, NULL, 16);

	return get_relfile(file_path, file_addr);
}

static void print_label(struct pxecfg_menu_item *label)
{
	printf("Label: %s\n", label->name);

	if (label->kernel)
		printf("\tkernel: %s\n", label->kernel);

	if (label->append)
		printf("\tappend: %s\n", label->append);

	if (label->initrd)
		printf("\tinitrd: %s\n", label->initrd);
}

static void print_menu(struct pxecfg_menu *menu)
{
	struct pxecfg_menu_item *iter;

	if (menu->title)
		printf("Menu: %s\n", menu->title);

	iter = menu->labels;

	while (iter) {
		print_label(iter);

		iter = iter->next;
	}
}

static void destroy_label(struct pxecfg_menu_item *label)
{
	if (label->name)
		free(label->name);

	if (label->kernel)
		free(label->kernel);

	if (label->append)
		free(label->append);

	if (label->initrd)
		free(label->initrd);

	free(label);
}

static void destroy_menu(struct pxecfg_menu *menu)
{
	struct pxecfg_menu_item *iter, *next;

	if (!menu)
		return;

	iter = menu->labels;

	while (iter) {
		next = iter->next;
		destroy_label(iter);
		iter = next;
	}

	free(menu);
}

/*
 * Do what it takes to boot a chosen label.
 *
 * Retreive the kernel and initrd, and prepare bootargs.
 */
static void boot_label(struct pxecfg_menu_item *label)
{
	char *bootm_argv[] = { "bootm", NULL, NULL, NULL, NULL };

	print_label(label);

	label->attempted = 1;

	if (label->localboot) {
		perform_localboot(label);
		return;
	}

	if (label->kernel == NULL) {
		printf("No kernel given, skipping label\n");
		return;
	}

	if (label->initrd) {
		if (get_relfile_envaddr(label->initrd, "initrd_ram") < 0) {
			printf("Skipping label\n");
			return;
		}

		bootm_argv[2] = getenv("initrd_ram");
	} else
		bootm_argv[2] = "-";

	if (get_relfile_envaddr(label->kernel, "kernel_ram") < 0) {
		printf("Skipping label\n");
		return;
	}

	if (label->append)
		setenv("bootargs", label->append);

	bootm_argv[1] = getenv("kernel_ram");

	/*
	 * fdt usage is optional - if unset, this stays NULL.
	 */
	bootm_argv[3] = getenv("fdtaddr");

	do_bootm(NULL, 0, 4, bootm_argv);
}

/*
 * Change the default to the user's choice.
 */
static void user_choice(struct pxecfg_menu *menu)
{
	char cbuf[CONFIG_SYS_CBSIZE];
	struct pxecfg_menu_item *choice_label = NULL;

	while (!choice_label) {
		int readret;

		cbuf[0] = '\0';

		print_menu(menu);

		readret = readline_into_buffer("Enter label: ", cbuf);

		if (readret >= 0) {
			choice_label = label_by_name(menu->labels, cbuf);

			if (!choice_label)
				printf("%s not found\n", cbuf);
			else
				boot_label(choice_label);
		} else {
			printf("^C\n");
			return;
		}
	}
}

/*
 * Honor the timeout period from the cfg file or from the bootdelay
 * environment variable.
 *
 * If interrupted, allow the user to interactively choose a menu to boot from.
 */
static int delay_for_input(int timeout)
{
	char *bootdelay;

	bootdelay = getenv("bootdelay");

	if (timeout == -1) {
		if (bootdelay)
			timeout = simple_strtol(bootdelay, NULL, 10);
		else
			timeout = 0;
	}

	if (timeout) {
		printf("using the timeout: %d\n", timeout);

		if (_abortboot(timeout/10)) {
			printf("autoboot aborted!\n");
			return 1;
		}
	}

	return 0;
}

static void handle_pxecfg(struct pxecfg_menu *menu)
{
	struct pxecfg_menu_item *iter_label, *label = NULL;

	if (menu->labels == NULL) {
		printf("Configuration provided no labels, can't boot\n");
		return;
	}

	if (menu->prompt || delay_for_input(menu->timeout)) {
		user_choice(menu);
		return;
	}

	if (menu->default_label) {
		printf("Attempting to boot from default label: %s\n",
				menu->default_label);

		label = label_by_name(menu->labels, menu->default_label);

		if (label == NULL)
			label = menu->labels;
	} else
		label = menu->labels;

	while (label) {
		boot_label(label);

		label = NULL;

		iter_label = menu->labels;

		while (!label && iter_label) {
			if (!iter_label->attempted)
				label = iter_label;

			iter_label = iter_label->next;
		}
	}
}

static int boot_pxecfg(int argc, char * const argv[])
{
	unsigned long pxecfg_addr;
	struct pxecfg_menu *menu;
	char *pxecfg_ram;

	if (argc == 2) {
		pxecfg_ram = getenv("pxecfg_ram");
		if (pxecfg_ram == NULL) {
			printf("Missing pxecfg_ram in environment\n");
			return 1;
		}

		pxecfg_addr = simple_strtoul(pxecfg_ram, NULL, 16);
	} else if (argc == 3) {
		pxecfg_addr = simple_strtoul(argv[2], NULL, 16);
	} else {
		printf("Invalid number of arguments\n");
		return 1;
	}

	menu = parse_pxecfg((char *)(pxecfg_addr));

	if (menu == NULL) {
		printf("Error parsing config file\n");
		return 1;
	}

	handle_pxecfg(menu);

	destroy_menu(menu);

	return 0;
}

int do_pxecfg(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	if (argc < 2) {
		printf("pxecfg requires at least one argument\n");
		return EINVAL;
	}

	if (!strcmp(argv[1], "get"))
		return get_pxecfg(argc, argv);

	if (!strcmp(argv[1], "boot"))
		return boot_pxecfg(argc, argv);

	printf("Invalid pxecfg command: %s\n", argv[1]);

	return EINVAL;
}

U_BOOT_CMD(
	pxecfg, 2, 1, do_pxecfg,
	"commands to get and boot from pxecfg files",
	"get - try to retrieve a pxecfg file using tftp\npxecfg "
	"boot [pxecfg_addr] - boot from the pxecfg file at pxecfg_addr\n"
);
