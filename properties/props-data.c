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

#include <glib/gi18n-lib.h>

#include <gsf/gsf.h>


typedef struct {
    PropItemCallback callback;
    gpointer user_data;
    char* meta_name;
} HwpxXmlContext;

enum {
    PROP_TYPE_STRING,
    PROP_TYPE_DATE_TIME,
};

struct _MetaItem {
    char* name;
    char* label;
    int type;
};
typedef struct _MetaItem MetaItem;

static MetaItem META_ITEMS[] = {
    { .name = "CreatedDate", .label = N_("Created"),
      .type = PROP_TYPE_DATE_TIME },
    { .name = "ModifiedDate", .label = N_("Modified"),
      .type = PROP_TYPE_DATE_TIME },
    { .name = "creator", .label = N_("Creator"),
      .type = PROP_TYPE_STRING },
    { .name = "subject", .label = N_("Subject"),
      .type = PROP_TYPE_STRING },
    { .name = "keyword", .label = N_("Keywords"),
      .type = PROP_TYPE_STRING },
    { .name = "description", .label = N_("Description"),
      .type = PROP_TYPE_STRING },
    { .name = "language", .label = N_("Language"),
      .type = PROP_TYPE_STRING },
    { .name = "lastsaveby", .label = N_("Last save by"),
      .type = PROP_TYPE_STRING },
};

#define NUMBER_META_ITEMS (sizeof(META_ITEMS)/sizeof(META_ITEMS[0]))

/* <opf:title> */
static void
hwpx_metadata_title_end (GsfXMLIn *xin, GsfXMLBlob *unknown)
{
    HwpxXmlContext *ctx = xin->user_state;
    ctx->callback (_("Title"), xin->content->str, ctx->user_data);
}

/* <opf:meta> */
static void
hwpx_metadata_meta_start (GsfXMLIn *xin, xmlChar const **attrs)
{
    HwpxXmlContext *ctx = xin->user_state;

    for (int i = 0; attrs[i] != NULL; i += 2) {
        if (g_strcmp0 ((char*) attrs[i], "name") == 0) {
            ctx->meta_name = g_strdup((char*) attrs[i+1]);
            break;
        }
    }
}

static void
hwpx_metadata_meta_end (GsfXMLIn *xin, GsfXMLBlob *unknown)
{
    HwpxXmlContext *ctx = xin->user_state;

    char* name = ctx->meta_name;
    const char* value = xin->content->str;

    if (name && value && *value) {
        const char *label = name;

        for (int i = 0; i < NUMBER_META_ITEMS; i++) {
            MetaItem *item = &META_ITEMS[i];

            if (g_strcmp0(name, item->name) == 0) {
                label = item->label;
                switch (item->type) {
                    case PROP_TYPE_DATE_TIME: {
                        g_autoptr(GDateTime) dt = g_date_time_new_from_iso8601(value, NULL);
                        g_autoptr(GDateTime) local_dt = g_date_time_to_local(dt);
                        g_autofree char *dtstr = g_date_time_format(local_dt, "%c");
                        ctx->callback (_(label), dtstr, ctx->user_data);
                        break;
                    }
                    case PROP_TYPE_STRING:
                    default:
                        ctx->callback (_(label), value, ctx->user_data);
                        break;
                }
                break;
            }
        }

        g_free(name);
    }
}

enum {
    TAG_ROOT = 1,
    TAG_PACKAGE ,
    TAG_METADATA,
    TAG_TITLE,
    TAG_META,
    TAG_LANGUAGE,
    TAG_MANIFEST,
    TAG_MANIFEST_ITEM,
    TAG_SPINE,
    TAG_SPINE_ITEMREF,
};

enum {
    HWP_NS_OPF = 1,
    HWP_NS_HPF,
};

static const GsfXMLInNS hwpx_content_hpf_ns[] = {
    GSF_XML_IN_NS (HWP_NS_OPF, "http://www.idpf.org/2007/opf/"),
    GSF_XML_IN_NS (HWP_NS_HPF, "http://www.hancom.co.kr/schema/2011/hpf"),
    GSF_XML_IN_NS_END
};

