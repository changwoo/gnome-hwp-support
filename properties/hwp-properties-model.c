/* Copyright (C) 2012-2022 Changwoo Ryu
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

#include "hwp-properties-model.h"
#include "props-data.h"

#include <glib/gi18n-lib.h>
#include <gsf/gsf-meta-names.h>
#include <gsf/gsf-timestamp.h>

typedef struct _HwpPropertiesModel HwpPropertiesModel;
struct _HwpPropertiesModel {
    GListStore *group_model;
};

static void
append_item (HwpPropertiesModel *self,
             const char         *name,
             const char         *value)
{
    g_autoptr (NautilusPropertiesItem) item = NULL;

    item = nautilus_properties_item_new (name, value);
    g_list_store_append (self->group_model, item);
}

void
set_info (HwpPropertiesModel *self, const GsfDocMetaData *meta_data)
{
    GsfDocProp *prop;
    unsigned i;

    static const struct {
        const char *key;
        const char *name;
    } meta_prop [] = {
        { GSF_META_NAME_CREATOR, N_("Creator") },
        { GSF_META_NAME_DATE_MODIFIED, N_("Modified") },
        { GSF_META_NAME_DATE_CREATED, N_("Created") },
        { GSF_META_NAME_TITLE, N_("Title") },
        { GSF_META_NAME_KEYWORDS, N_("Keywords") },
        { GSF_META_NAME_SUBJECT, N_("Subject") },
        { GSF_META_NAME_PAGE_COUNT, N_("Number of pages") },
    };

    for (i = 0; i < sizeof(meta_prop)/sizeof(meta_prop[0]); i++) {
        prop = gsf_doc_meta_data_lookup (meta_data, meta_prop[i].key);
        if (!prop)
            continue;

        const GValue *value = gsf_doc_prop_get_val(prop);
        const char *s;

        switch (G_TYPE_FUNDAMENTAL(G_VALUE_TYPE(value))) {
        case G_TYPE_STRING:
            s = g_value_get_string (gsf_doc_prop_get_val(prop));
            if (!s || *s == '\0')
                continue;
            append_item(self, _(meta_prop[i].name), s);
            break;
        case G_TYPE_BOXED:
            if (VAL_IS_GSF_TIMESTAMP (value)) {
                GsfTimestamp* ts = g_value_get_boxed (value);
                char *sa = gsf_timestamp_as_string (ts);
                append_item(self, _(meta_prop[i].name), s);
            }
            break;
        default:
            continue;
        }

    }
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
    g_autofree char *uri = NULL;
    g_autofree char *mime_type = NULL;
    GsfDocMetaData *meta_data = NULL;

    uri = nautilus_file_info_get_uri (file_info);
    mime_type = nautilus_file_info_get_mime_type (file_info);
    if (strcmp(mime_type, "application/x-hwp") != 0)
        goto end;

    meta_data = props_data_read(uri, NULL);
    if (!meta_data)
        goto end;

    set_info(self, meta_data);

  end:
    if (meta_data)
        g_object_unref (meta_data);
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
