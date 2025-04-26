#include <curses.h>
#include <fstream>
#include <string>
#include <vector>

struct EditorState {
    std::vector<std::string> lines;
    int cursorX, cursorY;
};

std::vector<EditorState> undo;
std::vector<EditorState> redo;

void initUI() {
    initscr();              // Start curses mode
    cbreak();               // Line buffering disabled
    noecho();               // Don't echo() while we do getch
    keypad(stdscr, TRUE);   // Enable arrow keys, etc.
}

void shutdownUI() {
    endwin();               // End curses mode
}

void updateStatusBar(const char* filename, int x, int y) {
    attron(A_REVERSE);
    mvprintw(LINES - 1, 0, "Editing: %s | Line: %d, Col: %d", filename, y - 2, x);
    clrtoeol();
    attroff(A_REVERSE);
    move(y, x);
}

void createFile() {
    clear();
    printw("Enter file name to create and edit: ");
    echo();
    char filename[100];
    getstr(filename);
    noecho();

    clear();
    printw("== Editing '%s' ==\n(ESC to save & exit)\n\n", filename);

    std::vector<std::string> lines(1);
    bool mfied =false;
    int y = 3, x = 0;
    move(y, x);

    int ch;
    while ((ch = getch()) != 27) { // ESC to exit
        switch (ch) {

                case 26:
                    if(!undo.empty()){
                        redo.push_back({lines, x, y});
                        auto stat = undo.back();
                        undo.pop_back();
                        lines = stat.lines;
                        x=stat.cursorX;
                        y=stat.cursorY;
                        clear();
                        for (size_t i = 0; i < lines.size(); ++i)
                            mvprintw(3 + i, 0, lines[i].c_str());
                    }
                    break;
                case 25:
                    if(!redo.empty()){
                        undo.push_back({lines, x, y});
                        auto stat = redo.back();
                        redo.pop_back();
                        lines = stat.lines;
                        x=stat.cursorX;
                        y=stat.cursorY;
                        clear();
                        for (size_t i = 0; i < lines.size(); ++i)
                            mvprintw(3 + i, 0, lines[i].c_str());  
                    }
                    break;

            case 19:{
                std::ofstream file(filename);
                for(const auto& line: lines)
                file<< line<< std::endl;
                mfied = false;
                mvprintw(LINES-2, 0, "File saved!      ");
            }
            break;

            case KEY_BACKSPACE:
            case 127:
            case 8:
                undo.push_back({lines, x, y});
                redo.clear();
                if (x > 0) {
                    lines[y - 3].erase(x - 1, 1);
                    mvdelch(y, --x);
                } else if (y > 3) {
                    x = lines[y - 4].length();
                    lines[y - 4] += lines[y - 3];
                    lines.erase(lines.begin() + (y - 3));
                    move(--y, x);
                    updateStatusBar(filename, x, y);
                    clrtoeol();
                    printw("%s", lines[y - 3].c_str());
                }
                break;

            case KEY_LEFT:
                if (x > 0) {
                    --x;
                } else if (y > 3) {
                    --y;
                    x = lines[y - 3].length();
                }
                break;

            case KEY_RIGHT:
                if (x < lines[y - 3].length()) {
                    ++x;
                } else if (y - 2 < lines.size()) {
                    ++y;
                    x = 0;
                }
                break;

            case KEY_UP:
                if (y > 3) {
                    --y;
                    x = std::min<int>(x, lines[y - 3].length());
                }
                break;

            case KEY_DOWN:
                if (y - 2 < lines.size()) {
                    ++y;
                    x = std::min<int>(x, lines[y - 3].length());
                }
                break;

            case '\n':
                undo.push_back({lines, x, y});
                redo.clear();
                {
                    std::string remaining = lines[y - 3].substr(x);
                    lines[y - 3] = lines[y - 3].substr(0, x);
                    lines.insert(lines.begin() + (y - 2), remaining);
                    x = 0;
                    ++y;
                    clrtoeol();
                }
                break;

            default:
                if (isprint(ch)) {
                    undo.push_back({lines, x, y});
                    redo.clear();
                    lines[y - 3].insert(x, 1, (char)ch);
                    insch(ch);
                    ++x;
                    mfied = true;
                }
                break;
        }
        
        move(y, x);
        updateStatusBar(filename, x, y);
    }

    // Save the file
    std::ofstream file(filename);
    for (const auto& line : lines)
        file << line << '\n';

    printw("\n\nFile saved successfully!\nPress any key to return to menu...");
    getch();
}


