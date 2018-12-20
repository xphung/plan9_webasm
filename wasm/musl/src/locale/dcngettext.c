#include <libintl.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <limits.h>
#include <sys/stat.h>
#include <ctype.h>
#include "locale_impl.h"
#include "libc.h"
#include "atomic.h"

struct binding {
	struct binding *next;
	int dirlen;
	volatile int active;
	char *domainname;
	char *dirname;
	char buf[];
};

static void *volatile bindings;

static char *gettextdir(const char *domainname, size_t *dirlen)
{
	struct binding *p;
	for (p=bindings; p; p=p->next) {
		if (!strcmp(p->domainname, domainname) && p->active) {
			*dirlen = p->dirlen;
			return (char *)p->dirname;
		}
	}
	return 0;
}

char *bindtextdomain(const char *domainname, const char *dirname)
{
	return NULL; // a_cas_p atomic is not avail in webasm
}

static const char catnames[][12] = {
	"LC_CTYPE",
	"LC_NUMERIC",
	"LC_TIME",
	"LC_COLLATE",
	"LC_MONETARY",
	"LC_MESSAGES",
};

static const char catlens[] = { 8, 10, 7, 10, 11, 11 };

struct msgcat {
	struct msgcat *next;
	const void *map;
	size_t map_size;
	void *volatile plural_rule;
	volatile int nplurals;
	struct binding *binding;
	const struct __locale_map *lm;
	int cat;
};

static char *dummy_gettextdomain()
{
	return "messages";
}

weak_alias(dummy_gettextdomain, __gettextdomain);

const unsigned char *__map_file(const char *, size_t *);
int __munmap(void *, size_t);
unsigned long __pleval(const char *, unsigned long);

char *dcngettext(const char *domainname, const char *msgid1, const char *msgid2, unsigned long int n, int category)
{
        return NULL; // a_cas_p atomic is not avail in webasm
}

char *dcgettext(const char *domainname, const char *msgid, int category)
{
	return dcngettext(domainname, msgid, 0, 1, category);
}

char *dngettext(const char *domainname, const char *msgid1, const char *msgid2, unsigned long int n)
{
	return dcngettext(domainname, msgid1, msgid2, n, LC_MESSAGES);
}

char *dgettext(const char *domainname, const char *msgid)
{
	return dcngettext(domainname, msgid, 0, 1, LC_MESSAGES);
}
