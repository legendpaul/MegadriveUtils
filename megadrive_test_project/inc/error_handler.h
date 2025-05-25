#ifndef ERROR_HANDLER_H
#define ERROR_HANDLER_H

#include <genesis.h> // For u16

// Displays an error message on screen and halts program execution.
// Formats message as:
// ERROR IN:
// MODULE: <module_name>
// FUNC: <function_name>
// LINE: <line_number>
// MSG: <error_message>
void error_handler_display_error(const char* module_name, 
                                 const char* function_name, 
                                 u16 line_number, 
                                 const char* error_message);

#endif // ERROR_HANDLER_H
