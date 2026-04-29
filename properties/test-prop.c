/*
 * Copyright (C) 2012 Changwoo Ryu
 *
 * This program is free software; you can redistribute it and'or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see <http://www.gnu.org/licenses/>.
 */
#include <config.h>
#include <stdlib.h>
#include <locale.h>

#include <gsf/gsf-utils.h>
#include <gsf/gsf-meta-names.h>
#include <glib/gi18n-lib.h>

#include <stdio.h>

#include "props-data.h"

static void
prop_callback (const char *name, const char *value, gpointer user_data)
{
    printf("%s: %s\n", dgettext("nautilus", name), value);
}

int
main(int argc, char *argv[])
{
    char* path = argv[1];
    g_autoptr(GError) err;

    g_autofree char* abs_path = g_canonicalize_filename(path, NULL);
    g_autofree char* uri = g_filename_to_uri(abs_path, NULL, &err);
    if (uri == NULL) {
        g_printerr("Not a HWP or HWPX: %s", err->message);
        exit(1);
    }

    setlocale(LC_ALL, "");
    bindtextdomain (GETTEXT_PACKAGE, LOCALEDIR);
    bind_textdomain_codeset (GETTEXT_PACKAGE, "UTF-8");
    gsf_init();

    char* mime_type = NULL;
    if (g_str_has_suffix(path, ".hwpx"))
        mime_type = "application/x-hwpx";
    else if (g_str_has_suffix(path, ".hwp"))
        mime_type = "application/x-hwp";
    else {
        g_warning("Not a HWP or HWPX");
        exit(1);
    }

    props_data_for_each(uri, mime_type, prop_callback, NULL);
    exit(0);
}
