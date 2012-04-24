/*
 *   team.h - Network team device driver library
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

#ifndef _TEAM_H_
#define _TEAM_H_

#include <stdint.h>
#include <stdbool.h>
#include <stdarg.h>
#include <linux/filter.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * team_handle
 *
 * library user context
 */
struct team_handle;

struct team_handle *team_alloc(void);
int team_create(struct team_handle *th, const char *team_name);
int team_recreate(struct team_handle *th, const char *team_name);
int team_destroy(struct team_handle *th);
int team_init(struct team_handle *th, uint32_t ifindex);
void team_free(struct team_handle *th);
void team_set_log_fn(struct team_handle *th,
		     void (*log_fn)(struct team_handle *th, int priority,
				    const char *file, int line, const char *fn,
				    const char *format, va_list args));
int team_get_log_priority(struct team_handle *th);
void team_set_log_priority(struct team_handle *th, int priority);
void *team_get_user_priv(struct team_handle *th);
void team_set_user_priv(struct team_handle *th, void *priv);
int team_check_events(struct team_handle *th);
int team_get_mode_name(struct team_handle *th, char **mode_name);
int team_set_mode_name(struct team_handle *th, const char *mode_name);
int team_get_active_port(struct team_handle *th, uint32_t *ifindex);
int team_set_active_port(struct team_handle *th, uint32_t ifindex);
int team_get_bpf_hash_func(struct team_handle *th, struct sock_fprog *fp);
int team_set_bpf_hash_func(struct team_handle *th, const struct sock_fprog *fp);

/*
 * team_eventfd
 *
 * access to list of event handlers
 */
struct team_eventfd;

const struct team_eventfd *team_get_next_eventfd(struct team_handle *th,
						 const struct team_eventfd *eventfd);
#define team_for_each_event_fd(eventfd, th)				\
	for (eventfd = team_get_next_eventfd(th, NULL); eventfd;	\
	     eventfd = team_get_next_eventfd(th, eventfd))
int team_get_eventfd_fd(struct team_handle *th,
			const struct team_eventfd *eventfd);
int team_call_eventfd_handler(struct team_handle *th,
			      const struct team_eventfd *eventfd);

/*
 * team_port
 *
 * access to port_list and individual port
 */
struct team_port;

struct team_port *team_get_next_port(struct team_handle *th,
				     struct team_port *port);
#define team_for_each_port(port, th)				\
	for (port = team_get_next_port(th, NULL); port;		\
	     port = team_get_next_port(th, port))
/* port getters */
uint32_t team_get_port_ifindex(struct team_port *port);
uint32_t team_get_port_speed(struct team_port *port);
uint8_t team_get_port_duplex(struct team_port *port);
bool team_is_port_link_up(struct team_port *port);
bool team_is_port_changed(struct team_port *port);
bool team_is_port_removed(struct team_port *port);
struct team_ifinfo;
struct team_ifinfo *team_get_port_ifinfo(struct team_port *port);

/*
 * team_ifinfo
 *
 * access to list of ifinfo_list
 */

struct team_ifinfo *team_get_next_ifinfo(struct team_handle *th,
					 struct team_ifinfo *ifinfo);
#define team_for_each_ifinfo(ifinfo, th)			\
	for (ifinfo = team_get_next_ifinfo(th, NULL); ifinfo;	\
	     ifinfo = team_get_next_ifinfo(th, ifinfo))
/* ifinfo getters */
uint32_t team_get_ifinfo_ifindex(struct team_ifinfo *ifinfo);
struct team_port *team_get_ifinfo_port(struct team_ifinfo *ifinfo);
char *team_get_ifinfo_hwaddr(struct team_ifinfo *ifinfo);
bool team_is_ifinfo_hwaddr_changed(struct team_ifinfo *ifinfo);
size_t team_get_ifinfo_hwaddr_len(struct team_ifinfo *ifinfo);
bool team_is_ifinfo_hwaddr_len_changed(struct team_ifinfo *ifinfo);
char *team_get_ifinfo_ifname(struct team_ifinfo *ifinfo);
bool team_is_ifinfo_ifname_changed(struct team_ifinfo *ifinfo);

/*
 * team_option
 *
 * access to option_list and individual option
 */
struct team_port;

enum team_option_type {
	TEAM_OPTION_TYPE_U32,
	TEAM_OPTION_TYPE_STRING,
	TEAM_OPTION_TYPE_BINARY,
	TEAM_OPTION_TYPE_BOOL,
};

struct team_option;

struct team_option *team_get_option_by_name(struct team_handle *th,
					    const char *name);
struct team_option *team_get_next_option(struct team_handle *th,
					 struct team_option *option);
