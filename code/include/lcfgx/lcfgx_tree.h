/*
 * liblcfg - lightweight configuration file library
 * Copyright (c) 2007--2010  Paul Baecher
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 *    $Id$
 *
 */

#ifndef LCFGX_TREE_H
#define LCFGX_TREE_H

#include <lcfg/lcfg.h>

enum lcfgx_type
{
	lcfgx_string,
	lcfgx_list,
	lcfgx_map,
};

struct lcfgx_tree_node
{
	enum lcfgx_type type;
	char *key; /* NULL for root node */

	union
	{
		struct
		{
			void *data;
			size_t len;
		} string;
		struct lcfgx_tree_node *elements; /* in case of list or map type */
	} value;

	struct lcfgx_tree_node *next;
};

struct lcfgx_tree_node *lcfgx_tree_new(struct lcfg *);

void lcfgx_tree_delete(struct lcfgx_tree_node *);
void lcfgx_tree_dump(struct lcfgx_tree_node *node, int depth);

enum lcfgx_path_access
{
	LCFGX_PATH_NOT_FOUND,
	LCFGX_PATH_FOUND_WRONG_TYPE_BAD,
	LCFGX_PATH_FOUND_TYPE_OK,
};

extern const char *lcfgx_path_access_strings[];


enum lcfgx_path_access lcfgx_get(struct lcfgx_tree_node *root, struct lcfgx_tree_node **n, const char *key, enum lcfgx_type type);
enum lcfgx_path_access lcfgx_get_list(struct lcfgx_tree_node *root, struct lcfgx_tree_node **n, const char *key);
enum lcfgx_path_access lcfgx_get_map(struct lcfgx_tree_node *root, struct lcfgx_tree_node **n, const char *key);
enum lcfgx_path_access lcfgx_get_string(struct lcfgx_tree_node *root, struct lcfgx_tree_node **n, const char *key);


#endif

