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

GsfDocMetaData *
props_data_read(const char *uri, GError **error)
{
    GsfInput *input;
    GsfInfile *infile;
    GsfInput *summary;

    input = gsf_input_gio_new_for_uri(uri, error);
    if (! input)
        return NULL;
    infile = gsf_infile_msole_new(input, NULL);
    g_object_unref(input);
    summary = gsf_infile_child_by_name(infile, "\005HwpSummaryInformation");
    g_object_unref(infile);

    int size = gsf_input_size(summary);
    guint8 *buf = g_malloc(size);
    gsf_input_read(summary, size, buf);

    static guint8 const component_guid [] = {
        0xe0, 0x85, 0x9f, 0xf2, 0xf9, 0x4f, 0x68, 0x10,
        0xab, 0x91, 0x08, 0x00, 0x2b, 0x27, 0xb3, 0xd9
    };
    g_object_unref(summary);

    /* Trick the libgsf's MSOLE property set parser, by changing
     * its GUID. The \005HwpSummaryInformation is compatible with
     * the summary property set.
     */
    memcpy(buf + 28, component_guid, sizeof(component_guid));
    summary = gsf_input_memory_new(buf, size, TRUE);
    g_free(buf);

    GsfDocMetaData *meta;
    meta = gsf_doc_meta_data_new();
    gsf_msole_metadata_read(summary, meta);
    g_object_unref(summary);

    /* TODO: workaround buggy HWP */

    return meta;
}


