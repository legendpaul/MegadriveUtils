#ifndef TEST_SCROLLING_H
#define TEST_SCROLLING_H

#include <genesis.h> // For SGDK types if used in function signatures

void scrolling_test_init();
void scrolling_test_update();
void scrolling_test_on_exit(); // Optional, if specific cleanup is needed beyond return_to_menu

#endif // TEST_SCROLLING_H
