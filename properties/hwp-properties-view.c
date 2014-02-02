/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8; c-indent-level: 8 -*- */
/* Copyright (C) 2012 Changwoo Ryu <cwryu@debian.org
 *
 * Based on Evince properties/ev-properties-view.c:
 */
/* this file is part of evince, a gnome document viewer
 *
 *  Copyright (C) 2005 Red Hat, Inc
 *
 * Evince is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * Evince is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#include "config.h"

#include <string.h>
#include <sys/time.h>
#include <time.h>

#ifdef HAVE__NL_MEASUREMENT_MEASUREMENT
#include <langinfo.h>
#endif

#include <glib/gi18n-lib.h>
#include <gtk/gtk.h>

#include "hwp-properties-view.h"

#include <gsf/gsf-meta-names.h>
#include <gsf/gsf-timestamp.h>

typedef enum {
	TITLE_PROPERTY,
	URI_PROPERTY,
	SUBJECT_PROPERTY,
	AUTHOR_PROPERTY,
	KEYWORDS_PROPERTY,
	PRODUCER_PROPERTY,
	CREATOR_PROPERTY,
	CREATION_DATE_PROPERTY,
	MOD_DATE_PROPERTY,
	N_PAGES_PROPERTY,
	LINEARIZED_PROPERTY,
	FORMAT_PROPERTY,
	SECURITY_PROPERTY,
	PAPER_SIZE_PROPERTY,
	N_PROPERTIES
} Property;

typedef struct {
	Property property;
	const char *label;
} PropertyInfo;

static const PropertyInfo properties_info[] = {
	{ TITLE_PROPERTY,         N_("Title:") },
	{ URI_PROPERTY,           N_("Location:") },
	{ SUBJECT_PROPERTY,       N_("Subject:") },
	{ AUTHOR_PROPERTY,        N_("Author:") },
	{ KEYWORDS_PROPERTY,      N_("Keywords:") },
	{ PRODUCER_PROPERTY,      N_("Producer:") },
	{ CREATOR_PROPERTY,       N_("Creator:") },
	{ CREATION_DATE_PROPERTY, N_("Created:") },
	{ MOD_DATE_PROPERTY,      N_("Modified:") },
	{ N_PAGES_PROPERTY,       N_("Number of Pages:") },
	{ LINEARIZED_PROPERTY,    N_("Optimized:") },
	{ FORMAT_PROPERTY,        N_("Format:") },
	{ SECURITY_PROPERTY,      N_("Security:") },
	{ PAPER_SIZE_PROPERTY,    N_("Paper Size:") }
};

struct _HwpPropertiesView {
	GtkVBox base_instance;

	GtkWidget *grid;
	GtkWidget *labels[N_PROPERTIES];
	gchar     *uri;
};

struct _HwpPropertiesViewClass {
	GtkVBoxClass base_class;
};

G_DEFINE_TYPE (HwpPropertiesView, hwp_properties_view, GTK_TYPE_VBOX)

static void
hwp_properties_view_dispose (GObject *object)
{
	HwpPropertiesView *properties = HWP_PROPERTIES_VIEW (object);
	
	if (properties->uri) {
		g_free (properties->uri);
		properties->uri = NULL;
	}
	
	G_OBJECT_CLASS (hwp_properties_view_parent_class)->dispose (object);
}

static void
hwp_properties_view_class_init (HwpPropertiesViewClass *properties_class)
{
	GObjectClass *g_object_class = G_OBJECT_CLASS (properties_class);

	g_object_class->dispose = hwp_properties_view_dispose;
}

/* This is cut out of gconvert.c from glib (and mildly modified).  Not all
   backends give valid UTF-8 for properties, so we make sure that is.
*/
static gchar *
make_valid_utf8 (const gchar *name)
{
	GString *string;
	const gchar *remainder, *invalid;
	gint remaining_bytes, valid_bytes;
  
	string = NULL;
	remainder = name;
	remaining_bytes = strlen (name);
  
	while (remaining_bytes != 0) 
	{
		if (g_utf8_validate (remainder, remaining_bytes, &invalid)) 
			break;
		valid_bytes = invalid - remainder;
    
		if (string == NULL) 
			string = g_string_sized_new (remaining_bytes);

		g_string_append_len (string, remainder, valid_bytes);
		g_string_append_c (string, '?');
      
		remaining_bytes -= valid_bytes + 1;
		remainder = invalid + 1;
	}
  
	if (string == NULL)
		return g_strdup (name);
  
	g_string_append (string, remainder);

	g_assert (g_utf8_validate (string->str, -1, NULL));
  
	return g_string_free (string, FALSE);
}

