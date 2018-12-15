#ifndef _PWD_H
#define _PWD_H

#ifdef __cplusplus
extern "C" {
#endif

//#include <features.h>

#define __NEED_size_t
#define __NEED_uid_t
#define __NEED_gid_t

#include "alltypes.h"

	   struct passwd {
		   char	   *pw_name;	   /* user name */
		   char	   *pw_passwd;	   /* encrypted password */
		   uid_t   pw_uid;	   /* user uid */
		   gid_t   pw_gid;	   /* user gid */
		   time_t  pw_change;	   /* password change time */
		   char	   *pw_class;	   /* user access class */
		   char	   *pw_gecos;	   /* Honeywell login info */
		   char	   *pw_dir;	   /* home directory */
		   char	   *pw_shell;	   /* default shell */
		   time_t  pw_expire;	   /* account expiration */
		   int	   pw_fields;	   /* internal: fields filled in */
	   };

#ifdef __cplusplus
}
#endif

#endif
