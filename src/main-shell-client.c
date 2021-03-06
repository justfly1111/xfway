/* Copyright (C) 2019 adlo
 *
 * This library is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this library; if not, see <http://www.gnu.org/licenses/>
 */

#include <stdio.h>
#include <string.h>
#include <wayland-client.h>
#include <protocol/xfway-shell-client-protocol.h>
#include <protocol/wlr-foreign-toplevel-management-unstable-v1-client-protocol.h>
#include "screen.h"
#include "client.h"
#include "../util/libgwater-wayland.h"

struct wl_display *display = NULL;
static struct wl_registry *registry = NULL;

static Client *focus = NULL;

enum toplevel_state_field {
	TOPLEVEL_STATE_MAXIMIZED = (1 << 0),
	TOPLEVEL_STATE_MINIMIZED = (1 << 1),
	TOPLEVEL_STATE_ACTIVATED = (1 << 2),
	TOPLEVEL_STATE_FULLSCREEN = (1 << 3),
	TOPLEVEL_STATE_INVALID = (1 << 4),
};

static void shell_handle_tabwin (void *data, struct xfway_shell *shell)
{

}

static void shell_handle_tabwin_next (void *data, struct xfway_shell *shell)
{

}

static void shell_handle_tabwin_destroy (void *data, struct xfway_shell *shell)
{

}

struct xfway_shell_listener shell_impl = {
  shell_handle_tabwin,
  shell_handle_tabwin_next,
  shell_handle_tabwin_destroy,
};

static void toplevel_handle_title(void *data,
		struct zwlr_foreign_toplevel_handle_v1 *zwlr_toplevel,
		const char *title)
{
  Client *c = data;
  if (c->name)
    g_free (c->name);
  c->name = g_strdup (title);
}

static void toplevel_handle_app_id(void *data,
		struct zwlr_foreign_toplevel_handle_v1 *zwlr_toplevel,
		const char *app_id)
{

}

static uint32_t array_to_state(struct wl_array *array) {
	uint32_t state = 0;
	uint32_t *entry;
	wl_array_for_each(entry, array) {
		if (*entry == ZWLR_FOREIGN_TOPLEVEL_HANDLE_V1_STATE_MAXIMIZED)
			state |= TOPLEVEL_STATE_MAXIMIZED;
		if (*entry == ZWLR_FOREIGN_TOPLEVEL_HANDLE_V1_STATE_MINIMIZED)
			state |= TOPLEVEL_STATE_MINIMIZED;
		if (*entry == ZWLR_FOREIGN_TOPLEVEL_HANDLE_V1_STATE_ACTIVATED)
			state |= TOPLEVEL_STATE_ACTIVATED;
		if (*entry == ZWLR_FOREIGN_TOPLEVEL_HANDLE_V1_STATE_FULLSCREEN)
			state |= TOPLEVEL_STATE_FULLSCREEN;
	}

	return state;
}

static void toplevel_handle_state(void *data,
		struct zwlr_foreign_toplevel_handle_v1 *zwlr_toplevel,
		struct wl_array *state) {
	Client *c = data;
	uint32_t s = array_to_state(state);

  if (s & TOPLEVEL_STATE_ACTIVATED)
    focus = c;
}

static void toplevel_handle_done(void *data,
		struct zwlr_foreign_toplevel_handle_v1 *zwlr_toplevel)
{

}

static void toplevel_handle_closed(void *data,
		struct zwlr_foreign_toplevel_handle_v1 *zwlr_toplevel)
{
  Client *c = data;

  if (c->name)
    g_free (c->name);

  clientUnframe (c, FALSE);
}

static const struct zwlr_foreign_toplevel_handle_v1_listener toplevel_impl = {
	.title = toplevel_handle_title,
	.app_id = toplevel_handle_app_id,
	.output_enter = NULL,
	.output_leave = NULL,
	.state = toplevel_handle_state,
	.done = toplevel_handle_done,
	.closed = toplevel_handle_closed,
};

static void toplevel_manager_handle_toplevel(void *data,
		struct zwlr_foreign_toplevel_manager_v1 *toplevel_manager,
		struct zwlr_foreign_toplevel_handle_v1 *zwlr_toplevel)
{
  ScreenInfo *screen_info = data;
  Client *c;

  c = clientFrame (screen_info, zwlr_toplevel, FALSE);

  zwlr_foreign_toplevel_handle_v1_add_listener (zwlr_toplevel, &toplevel_impl,
                                                c);
}

static void toplevel_manager_handle_finished(void *data,
		struct zwlr_foreign_toplevel_manager_v1 *toplevel_manager) {
	zwlr_foreign_toplevel_manager_v1_destroy(toplevel_manager);
}

static const struct zwlr_foreign_toplevel_manager_v1_listener toplevel_manager_impl = {
	.toplevel = toplevel_manager_handle_toplevel,
	.finished = toplevel_manager_handle_finished,
};

void global_add (void               *data,
                 struct wl_registry *registry,
                 uint32_t            name,
                 const char         *interface,
                 uint32_t            version)
{
  ScreenInfo *screen_info = data;

  if (strcmp (interface, "xfway_shell") == 0)
    {
      struct xfway_shell *shell = NULL;
      shell = wl_registry_bind (registry, name, &xfway_shell_interface, 1);

      xfway_shell_add_listener (shell, &shell_impl, NULL);
    }
  else if (strcmp(interface,
			"zwlr_foreign_toplevel_manager_v1") == 0) {
		screen_info->toplevel_manager = wl_registry_bind(registry, name,
				&zwlr_foreign_toplevel_manager_v1_interface,
				2);

    zwlr_foreign_toplevel_manager_v1_add_listener(screen_info->toplevel_manager,
				&toplevel_manager_impl, screen_info);
      }
}
void global_remove (void               *data,
                    struct wl_registry *registry,
                    uint32_t            name)
{

}

struct wl_registry_listener registry_listener =
{
  .global = global_add,
  .global_remove = global_remove
};

int main (int    argc,
          char **argv)
{
  ScreenInfo *screen_info;
  GdkScreen *screen;
  GWaterWaylandSource *source;
  display = wl_display_connect (NULL);

  if (display == NULL)
    {
      fprintf (stderr, "Can't connect to display");
    }

  gtk_init (&argc, &argv);

  screen = gdk_screen_get_default ();

  screen_info = myScreenInit (screen);

  registry = wl_display_get_registry (display);

  wl_registry_add_listener (registry, &registry_listener, screen_info);

  wl_display_roundtrip (display);
  wl_display_roundtrip (display);

  source = g_water_wayland_source_new_for_display (NULL, display);

  gtk_main ();

  return 0;
}