void editFile(const char* filename){
    std::ifstream file(filename);
    std::vector<std::string> lines;
    std::string line;
    bool mfied=false;

    while(std::getline(file, line)){
        lines.push_back(line);
    }
    if(lines.empty())lines.push_back("");
    clear();
    printw("###EDITING (%s)###\n(ESC to save and exit)\n-----------------\n",filename);
    int y = 3, x = 0;
    int ch;
    for(size_t i=0; i<lines.size(); ++i){
        mvprintw(3+i,0,lines[i].c_str());
        updateStatusBar(filename, x, y);
    }
    move(y,x);
    updateStatusBar(filename, x, y);

    while ((ch = getch()) != 27) { // ESC to exit
        switch (ch) {

            case 26:
                if(!undo.empty()){
                    redo.push_back({lines, x, y});
                    auto stat = undo.back();
                    undo.pop_back();
                    lines = stat.lines;
                    x=stat.cursorX;
                    y=stat.cursorY;
                    clear();
                    for (size_t i = 0; i < lines.size(); ++i)
                        mvprintw(3 + i, 0, lines[i].c_str());
                }
                break;
            case 25:
                if(!redo.empty()){
                    undo.push_back({lines, x, y});
                    auto stat = redo.back();
                    redo.pop_back();
                    lines = stat.lines;
                    x=stat.cursorX;
                    y=stat.cursorY;
                    clear();
                    for (size_t i = 0; i < lines.size(); ++i)
                        mvprintw(3 + i, 0, lines[i].c_str());
                }
                break;

            case 19:{
                    std::ofstream file(filename);
                    for(const auto& line: lines)
                    file<< line<< std::endl;
                    mfied = false;
                    mvprintw(LINES-2, 0, "File saved!      ");
                    move(y,x);
                }
                break;

            case KEY_BACKSPACE:
            case 127:
            case 8:
            undo.push_back({lines, x, y});
            redo.clear();
            if (x > 0) {
                lines[y - 3].erase(x - 1, 1);
                mvdelch(y, --x);
            } else if (y > 3) {
                x = lines[y - 4].length();
                lines[y - 4] += lines[y - 3];
                lines.erase(lines.begin() + (y - 3));
                --y;
                clrtoeol();
                printw("%s", lines[y - 3].c_str());
            }
            break;

            case KEY_LEFT:
                if (x > 0) {
                    --x;
                } else if (y > 3) {
                    --y;
                    x = lines[y - 3].length();
                }
                break;

            case KEY_RIGHT:
                if (x < lines[y - 3].length()) {
                    ++x;
                } else if (y - 2 < lines.size()) {
                    ++y;
                    x = 0;
                }
                break;

            case KEY_UP:
                if (y > 3) {
                    --y;
                    x = std::min<int>(x, lines[y - 3].length());
                }
                break;

            case KEY_DOWN:
                if (y - 2 < lines.size()) {
                    ++y;
                    x = std::min<int>(x, lines[y - 3].length());
                }
                break;

            case '\n':
                undo.push_back({lines, x, y});
                redo.clear();
                {
                    std::string remaining = lines[y - 3].substr(x);
                    lines[y - 3] = lines[y - 3].substr(0, x);
                    lines.insert(lines.begin() + (y - 2), remaining);
                    x = 0;
                    ++y;
                    clrtoeol();
                }
                break;

            default:
                if (isprint(ch)) {
                    
                undo.push_back({lines, x, y});
                redo.clear();
                    lines[y - 3].insert(x, 1, (char)ch);
                    insch(ch);
                    ++x;
                    mfied = true;
                }
                break;
        }
        
        move(y, x);
        updateStatusBar(filename, x, y);
    }
    std::ofstream outfile(filename);
    for(const auto& l: lines){
        outfile << l << std::endl;
    }
    printw("Press any key to exit....");
    getch();
}


void openFile(){
    clear();
    printw("Enter file name to open: ");
    char filename[100];
    echo();
    getstr(filename);
    noecho();

    std::ifstream file(filename);
    if(!file){
        printw("failed to open ...%s ",filename);
        printw("Press any key to continue");
        getch();
        return;
    }
    clear();
    printw("\nCONTENTS OF THE FILE ... %s ",filename);
    printw("\n---------------------------------");
    std::string line;
    int lcount = 0;
    while(std::getline(file,line) && LINES-2){
        printw("\n%s",line.c_str());
        lcount++;
    }
    printw("\nDo you wanna edit the file..?(Y/N)");
    char choice;
    echo();
    choice = getch();
    if(choice =='Y' || choice == 'y'){
        editFile(filename);
    }
    printw("\nPress any key to continue...");
    getch();
}


void showMenu() {
    int choice;
    while (true) {
        clear();
        printw("== Text Editor ==\n");
        printw("1. Create New File\n");
        printw("2. Open Existing File\n");
        printw("3. Exit\n");
        printw("Choose an option: ");

        echo();
        choice = getch();
        // noecho();

        if (choice == '1') {
            createFile();
        }
        else if (choice == '2'){
            openFile();
        } else if (choice == '3') {
            break;
        }
    }
}

int main() {
    initUI();
    showMenu();
    shutdownUI();
    return 0;
}
