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
    char *uri = argv[1];

    setlocale(LC_ALL, "");
    bindtextdomain (GETTEXT_PACKAGE, LOCALEDIR);
    bind_textdomain_codeset (GETTEXT_PACKAGE, "UTF-8");
    gsf_init();

    props_data_for_each(uri, prop_callback, NULL);

    exit(0);
}
