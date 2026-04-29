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
#ifndef __PROPS_DATA_H__
#define __PROPS_DATA_H__

#include <gsf/gsf-doc-meta-data.h>

typedef void (*PropItemCallback) (const char *name, const char *value, gpointer user_data);

void props_data_for_each(const char *uri, const char* mime_type, PropItemCallback callback, gpointer user_data);

#endif /* __PROPS_DATA_H__ */
