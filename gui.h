#include <curses.h>

void start_gui(void);
void handle_prompt(WINDOW *win);
void handle_code(WINDOW *win);
void resize_handler(int sig);