/*
 * Copyright (c) 2010 Red Hat, Inc.
 *
 * All rights reserved.
 *
 * Author: Angus Salkeld <asalkeld@redhat.com>
 *
 * This file is part of libqb.
 *
 * libqb is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 2.1 of the License, or
 * (at your option) any later version.
 *
 * libqb is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with libqb.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "os_base.h"
#include <check.h>

#include <qb/qbdefs.h>
#include <qb/qbutil.h>
#include <qb/qbarray.h>

struct test_my_st {
	int32_t a;
	int32_t b;
	int32_t c;
	int32_t d;
};


START_TEST(test_array_limits)
{
	qb_array_t *a;
	int32_t res;
	struct test_my_st *st;

	a = qb_array_create(INT_MAX, sizeof(struct test_my_st));
	fail_unless(a == NULL);
	a = qb_array_create(-56, sizeof(struct test_my_st));
	fail_unless(a == NULL);
	a = qb_array_create(67, 0);
	fail_unless(a == NULL);

	/* working array */
	a = qb_array_create(10, sizeof(struct test_my_st));
	fail_if(a == NULL);

	/* out-of-bounds */
	res = qb_array_index(a, 10, (void**)&st);
	ck_assert_int_eq(res, -EINVAL);
	res = qb_array_index(a, -10, (void**)&st);
	ck_assert_int_eq(res, -EINVAL);
	res = qb_array_index(NULL, 1, (void**)&st);
	ck_assert_int_eq(res, -EINVAL);
	res = qb_array_index(a, -10, NULL);
	ck_assert_int_eq(res, -EINVAL);

	qb_array_free(a);
}
END_TEST

START_TEST(test_array_correct_retrieval)
{
	qb_array_t *a;
	int32_t i;
	int32_t res;
	struct test_my_st *st;

	a = qb_array_create(112, sizeof(struct test_my_st));

	for (i = 0; i < 112; i++) {
		res = qb_array_index(a, i, (void**)&st);
		ck_assert_int_eq(res, 0);
		st->a = i;
		st->b = i+1;
		st->c = i+2;
		st->d = i+3;
	}
	/* read back */
	for (i = 0; i < 112; i++) {
		res = qb_array_index(a, i, (void**)&st);
		ck_assert_int_eq(res, 0);
		ck_assert_int_eq(st->a, i);
		ck_assert_int_eq(st->b, i+1);
		ck_assert_int_eq(st->c, i+2);
		ck_assert_int_eq(st->d, i+3);
	}

	qb_array_free(a);
}
END_TEST

START_TEST(test_array_static_memory)
{
	qb_array_t *a;
	int32_t res;
	struct test_my_st *st_old;
	struct test_my_st *st;

	a = qb_array_create(112, sizeof(struct test_my_st));

	res = qb_array_index(a, 99, (void**)&st_old);
	ck_assert_int_eq(res, 0);

	res = qb_array_grow(a, 1453);
	ck_assert_int_eq(res, 0);

	res = qb_array_index(a, 345, (void**)&st);
	st->a = 411;

	/* confirm the pointer is the same after a grow */
	res = qb_array_index(a, 99, (void**)&st);
	ck_assert_int_eq(res, 0);
	fail_unless(st == st_old);

	qb_array_free(a);
}
END_TEST

static Suite *rb_suite(void)
{
	TCase *tc;
	Suite *s = suite_create("qb_array");

	tc = tcase_create("limits");
	tcase_add_test(tc, test_array_limits);
	suite_add_tcase(s, tc);

	tc = tcase_create("correct_retrieval");
	tcase_add_test(tc, test_array_correct_retrieval);
	suite_add_tcase(s, tc);

	tc = tcase_create("static_memory");
	tcase_add_test(tc, test_array_static_memory);
	suite_add_tcase(s, tc);

	return s;
}

static void libqb_log_fn(const char *file_name,
			 int32_t file_line, int32_t severity, const char *msg)
{
	printf("libqb: %s:%d %s\n", file_name, file_line, msg);
}

int32_t main(void)
{
	int32_t number_failed;

	Suite *s = rb_suite();
	SRunner *sr = srunner_create(s);

	qb_util_set_log_function(libqb_log_fn);

	srunner_run_all(sr, CK_NORMAL);
	number_failed = srunner_ntests_failed(sr);
	srunner_free(sr);
	return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}