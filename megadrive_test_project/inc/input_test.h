#ifndef INPUT_TEST_H
#define INPUT_TEST_H

#include <genesis.h>

void input_test_init_display(); // Called once when test starts
void input_test_update_display(u16 joy_state); // Called each frame of the test

#endif // INPUT_TEST_H
