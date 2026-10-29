#include <stdio.h>

#include <SDL2/SDL.h>

#include "sdlw.h"

#define UNIT_TEST_FAIL \
	printf("Unit test '%s' failed.\n", __func__); \
	return false;
#define UNIT_TEST_PASS \
	printf("Unit test '%s' passed.\n", __func__); \
	return true;

#define UNIT_TEST_FUNCTION_AMOUNT 16

#define SUITE_NAME_SDLW "SDL wrapper"

// TODO Utilize c-logging?

// TODO dedicated file for SDLW tests

bool
unit_test_sdlw_version_is_2(void);
bool
unit_test_sdlw_renderer_info_is_accessible(void);

struct test_suite {
	const char* name;
	bool (*setup)(void);
	void (*teardown)(void);
	bool (**unit_test_functions)(void);
};

bool (*functions_unit_test_sdlw[UNIT_TEST_FUNCTION_AMOUNT])(void) = {
	unit_test_sdlw_version_is_2,
	unit_test_sdlw_renderer_info_is_accessible,
	NULL
};

static bool
suite_setup_sdlw(void)
{
	bool performance_success;

	performance_success = false;

	performance_success = sdlw_init();

	return performance_success;
}

static void
suite_teardown_sdlw(void)
{
	sdlw_uninitialize();
}

/*
 * Return value of all unit_test_* functions: did the test pass?
 */

bool
unit_test_sdlw_version_is_2(void)
{
	SDL_version version;

	sdlw_fill_version(&version);
	if (2 == version.major) {
		UNIT_TEST_PASS
	}

	printf("version.major is: %d\n", version.major);
	UNIT_TEST_FAIL
}

bool
unit_test_sdlw_renderer_info_is_accessible(void)
{
	SDL_RendererInfo renderer_info = {0};

	bool performance_success;

	performance_success = false;

	performance_success = sdlw_fill_main_renderer_info(&renderer_info);
	if (true == performance_success) {
		UNIT_TEST_PASS
	}

	UNIT_TEST_FAIL
}

/*
 * Return value: did all of the tests pass?
 */
bool
run_unit_tests(bool (*functions_unit_test[UNIT_TEST_FUNCTION_AMOUNT])(void))
{
	uint8_t i;
	uint8_t tests_ran;
	bool test_passed;
	bool all_tests_passed;
	bool (*unit_test)(void);

	all_tests_passed = false;
	test_passed = false;
	unit_test = NULL;
	tests_ran = 0;
	i = 0;

	for (i = 0; i < UNIT_TEST_FUNCTION_AMOUNT; i++) {
		unit_test = functions_unit_test[i];

		if (NULL == unit_test) {
			all_tests_passed = true;
			break;
		}

		test_passed = unit_test();
		tests_ran++;
		if (false == test_passed) {
			break;
		}
	}

	if (i == UNIT_TEST_FUNCTION_AMOUNT) {
		all_tests_passed = true;
	}

	printf("Unit tests ran: %d\n", tests_ran);
	return all_tests_passed;
}

bool
run_test_suite(struct test_suite const * const test_suite)
{
	bool unit_tests_passed;

	unit_tests_passed = false;

	printf("Running test suite: %s\n", test_suite->name);

	test_suite->setup();
	unit_tests_passed = run_unit_tests(test_suite->unit_test_functions);
	test_suite->teardown();

	if (false == unit_tests_passed) {
		printf("Unit tests of suite '%s' failed.\n", test_suite->name);
	}

	return unit_tests_passed;
}

int
main(void)
{
	struct test_suite test_suite_sdlw;
	bool test_suite_passed = false;

	test_suite_sdlw.name = SUITE_NAME_SDLW;
	test_suite_sdlw.setup = suite_setup_sdlw;
	test_suite_sdlw.teardown = suite_teardown_sdlw;
	test_suite_sdlw.unit_test_functions = functions_unit_test_sdlw;

	printf("Running unit tests.\n");

	test_suite_passed = run_test_suite(&test_suite_sdlw);
	if (false == test_suite_passed) {
		goto fail;
	}

	printf("All test suites passed!\n");
	return 0;

fail:
	printf("Some test suite failed\n");
	return 1;
}
