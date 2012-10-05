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
#include <stdlib.h>
#include <stdio.h>
#include <check.h>
#include "../include/lcfg/lcfg.h"

struct conf_value
{
	const char *key;
	void *value;
	int value_len;
	int visited;
};

static enum lcfg_status test_visitor(const char *key, void *data, size_t len, void *user_data)
{
	struct conf_value *ref;

	for( ref = user_data; ref->key != NULL; ref++ )
	{
		if( !strcmp(ref->key, key) )
		{
			fail_unless(ref->value_len == len,
				"invalid value length for key \"%s\"; expected %d, got %d",
				ref->key, ref->value_len, len);

			fail_unless(!memcmp(ref->value, data, ref->value_len),
				"invalid value for key \"%s\"", ref->key);

			ref->visited = 1;
			return lcfg_status_ok;
		}
	}

	fail("invalid configuration value \"%s\"", key);

	return lcfg_status_error;
}

static void test_missing(struct conf_value *ref)
{
	struct conf_value *r;

	for( r = ref; r->key != NULL; r++ )
	{
		fail_unless(r->visited == 1, "missing key (in file): \"%s\"", r->key);
	}
}

/* test the example configuration for correct parsing */
START_TEST(test_example)
{
	struct conf_value example_conf[] = {
		{"string-value", "foo", 3, 0},
		{"list_value.0", "a", 1, 0},
		{"list_value.1", "b", 1, 0},
		{"list_value.2", "c", 1, 0},
		{"map-value.foo", "bar", 3, 0},
		{"map-value.bar", "foo", 3, 0},
		{"binary_string", "\0\xff\r\n\0\0\x4a", 7, 0},
		{"winpath", "c:\\windows\\", 11, 0},
		{"nested-list.0.0.0.0", "deep nesting", 12, 0},
		{"a.d.0", "d", 1, 0},
		{"a.d.1.0", "e", 1, 0},
		{"a.d.1.1", "r", 1, 0},
		{"a.d.2", "my index is 2", 13, 0},
		{NULL, NULL, 0, 0}
	};

	struct lcfg *c = lcfg_new("conf/example.conf");

	fail_unless(c != NULL, NULL);
	fail_unless(lcfg_parse(c) == lcfg_status_ok,
		"could not parse file: %s", lcfg_error_get(c));

	fail_unless(lcfg_accept(c, test_visitor, example_conf) == lcfg_status_ok,
		"visitor returned error: %s", lcfg_error_get(c));

	test_missing(example_conf);

	lcfg_delete(c);
}
END_TEST

Suite *liblcfg_suite(void)
{
	Suite *s = suite_create("liblcfg");

	TCase *tc_core = tcase_create("Core");
	tcase_add_test(tc_core, test_example);
	suite_add_tcase(s, tc_core);
	
	return s;
}


int main()
{
	int number_failed;
	Suite *s = liblcfg_suite();
	SRunner *sr = srunner_create(s);
	srunner_run_all (sr, CK_NORMAL);
	number_failed = srunner_ntests_failed (sr);
	srunner_free (sr);
	
	return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
