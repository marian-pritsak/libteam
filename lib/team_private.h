/*
 *   team-private.h - Network team device driver library private header
 *   Copyright (C) 2011 Jiri Pirko <jpirko@redhat.com>
 *
 *   This library is free software; you can redistribute it and/or
 *   modify it under the terms of the GNU Lesser General Public
 *   License as published by the Free Software Foundation; either
 *   version 2.1 of the License, or (at your option) any later version.
 *
 *   This library is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *   Lesser General Public License for more details.
 *
 *   You should have received a copy of the GNU Lesser General Public
 *   License along with this library; if not, write to the Free Software
 *   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifndef _TEAM_PRIVATE_H_
#define _TEAM_PRIVATE_H_

#include <stdarg.h>
#include <syslog.h>
#include <netlink/netlink.h>
#include <team.h>
#include <private/list.h>

#define TEAM_EXPORT __attribute__ ((visibility("default")))

/**
 * SECTION: team_handler
 * @short_description: libteam context
 */

struct team_handle {
	struct nl_sock *	nl_sock;
	int			nl_sock_err;
	struct nl_sock *	nl_sock_event;
	int			family;
	uint32_t		ifindex;
	struct list_item	port_list;
	struct list_item	ifinfo_list;
	struct list_item	option_list;
	struct {
		struct list_item		list;
		team_change_type_mask_t		pending_type_mask;
	} change_handler;
	struct {
		struct nl_sock *	sock;
		struct nl_cache *	link_cache;
		struct nl_sock *	sock_event;
	} nl_cli;
	void (*log_fn)(struct team_handle *th, int priority,
		       const char *file, int line, const char *fn,
		       const char *format, va_list args);
	int log_priority;
	void *user_priv;
};

/**
 * SECTION: logging
 * @short_description: libteam logging facility
 */

void team_log(struct team_handle *th, int priority,
	      const char *file, int line, const char *fn,
	      const char *format, ...);

static inline void __attribute__((always_inline, format(printf, 2, 3)))
team_log_null(struct team_handle *th, const char *format, ...) {}

#define team_log_cond(th, prio, arg...)					\
	do {								\
		if (team_get_log_priority(th) >= prio)			\
			team_log(th, prio, __FILE__, __LINE__,		\
				 __FUNCTION__, ## arg);			\
	} while (0)

#ifdef ENABLE_LOGGING
#  ifdef ENABLE_DEBUG
#    define dbg(th, arg...) team_log_cond(th, LOG_DEBUG, ## arg)
#  else
#    define dbg(th, arg...) team_log_null(th, ## arg)
#  endif
#  define info(th, arg...) team_log_cond(th, LOG_INFO, ## arg)
#  define warn(th, arg...) team_log_cond(th, LOG_WARN, ## arg)
#  define err(th, arg...) team_log_cond(th, LOG_ERR, ## arg)
#else
#  define dbg(th, arg...) team_log_null(th, ## arg)
#  define info(th, arg...) team_log_null(th, ## arg)
#  define warn(th, arg...) team_log_null(th, ## arg)
#  define err(th, arg...) team_log_null(th, ## arg)
#endif

/**
 * SECTION: function prototypes
 * @short_description: prototypes for internal functions
 */

int get_port_list_handler(struct nl_msg *msg, void *arg);
int port_list_alloc(struct team_handle *th);
int port_list_init(struct team_handle *th);
void port_list_free(struct team_handle *th);
int ifinfo_event_handler(struct nl_msg *msg, void *arg);
int ifinfo_create(struct team_handle *th, uint32_t ifindex,
		  struct team_port *port, struct team_ifinfo **p_ifinfo);
void ifinfo_destroy(struct team_handle *th, uint32_t ifindex);
int get_options_handler(struct nl_msg *msg, void *arg);
int option_list_alloc(struct team_handle *th);
int option_list_init(struct team_handle *th);
void option_list_free(struct team_handle *th);
int nl2syserr(int nl_error);
int send_and_recv(struct team_handle *th, struct nl_msg *msg,
		  int (*valid_handler)(struct nl_msg *, void *),
		  void *valid_data);
void set_call_change_handlers(struct team_handle *th,
			      team_change_type_mask_t set_type_mask);
int check_call_change_handlers(struct team_handle *th,
			       team_change_type_mask_t call_type_mask);

#endif /* _TEAM_PRIVATE_H_ */
