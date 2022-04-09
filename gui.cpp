#include <curses.h>
#include <signal.h>
#include <thread>
#include <stdio.h>
#include <fstream>
#include <string>
#include <vector>
#include <mutex>
#include "gui.h"

std::string prompts[4] = { 
    "> First name: ",
    "> Last name: ",
    "> A number: ",
    "> UAH email: "
};

WINDOW *prompt = NULL;
WINDOW *code = NULL;

struct {
    int x;
    int y;
} curser_loc = { 0, 0 };

std::mutex mtx;

std::vector<std::string> uah;
std::vector<std::string> acm;
std::vector<std::string> source;

void start_gui(void)
{
    initscr();
    keypad(stdscr, TRUE);

    signal(SIGWINCH, resize_handler);

    read_file(uah, "./images/uah logo.txt");
    read_file(acm, "./images/acm logo.txt");
    read_file(source, "./gui.cpp");

    resize_handler(0);

    std::thread prompt_thread(handle_prompt, prompt);
    std::thread code_thread(handle_code, code);

    //wait on threads
    prompt_thread.join();
    code_thread.join();
}

void handle_prompt(WINDOW *win)
{
    (void)win;
    std::string curr_string;
    int line = 0;
    int col = 0;
    curser_loc = { col + int(prompts[line].length()) + 2, line + 1 };

    while (true) {
        /*mtx.lock();
        wmove(prompt, curser_loc.y, curser_loc.x);
        wrefresh(prompt);
        mtx.unlock();*/
        
        char c = wgetch(prompt);
        if (c == '\n' || c == '\t') {
            if (line == 3) {

            }
            else {
                line++;
                col = 0;

                curser_loc = { col + int(prompts[line].length()) + 1, line + 1 };
                mtx.lock();
                wmove(prompt, curser_loc.y, curser_loc.x);
                wrefresh(prompt);
                mtx.unlock();
            }
        }
        curr_string += c;
        col++;
        curser_loc.x++;
    }
}

void handle_code(WINDOW *win)
{
    (void)win;
    int offset = 0;
    while (true) {
        mtx.lock();
        wclear(code);

        for (int i = offset; i - offset < getmaxy(code) - 2; i++) {
            if (i >= int(source.size())) {
                offset = 0;
                break;
            }
            mvwprintw(code, i + 1 - offset, 1, "%s", source[i].c_str());
        }
        
        box(code, 0, 0);
        wrefresh(code);
        offset++;

        wmove(prompt, curser_loc.y, curser_loc.x);
        wrefresh(prompt);
        mtx.unlock();

        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
}

void refresh_prompt() {
    mtx.lock();
    wclear(prompt);
    box(prompt, 0, 0);
    for (int i = 0; i < 4; i++) {
        mvwprintw(prompt, i + 1, 2, "%s", prompts[i].c_str());
    }
    int i = 14;
    for (std::string line : acm)
        mvwprintw(prompt, i++, 60, line.c_str());

    i = 7;
    for (std::string line : uah)
        mvwprintw(prompt, i++, 5, line.c_str());
    wrefresh(prompt);
    mtx.unlock();
}

void resize_handler(int sig)
{
    mtx.lock();
    (void)sig;

    int nh, nw;
    getmaxyx(stdscr, nh, nw);

    int middle = nw / 3 * 2;

    delwin(prompt);
    delwin(code);

    prompt = newwin(nh, middle, 0, 0);
    code   = newwin(nh, nw - middle, 0, middle);

    wclear(code);
    box(code, 0, 0);
    touchwin(stdscr);
    wrefresh(code);
    mtx.unlock();

    refresh_prompt();
}

void read_file(std::vector<std::string>& output, const char *filename) {
    std::ifstream file(filename);
    std::string line;
    while (std::getline(file, line)) {
        output.push_back(line);
    }
}