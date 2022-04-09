#include <curses.h>
#include <string>
#include <vector>

void start_gui(void);
void handle_prompt(WINDOW *win);
void handle_code(WINDOW *win);
void resize_handler(int sig);

void read_file(std::vector<std::string>& output, const char *filename);