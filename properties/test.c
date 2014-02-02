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
#include <stdlib.h>

#include <gsf/gsf-utils.h>
#include <gsf/gsf-meta-names.h>

#include <stdio.h>

#include "props-data.h"

int
main(int argc, char *argv[])
{
    char *uri = argv[1];
    GsfDocMetaData *meta_data;
    GsfDocProp *prop;
    const GValue *value;
    const char *tmp;

    gsf_init();

    meta_data = props_data_read(uri, NULL);
    if (meta_data) {
        prop = gsf_doc_meta_data_lookup(meta_data, GSF_META_NAME_TITLE);
        if (prop) {
            value = gsf_doc_prop_get_val(prop);

            tmp = g_value_get_string(value);
            fprintf(stderr, "str: %s\n", tmp);
        }
        g_object_unref(meta_data);
    }

    exit(0);
}
