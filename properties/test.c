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
	char *tmp;

	gsf_init();

	meta_data = props_data_read(uri, NULL);

	prop = gsf_doc_meta_data_lookup(meta_data, GSF_META_NAME_TITLE);
	if (prop) {
		value = gsf_doc_prop_get_val(prop);

		tmp = g_value_get_string(value);
		fprintf(stderr, "str: %s\n", tmp);
	}

	g_object_unref(meta_data);

	gsf_shutdown();

	exit(0);
}
