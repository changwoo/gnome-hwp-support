/* Copyright (C) 2022 Changwoo Ryu <cwryu@debian.org
 *
 * Based on Nautilus
 * extensions/image-properties/nautilus-image-properties-module.c:
 */
/* Copyright (C) 2018 Ernestas Kulik <ernestask@gnome.org>
 *
 * This file is part of Nautilus.
 *
 * Nautilus is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * Nautilus is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Nautilus.  If not, see <https://www.gnu.org/licenses/>.
 */

#include <config.h>

#include <glib/gi18n-lib.h>

#include <nautilus/nautilus-extension.h>

#include <gsf/gsf-utils.h>
#include "hwp-properties-model-provider.h"

#ifdef __has_attribute
#if __has_attribute(__visibility__)
#define PUBLIC __attribute__ ((__visibility__("default")))
#endif
#endif
#ifndef PUBLIC
#define PUBLIC
#endif

PUBLIC
void
nautilus_module_initialize (GTypeModule *module)
{
    bindtextdomain (GETTEXT_PACKAGE, LOCALEDIR);
    bind_textdomain_codeset (GETTEXT_PACKAGE, "UTF-8");

    hwp_properties_model_provider_load (module);

    gsf_init();
}

PUBLIC
void
nautilus_module_shutdown (void)
{
    gsf_shutdown();
}

PUBLIC
void
nautilus_module_list_types (const GType **types,
                            int          *num_types)
{
    static GType type_list[1] = { 0 };

    g_assert (types != NULL);
    g_assert (num_types != NULL);

    type_list[0] = HWP_TYPE_PROPERTIES_MODEL_PROVIDER;

    *types = type_list;
    *num_types = G_N_ELEMENTS (type_list);
}