static const GsfXMLInNode hwpx_content_hpf_dtd[] = {
    GSF_XML_IN_NODE_FULL(TAG_ROOT, TAG_ROOT, -1, NULL, GSF_XML_NO_CONTENT, FALSE, TRUE, NULL, NULL, 0),
    GSF_XML_IN_NODE(TAG_ROOT, TAG_PACKAGE, HWP_NS_OPF, "package", FALSE, NULL, NULL),
      GSF_XML_IN_NODE(TAG_PACKAGE, TAG_METADATA, HWP_NS_OPF, "metadata", FALSE, NULL, NULL),
        /* <opf:title> */
        GSF_XML_IN_NODE(TAG_METADATA, TAG_TITLE, HWP_NS_OPF, "title", TRUE, NULL, hwpx_metadata_title_end),
        /* <opf:meta> */
        GSF_XML_IN_NODE(TAG_METADATA, TAG_META, HWP_NS_OPF, "meta", TRUE, hwpx_metadata_meta_start, hwpx_metadata_meta_end),
        GSF_XML_IN_NODE(TAG_METADATA, TAG_LANGUAGE, HWP_NS_OPF, "language", TRUE, NULL, NULL),
      GSF_XML_IN_NODE(TAG_PACKAGE, TAG_MANIFEST, HWP_NS_OPF, "manifest", FALSE, NULL, NULL),
        GSF_XML_IN_NODE(TAG_MANIFEST, TAG_MANIFEST_ITEM, HWP_NS_OPF, "item", FALSE, NULL, NULL),
      GSF_XML_IN_NODE(TAG_PACKAGE, TAG_SPINE, HWP_NS_OPF, "spine", FALSE, NULL, NULL),
        GSF_XML_IN_NODE(TAG_SPINE, TAG_SPINE_ITEMREF, HWP_NS_OPF, "itemref", FALSE, NULL, NULL),
    GSF_XML_IN_NODE_END
};


static void
props_data_for_each_hwpx(GsfInfile        *infile,
                         PropItemCallback  callback,
                         gpointer          user_data)
{
    GsfInput *hpf_input = gsf_infile_child_by_vname(infile, "Contents", "content.hpf", NULL);
    if (!hpf_input) return;

    HwpxXmlContext ctx = { callback, user_data, 0 };

    GsfXMLInDoc *xin = gsf_xml_in_doc_new(hwpx_content_hpf_dtd, hwpx_content_hpf_ns);

    if (!gsf_xml_in_doc_parse(xin, hpf_input, &ctx)) {
        g_warning ("Failed to parse HWPX content.hpf");
    }

    gsf_xml_in_doc_free(xin);
    g_object_unref (hpf_input);
}


static char*
format_timestamp(GsfTimestamp *ts)
{
    g_autoptr(GDateTime) dt = g_date_time_new_from_unix_local(ts->timet);
    return g_date_time_format(dt, "%c");
}

static GsfDocMetaData *
props_data_read_hwp5_metadata(GsfInfile *infile)
{
    GsfInput* summary = gsf_infile_child_by_name(infile, "\005HwpSummaryInformation");
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

static void
props_data_for_each_hwp5(GsfInfile        *infile,
                         PropItemCallback  callback,
                         gpointer          user_data)
{
    GsfDocMetaData *meta_data;
    unsigned i;

    if (!callback) return;

    meta_data = props_data_read_hwp5_metadata(infile);
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
        { GSF_META_NAME_LANGUAGE, N_("Language") },
        { GSF_META_NAME_DESCRIPTION, N_("Description") },
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

void
props_data_for_each(const char *uri, const char *mime_type,
                    PropItemCallback callback, gpointer user_data)
{
    GsfInput* input = NULL;
    GError *error = NULL;

    input = gsf_input_gio_new_for_uri(uri, &error);
    if (!input) return;

    g_debug("mime_type: %s", mime_type);
    if (strcmp(mime_type, "application/x-hwpx") == 0) {
        gsf_input_seek(input, 0, G_SEEK_SET);
        GsfInfile* zip = gsf_infile_zip_new(input, NULL);
        if (zip) {
            props_data_for_each_hwpx(zip, callback, user_data);
            g_object_unref(zip);
            return;
        }
    } else if (strcmp(mime_type, "application/x-hwp") == 0) {
        gsf_input_seek(input, 0, G_SEEK_SET);
        GsfInfile* msole = gsf_infile_msole_new(input, NULL);
        if (msole) {
            props_data_for_each_hwp5(msole, callback, user_data);
            g_object_unref(msole);
        }
    }
}
