/* Copyright (C) 2012-2026 Changwoo Ryu
 *
 * Based on Nautilus
 * extensions/image-properties/nautilus-image-properties-model.c:
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

#include <config.h>
#include "hwp-properties-model.h"
#include "props-data.h"

#include <glib/gi18n-lib.h>

typedef struct _HwpPropertiesModel HwpPropertiesModel;
struct _HwpPropertiesModel {
    GListStore *group_model;
};

static void
hwp_prop_callback (const char *name, const char *value, gpointer user_data)
{
    HwpPropertiesModel *self = (HwpPropertiesModel *) user_data;

    g_autoptr (NautilusPropertiesItem) item = NULL;
    item = nautilus_properties_item_new (name, value);
    g_list_store_append (self->group_model, item);
}

static void
hwp_properties_model_init (HwpPropertiesModel *self)
{
    self->group_model = g_list_store_new (NAUTILUS_TYPE_PROPERTIES_ITEM);
}

static void
hwp_properties_model_free (HwpPropertiesModel *self)
{
    g_free (self);
}

static void
hwp_properties_model_load_from_file_info (HwpPropertiesModel *self,
                                          NautilusFileInfo   *file_info)
{
    g_autofree char *uri = nautilus_file_info_get_uri (file_info);
    g_autofree char *mime_type = nautilus_file_info_get_mime_type (file_info);
    if (strcmp(mime_type, "application/x-hwp") == 0) {
        props_data_for_each(uri, hwp_prop_callback, self);
    }
}

NautilusPropertiesModel *
hwp_properties_model_new (NautilusFileInfo *file_info)
{
    HwpPropertiesModel *self;
    NautilusPropertiesModel *model;

    self = g_new0 (HwpPropertiesModel, 1);

    hwp_properties_model_init (self);
    hwp_properties_model_load_from_file_info (self, file_info);

    model = nautilus_properties_model_new (_("HWP document properties"),
                                           G_LIST_MODEL (self->group_model));

    g_object_weak_ref (G_OBJECT (model),
                       (GWeakNotify) hwp_properties_model_free,
                       self);

    return model;
}