#define team_for_each_option(port, th)				\
	for (option = team_get_next_option(th, NULL); option;	\
	     option = team_get_next_option(th, option))
/* option getters */
char *team_get_option_name(struct team_option *option);
uint32_t team_get_option_port_ifindex(struct team_option *option);
enum team_option_type team_get_option_type(struct team_option *option);
uint32_t team_get_option_value_u32(struct team_option *option);
char *team_get_option_value_string(struct team_option *option);
void *team_get_option_value_binary(struct team_option *option);
bool team_get_option_value_bool(struct team_option *option);
unsigned int team_get_option_value_len(struct team_option *option);
bool team_is_option_changed(struct team_option *option);
int team_get_option_value_by_name_u32(struct team_handle *th,
				      const char *name,
				      uint32_t *u32_ptr);
int team_get_port_option_value_by_name_u32(struct team_handle *th,
					   const char *name,
					   uint32_t port_ifindex,
					   uint32_t *u32_ptr);
int team_get_option_value_by_name_string(struct team_handle *th,
					 const char *name,
					 char **str_ptr);
int team_get_port_option_value_by_name_string(struct team_handle *th,
					      const char *name,
					      uint32_t port_ifindex,
					      char **str_ptr);
int team_get_option_value_by_name_binary(struct team_handle *th,
					 const char *name,
					 void **data_ptr);
int team_get_port_option_value_by_name_binary(struct team_handle *th,
					      const char *name,
					      uint32_t port_ifindex,
					      void **data_ptr);
int team_get_option_value_by_name_bool(struct team_handle *th,
				       const char *name,
				       bool *bool_ptr);
int team_get_port_option_value_by_name_bool(struct team_handle *th,
					    const char *name,
					    uint32_t port_ifindex,
					    bool *bool_ptr);
/* option setters */
int team_set_option_value_by_name_u32(struct team_handle *th,
				      const char *name,
				      uint32_t val);
int team_set_port_option_value_by_name_u32(struct team_handle *th,
					   const char *name,
					   uint32_t port_ifindex,
					   uint32_t val);
int team_set_option_value_by_name_string(struct team_handle *th,
					 const char *name,
					 const char *str);
int team_set_port_option_value_by_name_string(struct team_handle *th,
					      const char *name,
					      uint32_t port_ifindex,
					      const char *str);
int team_set_option_value_by_name_binary(struct team_handle *th,
					 const char *name,
					 const void *data,
					 unsigned int data_len);
int team_set_port_option_value_by_name_binary(struct team_handle *th,
					      const char *name,
					      uint32_t port_ifindex,
					      const void *data,
					      unsigned int data_len);
int team_set_option_value_by_name_bool(struct team_handle *th,
				       const char *name,
				       bool val);
int team_set_port_option_value_by_name_bool(struct team_handle *th,
					    const char *name,
					    uint32_t port_ifindex,
					    bool val);

/*
 * team_change_handler
 *
 * define change event types and register change handler functions
 */
enum {
	TEAM_PORT_CHANGE	= 0x1,
	TEAM_OPTION_CHANGE	= 0x2,
	TEAM_IFINFO_CHANGE	= 0x4,
	TEAM_ANY_CHANGE		= TEAM_PORT_CHANGE |
				  TEAM_OPTION_CHANGE |
				  TEAM_IFINFO_CHANGE,
};

typedef unsigned int team_change_type_mask_t;

struct team_change_handler {
	int			(*func)(struct team_handle *th,
					void *func_priv,
					team_change_type_mask_t type_mask);
					/* type_mask passed to function
					 * represents types of events which
					 * really happened. */
	void *			func_priv;
	team_change_type_mask_t	type_mask;
};

int team_change_handler_register(struct team_handle *th,
				 struct team_change_handler *handler);
void team_change_handler_unregister(struct team_handle *th,
				    struct team_change_handler *handler);

/*
 * route netlink helper functions
 */
uint32_t team_ifname2ifindex(struct team_handle *th, const char *ifname);
char *team_ifindex2ifname(struct team_handle *th, uint32_t ifindex,
			  char *ifname, unsigned int maxlen);
int team_port_add(struct team_handle *th, uint32_t port_ifindex);
int team_port_remove(struct team_handle *th, uint32_t port_ifindex);
int team_hwaddr_set(struct team_handle *th, uint32_t ifindex,
		    const char *addr, unsigned int addr_len);
int team_hwaddr_get(struct team_handle *th, uint32_t ifindex,
		    char *addr, unsigned int addr_len);
int team_hwaddr_len_get(struct team_handle *th, uint32_t ifindex);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* _TEAM_H_ */
