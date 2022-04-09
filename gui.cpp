#include <curses.h>
#include <signal.h>
#include <thread>
#include <iostream>
#include "gui.h"

std::string prompts[4] = { 
    "> First name: ",
    "> Last name: ",
    "> A number: ",
    "> UAH email: "
};

WINDOW *prompt = NULL;
WINDOW *code = NULL;

void start_gui(void)
{
    initscr();
    keypad(stdscr, TRUE);

    prompt = newwin(30, 80, 0, 0);
    code   = newwin(30, 50, 0, 80);

    resize_handler(0);
    signal(SIGWINCH, resize_handler);

    //create new thread to spawn handle_prompt
    std::thread prompt_thread(handle_prompt, prompt);
    //create new thread to spawn handle_code
    std::thread code_thread(handle_code, code);

    //wait on threads
    prompt_thread.join();
    code_thread.join();
}

void handle_prompt(WINDOW *win)
{
    (void)win;
    while (true) {
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
}

void handle_code(WINDOW *win)
{
    (void)win;
    while (true) {
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
}

void resize_handler(int sig)
{
    (void)sig;
    resize_term(0, 0);

    int nh, nw;
    getmaxyx(stdscr, nh, nw);

    int middle = nw / 3 * 2;

    wresize(prompt, 30, middle);
    wresize(code, 30, nw - middle);

    wclear(prompt);
    box(prompt, 0, 0);

    wclear(code);
    box(code, 0, 0);

    //print prompt
    for (int i = 0; i < 4; i++)
        mvwprintw(prompt, i + 1, 1, prompts[i].c_str());

    touchwin(stdscr);

    wrefresh(prompt);
    wrefresh(code);

    std::cout << nh << ", " << nw << std::endl;
}