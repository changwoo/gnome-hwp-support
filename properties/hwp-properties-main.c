/*
 * Copyright (C) 2000, 2001 Eazel Inc.
 * Copyright (C) 2003  Andrew Sobala <aes@gnome.org>
 * Copyright (C) 2005  Bastien Nocera <hadess@hadess.net>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 * The Ev project hereby grant permission for non-gpl compatible GStreamer
 * plugins to be used and distributed together with GStreamer and Ev. This
 * permission are above and beyond the permissions granted by the GPL license
 * Ev is covered by.
 *
 * Monday 7th February 2005: Christian Schaller: Add excemption clause.
 * See license_change file for details.
 *
 */

#include <config.h>

#include <string.h>

#include <glib/gi18n-lib.h>
#include <gtk/gtk.h>

#include <libnautilus-extension/nautilus-extension-types.h>
#include <libnautilus-extension/nautilus-property-page-provider.h>

#include <gsf/gsf-utils.h>
#include "props-data.h"
#include "hwp-properties-view.h"

static GType epp_type = 0;
static void property_page_provider_iface_init
	(NautilusPropertyPageProviderIface *iface);
static GList *hwp_properties_get_pages
	(NautilusPropertyPageProvider *provider, GList *files);

static void
hwp_properties_plugin_register_type (GTypeModule *module)
{
	const GTypeInfo info = {
		sizeof (GObjectClass),
		(GBaseInitFunc) NULL,
		(GBaseFinalizeFunc) NULL,
		(GClassInitFunc) NULL,
		NULL,
		NULL,
		sizeof (GObject),
		0,
		(GInstanceInitFunc) NULL
	};
	const GInterfaceInfo property_page_provider_iface_info = {
		(GInterfaceInitFunc)property_page_provider_iface_init,
		NULL,
		NULL
	};

	epp_type = g_type_module_register_type (module, G_TYPE_OBJECT,
			"HwpPropertiesPlugin",
			&info, 0);
	g_type_module_add_interface (module,
			epp_type,
			NAUTILUS_TYPE_PROPERTY_PAGE_PROVIDER,
			&property_page_provider_iface_info);
}

static void
property_page_provider_iface_init (NautilusPropertyPageProviderIface *iface)
{
	iface->get_pages = hwp_properties_get_pages;
}

static GList *
hwp_properties_get_pages (NautilusPropertyPageProvider *provider,
			 GList *files)
{
	GError *error = NULL;
	GsfDocMetaData *meta_data;
	GList *pages = NULL;
	NautilusFileInfo *file;
	gchar *uri = NULL;
	gchar *mime_type = NULL;
	GtkWidget *page, *label;
	NautilusPropertyPage *property_page;

	/* only add properties page if a single file is selected */
	if (files == NULL || files->next != NULL)
		goto end;
	file = files->data;

	/* okay, make the page */
	uri = nautilus_file_info_get_uri (file);
	mime_type = nautilus_file_info_get_mime_type (file);
	
	meta_data = props_data_read (uri, &error);
	if (!meta_data)
		goto end;

	label = gtk_label_new (_("Document"));
	page = hwp_properties_view_new (uri);
	hwp_properties_view_set_info (HWP_PROPERTIES_VIEW (page),
				      meta_data);
	gtk_widget_show (page);
	property_page = nautilus_property_page_new ("document-properties",
			label, page);
	g_object_unref (meta_data);

	pages = g_list_prepend (pages, property_page);

end:
	g_free (uri);
	g_free (mime_type);
	
	return pages;
}

/* --- extension interface --- */
void
nautilus_module_initialize (GTypeModule *module)
{
	hwp_properties_plugin_register_type (module);
	hwp_properties_view_register_type (module);

	gsf_init();
}

void
nautilus_module_shutdown (void)
{
	gsf_shutdown();
}

void
nautilus_module_list_types (const GType **types,
                            int          *num_types)
{
	static GType type_list[1];

	type_list[0] = epp_type;
	*types = type_list;
	*num_types = G_N_ELEMENTS (type_list);
}
