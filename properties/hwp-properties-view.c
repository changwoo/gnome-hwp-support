/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8; c-indent-level: 8 -*- */
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
	gchar     *text;
	gint       row = 0;

	GsfDocProp *prop;
	const GValue *value;
	char *tmp;
	

	grid = properties->grid;

	prop = gsf_doc_meta_data_lookup (meta_data, GSF_META_NAME_TITLE);
	if (prop) {
		value = gsf_doc_prop_get_val(prop);
		tmp = g_value_get_string (value);
		set_property (properties, GTK_GRID (grid), TITLE_PROPERTY, tmp, &row);
	}

	/*
	if (info->fields_mask & EV_DOCUMENT_INFO_TITLE) {
		set_property (properties, GTK_GRID (grid), TITLE_PROPERTY, info->title, &row);
	}
	set_property (properties, GTK_GRID (grid), URI_PROPERTY, properties->uri, &row);
	if (info->fields_mask & EV_DOCUMENT_INFO_SUBJECT) {
		set_property (properties, GTK_GRID (grid), SUBJECT_PROPERTY, info->subject, &row);
	}
	if (info->fields_mask & EV_DOCUMENT_INFO_AUTHOR) {
		set_property (properties, GTK_GRID (grid), AUTHOR_PROPERTY, info->author, &row);
	}
	if (info->fields_mask & EV_DOCUMENT_INFO_KEYWORDS) {
		set_property (properties, GTK_GRID (grid), KEYWORDS_PROPERTY, info->keywords, &row);
	}
	if (info->fields_mask & EV_DOCUMENT_INFO_PRODUCER) {
		set_property (properties, GTK_GRID (grid), PRODUCER_PROPERTY, info->producer, &row);
	}
	if (info->fields_mask & EV_DOCUMENT_INFO_CREATOR) {
		set_property (properties, GTK_GRID (grid), CREATOR_PROPERTY, info->creator, &row);
	}
	if (info->fields_mask & EV_DOCUMENT_INFO_CREATION_DATE) {
		text = hwp_document_misc_format_date (info->creation_date);
		set_property (properties, GTK_GRID (grid), CREATION_DATE_PROPERTY, text, &row);
		g_free (text);
	}
	if (info->fields_mask & EV_DOCUMENT_INFO_MOD_DATE) {
		text = hwp_document_misc_format_date (info->modified_date);
		set_property (properties, GTK_GRID (grid), MOD_DATE_PROPERTY, text, &row);
		g_free (text);
	}
	if (info->fields_mask & EV_DOCUMENT_INFO_FORMAT) {
		set_property (properties, GTK_GRID (grid), FORMAT_PROPERTY, info->format, &row);
	}
	if (info->fields_mask & EV_DOCUMENT_INFO_N_PAGES) {
		text = g_strdup_printf ("%d", info->n_pages);
		set_property (properties, GTK_GRID (grid), N_PAGES_PROPERTY, text, &row);
		g_free (text);
	}
	if (info->fields_mask & EV_DOCUMENT_INFO_LINEARIZED) {
		set_property (properties, GTK_GRID (grid), LINEARIZED_PROPERTY, info->linearized, &row);
	}
	if (info->fields_mask & EV_DOCUMENT_INFO_SECURITY) {
		set_property (properties, GTK_GRID (grid), SECURITY_PROPERTY, info->security, &row);
	}
	if (info->fields_mask & EV_DOCUMENT_INFO_PAPER_SIZE) {
		text = hwp_regular_paper_size (info);
		set_property (properties, GTK_GRID (grid), PAPER_SIZE_PROPERTY, text, &row);
		g_free (text);
	}
	*/
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
