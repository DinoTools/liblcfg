/*
  Copyright (c) 2012, Paul Baecher
  All rights reserved.

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are met:
      * Redistributions of source code must retain the above copyright
        notice, this list of conditions and the following disclaimer.
      * Redistributions in binary form must reproduce the above copyright
        notice, this list of conditions and the following disclaimer in the
        documentation and/or other materials provided with the distribution.
      * Neither the name of the <organization> nor the
        names of its contributors may be used to endorse or promote products
        derived from this software without specific prior written permission.
  
  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  DISCLAIMED. IN NO EVENT SHALL <THE COPYRIGHT HOLDER> BE LIABLE FOR ANY
  DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#include <string.h>
#include <stdio.h>

#include <lcfgx/lcfgx_tree.h>

static struct lcfgx_tree_node *lcfgx_tree_node_new(enum lcfgx_type type, const char *key)
{
	struct lcfgx_tree_node *node = malloc(sizeof(struct lcfgx_tree_node));

	node->type = type;

	if( key != NULL )
		node->key = strdup(key);
	else
		node->key = NULL;

	node->next = NULL;

	return node;
}


void lcfgx_tree_dump(struct lcfgx_tree_node *node, int depth)
{
//	printf("%s node %p node->key %s depth %i\n", __PRETTY_FUNCTION__, node, node->key, depth);
	void sp(int n)
	{
		int i;

		for( i = 0; i < n; i++ )
			printf("%c", ' ');
	}

	sp(depth);
	if( node->key != NULL )
		printf("%s", node->key);
	else
		printf("%s", "(none)");

	struct lcfgx_tree_node *n;

	switch( node->type )
	{
		case lcfgx_string:
			printf(" = \"%s\"\n", (char *)node->value.string.data);
			break;

		case lcfgx_list:
		case lcfgx_map:
			printf("%c", '\n');
			n = node->value.elements;
			for( ; n != NULL; n = n->next )
				lcfgx_tree_dump(n, depth + 2);
			break;
	}
}

static void lcfgx_tree_insert(int pathc, char **pathv, void *data, size_t len, struct lcfgx_tree_node *node)
{
	struct lcfgx_tree_node *n;

	for( n = node->value.elements; n != NULL; n = n->next )
		if( !strcmp(pathv[0], n->key) )
			break;

	if( pathc == 1 )
	{
		/* leaf node --> string value */
		if( n == NULL )
		{
			/* not found, insert */
			n = lcfgx_tree_node_new(lcfgx_string, pathv[0]);
			n->value.string.len = len;
			n->value.string.data = malloc(len+1);
			memset(n->value.string.data, 0, len+1);
			memcpy(n->value.string.data, data, len);
			n->next = NULL;
			if ( node->value.elements != NULL )
			{
				struct lcfgx_tree_node *end = node->value.elements;
				while (end->next != NULL)
					end = end->next;
				end->next = n;
			}else
			{
				node->value.elements = n;
			}
		}
	}
	else
	{
		/* inner node --> (map/list) */
		if( n == NULL )
		{
			/* not found, insert it */
			n = lcfgx_tree_node_new(lcfgx_map, pathv[0]);
			n->value.elements = NULL;
			n->next = NULL;
			if ( node->value.elements != NULL )
			{
				struct lcfgx_tree_node *end = node->value.elements;
				while (end->next != NULL)
					end = end->next;
				end->next = n;
			}else
			{
				node->value.elements = n;
			}
		}

		/* recurse into map/list */
		lcfgx_tree_insert(pathc - 1, &pathv[1], data, len, n);
	}
}

enum lcfg_status lcfgx_tree_visitor(const char *key, void *data, size_t len, void *user_data)
{
	struct lcfgx_tree_node *root = user_data;
	char path[strlen(key) + 1];
	int path_components = 1;

	strncpy(path, key, strlen(key) + 1);

	while( *key != 0 )
		if( *key++ == '.' )
			path_components++;

	char *pathv[path_components];
	char *token;
	char *saveptr = NULL;
	int pathc = 0;

	while( (token = strtok_r(pathc == 0 ? path : NULL, ".", &saveptr)) != NULL )
		pathv[pathc++] = token;