static void
set_property (HwpPropertiesView *properties,
	      GtkGrid          *grid,
	      Property          property,
	      const gchar      *text,
	      gint             *row)
{
	GtkWidget *label;
	gchar     *markup;
	gchar     *valid_text;

	if (!properties->labels[property]) {
		label = gtk_label_new (NULL);
		g_object_set (G_OBJECT (label), "xalign", 0.0, NULL);
		markup = g_strdup_printf ("<b>%s</b>", _(properties_info[property].label));
		gtk_label_set_markup (GTK_LABEL (label), markup);
		g_free (markup);

		gtk_grid_attach (grid, label, 0, *row, 1, 1);
		gtk_widget_show (label);
	}

	if (!properties->labels[property]) {
		label = gtk_label_new (NULL);

		g_object_set (G_OBJECT (label),
			      "xalign", 0.0,
			      "width_chars", 25,
			      "selectable", TRUE,
			      "ellipsize", PANGO_ELLIPSIZE_END,
			      NULL);
	} else {
		label = properties->labels[property];
	}

	if (text == NULL || text[0] == '\000') {
		markup = g_markup_printf_escaped ("<i>%s</i>", _("None"));
		gtk_label_set_markup (GTK_LABEL (label), markup);
		g_free (markup);
	} else {
		valid_text = make_valid_utf8 (text ? text : "");
		gtk_label_set_text (GTK_LABEL (label), valid_text);
		g_free (valid_text);
	}

	if (!properties->labels[property]) {
		gtk_grid_attach (grid, label, 1, *row, 1, 1);
		properties->labels[property] = label;
	}

	gtk_widget_show (label);

	*row += 1;
}

void
hwp_properties_view_set_info (HwpPropertiesView *properties, const GsfDocMetaData *meta_data)
{
	GtkWidget *grid;
	gint       row = 0;

	GsfDocProp *prop;
	int i;

	static const struct {
		const char *key;
		Property prop;
	} meta_prop [] = {
		{ GSF_META_NAME_CREATOR, CREATOR_PROPERTY },
		{ GSF_META_NAME_DATE_MODIFIED, MOD_DATE_PROPERTY },
		{ GSF_META_NAME_DATE_CREATED, CREATION_DATE_PROPERTY },
		{ GSF_META_NAME_TITLE, TITLE_PROPERTY },
		/* { GSF_META_NAME_DESCRIPTION, TITLE_PROPERTY },*/
		{ GSF_META_NAME_KEYWORDS, KEYWORDS_PROPERTY },
		{ GSF_META_NAME_SUBJECT, SUBJECT_PROPERTY },
		{ GSF_META_NAME_PAGE_COUNT, N_PAGES_PROPERTY },
	};
	

	grid = properties->grid;

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
			set_property (properties, GTK_GRID (grid),
				      meta_prop[i].prop, s, &row);
			break;
		case G_TYPE_BOXED:
			if (VAL_IS_GSF_TIMESTAMP (value)) {
				GsfTimestamp* ts = g_value_get_boxed (value);
				char *sa = gsf_timestamp_as_string (ts);
				set_property (properties, GTK_GRID (grid),
					      meta_prop[i].prop, sa, &row);
			}
			break;
		default:
			continue;
		}

	}

	set_property (properties, GTK_GRID (grid), URI_PROPERTY, properties->uri, &row);
}

static void
hwp_properties_view_init (HwpPropertiesView *properties)
{
	properties->grid = gtk_grid_new ();
	gtk_grid_set_column_spacing (GTK_GRID (properties->grid), 12);
	gtk_grid_set_row_spacing (GTK_GRID (properties->grid), 6);
	gtk_container_set_border_width (GTK_CONTAINER (properties->grid), 12);
	gtk_box_pack_start (GTK_BOX (properties), properties->grid, TRUE, TRUE, 0);
	gtk_widget_show (properties->grid);
}

void
hwp_properties_view_register_type (GTypeModule *module)
{
	hwp_properties_view_get_type ();
}

GtkWidget *
hwp_properties_view_new (const gchar *uri)
{
	HwpPropertiesView *properties;

	properties = g_object_new (HWP_TYPE_PROPERTIES, NULL);
	properties->uri = g_strdup (uri);

	return GTK_WIDGET (properties);
}
