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

#ifndef __HWP_PROPERTIES_VIEW_H__
#define __HWP_PROPERTIES_VIEW_H__

#include <gtk/gtk.h>

#include "props-data.h"

G_BEGIN_DECLS

typedef struct _HwpPropertiesView HwpPropertiesView;
typedef struct _HwpPropertiesViewClass HwpPropertiesViewClass;
typedef struct _HwpPropertiesViewPrivate HwpPropertiesViewPrivate;

#define HWP_TYPE_PROPERTIES			(hwp_properties_view_get_type())
#define HWP_PROPERTIES_VIEW(object)	        (G_TYPE_CHECK_INSTANCE_CAST((object), HWP_TYPE_PROPERTIES, HwpPropertiesView))
#define HWP_PROPERTIES_VIEW_CLASS(klass)		(G_TYPE_CHECK_CLASS_CAST((klass), HWP_TYPE_PROPERTIES, HwpPropertiesViewClass))
#define HWP_IS_PROPERTIES_VIEW(object)		(G_TYPE_CHECK_INSTANCE_TYPE((object), HWP_TYPE_PROPERTIES))
#define HWP_IS_PROPERTIES_VIEW_CLASS(klass)   	(G_TYPE_CHECK_CLASS_TYPE((klass), HWP_TYPE_PROPERTIES))
#define HWP_PROPERTIES_VIEW_GET_CLASS(object) 	(G_TYPE_INSTANCE_GET_CLASS((object), HWP_TYPE_PROPERTIES, HwpPropertiesViewClass))

GType		hwp_properties_view_get_type		(void);
void		hwp_properties_view_register_type	(GTypeModule *module);

GtkWidget      *hwp_properties_view_new			(const gchar          *uri);
void		hwp_properties_view_set_info		(HwpPropertiesView     *properties,
							 const GsfDocMetaData *metadata);

G_END_DECLS

#endif /* __HWP_PROPERTIES_VIEW_H__ */