	lcfgx_tree_insert(pathc, pathv, data, len, root);

	return lcfg_status_ok;
}



void lcfgx_correct_type(struct lcfgx_tree_node *node)
{
	if( node->type == lcfgx_string )
		return;

	struct lcfgx_tree_node *n = NULL;
	if( node->key == NULL )	/* root node */
		n = node;

	if( node->type == lcfgx_map || node->type == lcfgx_list )
		n = node->value.elements;
	
	if( n == NULL )
		return;

	/* child key is integer, we have a list */
	char *end;
	if( strtol(n->key, &end, 10) >= 0 && n->key != end )
		node->type = lcfgx_list;

	struct lcfgx_tree_node *it;
	for( it = n; it != NULL; it = it->next )
		lcfgx_correct_type(it);
}

struct lcfgx_tree_node *lcfgx_tree_new(struct lcfg *c)
{
	struct lcfgx_tree_node *root = lcfgx_tree_node_new(lcfgx_map, NULL);

	root->value.elements = NULL;

	lcfg_accept(c, lcfgx_tree_visitor, root);
	lcfgx_correct_type(root);
	return root;
}

void lcfgx_tree_delete(struct lcfgx_tree_node *n)
{

	if( n->type != lcfgx_string )
	{
		struct lcfgx_tree_node *m, *next;

		for( m = n->value.elements; m != NULL; )
		{
			next = m->next;
			lcfgx_tree_delete(m);
			m = next;
		}
	}
	else
	{
		free(n->value.string.data);
	}

	if( n->key != NULL )
		free(n->key);

	free(n);
}

const char *lcfgx_path_access_strings[] =
{
	"LCFGX_PATH_NOT_FOUND",
	"LCFGX_PATH_FOUND_WRONG_TYPE_BAD",
	"LCFGX_PATH_FOUND_TYPE_OK",
};

struct lcfgx_tree_node *cfg_get_recursive(struct lcfgx_tree_node *node, int pathc, char **pathv)
{
	struct lcfgx_tree_node *it;// = node;

	for( it = node->value.elements; it != NULL; it = it->next )
	{
		if( strcmp(pathv[0], it->key) == 0 )
			break;
	}

	if( it != NULL )
	{
		if( pathc == 1 )
			return it;
		else
			return cfg_get_recursive(it, pathc - 1, &pathv[1]);
	}
	else
		return NULL;
}


enum lcfgx_path_access lcfgx_get(struct lcfgx_tree_node *root, struct lcfgx_tree_node **n, const char *key, enum lcfgx_type type)
{
	char path[strlen(key) + 1];
	int path_components = 1;

	strncpy(path, key, strlen(key) + 1);

	while( *key != 0 )
		if( *key++ == '.' )
			path_components++;

	char *pathv[path_components];
	char *token;
	char *saveptr = NULL;
	int pathc = 0;

	while( (token = strtok_r(pathc == 0 ? path : NULL, ".", &saveptr)) != NULL )
		pathv[pathc++] = token;


	struct lcfgx_tree_node *node;

	if( pathc == 0 )
		node = root;
	else
		node = cfg_get_recursive(root, pathc, pathv);

	if( node == NULL )
		return LCFGX_PATH_NOT_FOUND;

	if( node->type != type )
		return LCFGX_PATH_FOUND_WRONG_TYPE_BAD;

	*n = node;

	return LCFGX_PATH_FOUND_TYPE_OK;
}

enum lcfgx_path_access lcfgx_get_list(struct lcfgx_tree_node *root, struct lcfgx_tree_node **n, const char *key)
{
	return lcfgx_get(root, n, key, lcfgx_list);
}

enum lcfgx_path_access lcfgx_get_map(struct lcfgx_tree_node *root, struct lcfgx_tree_node **n, const char *key)
{
	return lcfgx_get(root, n, key, lcfgx_map);
}

enum lcfgx_path_access lcfgx_get_string(struct lcfgx_tree_node *root, struct lcfgx_tree_node **n, const char *key)
{
	return lcfgx_get(root, n, key, lcfgx_string);
}

