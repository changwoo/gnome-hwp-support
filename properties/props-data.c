/* 
 * Copyright (C) 2012-2026 Changwoo Ryu
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
#include "props-data.h"

#include <string.h>

#include <gsf/gsf-doc-meta-data.h>
#include <gsf/gsf-infile-msole.h>
#include <gsf/gsf-infile.h>
#include <gsf/gsf-input-gio.h>
#include <gsf/gsf-input-memory.h>
#include <gsf/gsf-input-stdio.h>
#include <gsf/gsf-msole-utils.h>
#include <gsf/gsf-utils.h>
#include <gsf/gsf-meta-names.h>
#include <gsf/gsf-timestamp.h>

#include <glib/gi18n-lib.h>


static GsfDocMetaData *
props_data_read_internal(const char *uri)
{
    GsfInput *input;
    GsfInfile *infile;
    GsfInput *summary;
    GError *error = NULL;

    input = gsf_input_gio_new_for_uri(uri, &error);
    if (!input) {
        if (error) g_error_free(error);
        return NULL;
    }

    infile = gsf_infile_msole_new(input, NULL);
    g_object_unref(input);
    if (!infile) return NULL;

    summary = gsf_infile_child_by_name(infile, "\005HwpSummaryInformation");
    g_object_unref(infile);
    if (!summary) return NULL;

    static guint8 const component_guid [] = {
        0xe0, 0x85, 0x9f, 0xf2, 0xf9, 0x4f, 0x68, 0x10,
        0xab, 0x91, 0x08, 0x00, 0x2b, 0x27, 0xb3, 0xd9
    };

    int size = gsf_input_size(summary);
    if (size < (28 + sizeof(component_guid))) {
        g_object_unref(summary);
        return NULL;
    }

    guint8 *buf = g_malloc(size);
    gsf_input_read(summary, size, buf);
    g_object_unref(summary);

    memcpy(buf + 28, component_guid, sizeof(component_guid));
    summary = gsf_input_memory_new(buf, size, TRUE);

    GsfDocMetaData *meta = gsf_doc_meta_data_new();
    gsf_doc_meta_data_read_from_msole(meta, summary);
    g_object_unref(summary);

    return meta;
}

static char*
format_timestamp(GsfTimestamp *ts)
{
    g_autoptr(GDateTime) dt = g_date_time_new_from_unix_local(ts->timet);
    return g_date_time_format(dt, "%c");
}

void
props_data_for_each(const char *uri, HwpPropCallback callback, gpointer user_data)
{
    GsfDocMetaData *meta_data;
    unsigned i;

    if (!callback) return;

    meta_data = props_data_read_internal(uri);
    if (!meta_data) return;

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

    for (i = 0; i < G_N_ELEMENTS(meta_prop); i++) {
        GsfDocProp *prop = gsf_doc_meta_data_lookup(meta_data, meta_prop[i].key);
        if (!prop) continue;

        const GValue *value = gsf_doc_prop_get_val(prop);
        g_autofree char *val_str = NULL;

        switch (G_TYPE_FUNDAMENTAL(G_VALUE_TYPE(value))) {
            case G_TYPE_STRING:
                {
                    const char *s = g_value_get_string(value);
                    if (s && *s != '\0') val_str = g_strdup(s);
                }
                break;
            case G_TYPE_BOXED:
                if (VAL_IS_GSF_TIMESTAMP(value)) {
                    GsfTimestamp *ts = g_value_get_boxed(value);
                    val_str = format_timestamp(ts);
                }
                break;
        }

        if (val_str) {
            callback(_(meta_prop[i].name), val_str, user_data);
        }
    }

    g_object_unref(meta_data);
}
