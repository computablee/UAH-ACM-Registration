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
    start_color();
    init_pair(1, COLOR_WHITE, COLOR_BLACK);
    init_pair(2, COLOR_CYAN, COLOR_BLACK);
    init_pair(3, COLOR_BLUE, COLOR_BLACK);
    keypad(stdscr, TRUE);
    noecho();

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
    std::ofstream outfile;
    outfile.open("./students/students.txt", std::ios_base::app);

    (void)win;
    std::string curr_string[4] = { "", "", "", "" };
    int line = 0;
    int col = 0;
    curser_loc = { col + int(prompts[line].length()) + 2, line + 1 };

    while (true) {
        refresh_prompt();

        mtx.lock();
        for (int i = 0; i < 4; i++) {
            mvwprintw(prompt, i + 1, int(prompts[i].length()) + 2, curr_string[i].c_str());
        }
        wmove(prompt, curser_loc.y, curser_loc.x);
        wrefresh(prompt);
        mtx.unlock();
        
        char c = wgetch(prompt);
        if (c == '\n' || c == '\t') {
            if (line == 3) {
                outfile << curr_string[0] << "," << curr_string[1] << "," << curr_string[2] << "," << curr_string[3] << std::endl;
                for (int i = 0; i < 4; i++)
                    curr_string[i] = "";
                line = 0;
                col = 0;
                curser_loc = { col + int(prompts[line].length()) + 2, line + 1 };
                continue;
            }
            else {
                line++;
                col = 0;

                curser_loc = { col + int(prompts[line].length()) + 2, line + 1 };
                mtx.lock();
                wmove(prompt, curser_loc.y, curser_loc.x);
                wrefresh(prompt);
                mtx.unlock();
            }
        }
        else if (c == 127) {
            if (col > 0) {
                col--;
                curser_loc.x--;
                curr_string[line].erase(col, 1);
            }
        }
        else {
            curr_string[line] += c;
            col++;
            curser_loc.x++;
        }
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

    for (int i = 0; i < 4; i++)
        mvwprintw(prompt, i + 1, 2, "%s", prompts[i].c_str());

    wattron(prompt, COLOR_PAIR(3));
    int i = 14;
    for (std::string line : acm)
        mvwprintw(prompt, i++, 60, line.c_str());

    wattron(prompt, COLOR_PAIR(2));
    i = 7;
    for (std::string line : uah)
        mvwprintw(prompt, i++, 5, line.c_str());

    wattron(prompt, COLOR_PAIR(1));
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