/* Copyright (C) 2012-2022 Changwoo Ryu
 *
 * Based on Nautilus
 * extensions/image-properties/nautilus-image-properties-model-provider.c:
 */
/* Copyright (C) 2004 Red Hat, Inc
 * Copyright (c) 2007 Novell, Inc.
 * Copyright (c) 2017 Thomas Bechtold <thomasbechtold@jpberlin.de>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with this program; if not, see <http://www.gnu.org/licenses/>.
 *
 * Author: Alexander Larsson <alexl@redhat.com>
 * XMP support by Hubert Figuiere <hfiguiere@novell.com>
 */
#include "hwp-properties-model-provider.h"
#include "hwp-properties-model.h"

static void properties_group_provider_iface_init (NautilusPropertiesModelProviderInterface *iface);

struct _HwpPropertiesModelProvider
{
    GObject parent_instance;
};

static void properties_group_provider_iface_init (NautilusPropertiesModelProviderInterface *iface);

G_DEFINE_DYNAMIC_TYPE_EXTENDED (HwpPropertiesModelProvider,
                                hwp_properties_model_provider,
                                G_TYPE_OBJECT,
                                0,
                                G_IMPLEMENT_INTERFACE_DYNAMIC (NAUTILUS_TYPE_PROPERTIES_MODEL_PROVIDER,
                                                               properties_group_provider_iface_init))


static GList *
get_models (NautilusPropertiesModelProvider *provider,
            GList                           *files)
{
    NautilusFileInfo *file_info;
    g_autofree char *mime_type = NULL;
    NautilusPropertiesModel *properties_group;

    if (files == NULL || files->next != NULL)
    {
        return NULL;
    }

    file_info = NAUTILUS_FILE_INFO (files->data);
    mime_type = nautilus_file_info_get_mime_type (file_info);

    if (strcmp (mime_type, "application/x-hwp") != 0)
        return NULL;
    properties_group = hwp_properties_model_new (file_info);

    return g_list_prepend (NULL, properties_group);
}

static void
properties_group_provider_iface_init (NautilusPropertiesModelProviderInterface *iface)
{
    iface->get_models = get_models;
}

static void
hwp_properties_model_provider_init (HwpPropertiesModelProvider *self)
{
    (void) self;
}

static void
hwp_properties_model_provider_class_init (HwpPropertiesModelProviderClass *klass)
{
    (void) klass;
}

static void
hwp_properties_model_provider_class_finalize (HwpPropertiesModelProviderClass *klass)
{
    (void) klass;
}


void
hwp_properties_model_provider_load (GTypeModule *module)
{
   hwp_properties_model_provider_register_type (module);
}
