#include "Menu.h"
#include <iostream>
#include <thread>
#include <stdexcept>
#include <fstream>

Menu::Menu() : menuActions({ "Single Player", "Two Player", "Create Level", "Quit" }), menuLevels({ "level0.txt", "level1.txt", "level2.txt", "level3.txt", "level4.txt", "level5.txt" }), menuActionSelection(0), menuLevelSelection(0) {}

GameMode Menu::Start() {
    return MainMenu();
}

GameMode Menu::MainMenu() {
    GameMode gM = GameMode::NONE; // Default value

    system("clear");
    ShowMenu("Select Game Mode:", menuActions, menuActionSelection);
    while (gM == GameMode::NONE) {
        LeeInput(gM, menuActionSelection, menuActions);

        system("clear");
        ShowMenu("Select Game Mode:", menuActions, menuActionSelection);
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
    return gM;
}

std::string Menu::LevelMenu(Tablero& tab, GameMode gm, int offsetX, int offsetY) {
    std::string s = "";
    system("clear");
    ShowMenu(gm == GameMode::SINGLE_PLAYER ? "Single Player:\n\nSelect Level:" : "Two Player:\n\nSelect Level:", menuLevels, menuLevelSelection);
    ShowLevelSelected(tab, menuLevels[menuLevelSelection], offsetX, offsetY);
    while (s == "") {
        LeeInputToLevel(s, menuLevelSelection, menuLevels);

        system("clear");
        ShowMenu(gm == GameMode::SINGLE_PLAYER ? "Single Player:\n\nSelect Level:" : "Two Player:\n\nSelect Level:", menuLevels, menuLevelSelection);
        ShowLevelSelected(tab, menuLevels[menuLevelSelection], offsetX, offsetY);
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
    return s;
}

void Menu::CreateLevel(Tablero& tab, int offsetX, int offsetY) {
    int width = -1;
    int height = -1;
    while (width < 6 || width > 30) {
        system("clear");
        std::cout << "Introduce level width: ";
        try {
            std::cin >> width;
        }
        catch (...) {
            std::cout << "Width must be a number between 6 and 30\n";
        }
    }

    while (height < 6 || height > 30) {
        system("clear");
        std::cout << "Introduce level height: ";
        try {
            std::cin >> height;
        }
        catch (...) {
            std::cout << "Height must be a number between 6 and 30\n";
        }
    }
    std::cin.ignore();
    std::cin.clear();
    std::cout << "\033[?25h"; // Show cursor
    tab.PlayerCreatesTablero(width, height);
    tab.Render(offsetX, offsetY, true);
    tab.ShowCursoAndInstructions(offsetX, offsetY);

    bool endOfCreating = false;
    char c = ' ';
    while (!endOfCreating && c != 'q') {
        c = LeeInputCreator(tab, endOfCreating);
        if (endOfCreating) {
            while (c != 's' && c != 'n') {
                system("clear");
                std::cout << "Save Level and Exit? [s/n] ";
                try {
                    std::cin >> c;
                }
                catch (...) {
                    std::cout << "Type s to accept or n to refuse\n";
                    c = ' ';
                }
            }
            if (c == 's') {
                endOfCreating = true;
            }
        }
        else if (c != 'q') {
            system("clear");
            tab.Render(offsetX, offsetY, true);
            tab.ShowCursoAndInstructions(offsetX, offsetY);
        }
        // Si 'q'
        std::cin.ignore();
        std::cin.clear();
        std::this_thread::sleep_for(std::chrono::milliseconds(80));
    }
    std::cout << "\033[?25l"; // Hide cursor

    if (c != 'q') {
        std::cout << "Name your file: ";
        try {
            std::string s;
            std::getline(std::cin, s);
            GenerateFile(s, tab);
            menuLevels.push_back(s + ".txt");
        }
        catch (...) {
            throw std::runtime_error("The format or the name of the file couldn't be saved");
        }
    }
}

char Menu::LeeInputCreator(Tablero& tab, bool& b) {
    b = false;
    char c = ' ';
    std::string tecla;
    std::cin >> tecla;
    std::transform(tecla.begin(), tecla.end(), tecla.begin(), ::toupper);
    if (tecla == "W" || tecla == "UPARROW") {
        tab.MoveCursor(Coor(0, -1));
    }
    else if (tecla == "S" || tecla == "DOWNARROW") {
        tab.MoveCursor(Coor(0, 1));
    }
    else if (tecla == "A" || tecla == "LEFTARROW") {
        tab.MoveCursor(Coor(-1, 0));
    }
    else if (tecla == "D" || tecla == "RIGHTARROW") {
        tab.MoveCursor(Coor(1, 0));
    }
    else if (tecla == "SPACEBAR" || tecla == "BACKSPACE") {
        b = true;
    }
    else if (tecla == "D0") {
        tab.ApplyBoxChange(0);
    }
    else if (tecla == "D1") {
        tab.ApplyBoxChange(1);
    }
    else if (tecla == "D2") {
        tab.ApplyBoxChange(2);
    }
    else if (tecla == "D3") {
        tab.ApplyBoxChange(3);
    }
    else if (tecla == "Q" || tecla == "ESCAPE") {
        c = 'q';
    }
    std::cin.ignore();
    std::cin.clear();
    return c;
}

void Menu::LeeInput(GameMode& gM, int& sel, const std::vector<std::string>& args) {
    std::string tecla;
    std::cin >> tecla;
    std::transform(tecla.begin(), tecla.end(), tecla.begin(), ::toupper);
    if (tecla == "W" || tecla == "UPARROW") {
        if (sel > 0) {
            sel--;
        }
        else {
            sel = args.size() - 1;
        }
    }
    else if (tecla == "S" || tecla == "DOWNARROW") {
        sel++;
    }
    else if (tecla == "SPACEBAR") {
        AcceptMenu(args[sel], gM);
    }
    else if (tecla == "ESCAPE") {
        gM = GameMode::QUIT;
    }

    sel = std::abs(sel % args.size());

    std::cin.ignore();
    std::cin.clear();
}

void Menu::LeeInputToLevel(std::string& s, int& sel, const std::vector<std::string>& args) {
    std::string tecla;
    std::cin >> tecla;
    std::transform(tecla.begin(), tecla.end(), tecla.begin(), ::toupper);
    if (tecla == "W" || tecla == "UPARROW") {
        if (sel > 0) {
            sel--;
        }
        else {
            sel = args.size() - 1;
        }
    }
    else if (tecla == "S" || tecla == "DOWNARROW") {
        sel++;
    }
    else if (tecla == "SPACEBAR") {
        s = args[sel];
    }
    else if (tecla == "ESCAPE") {
        s = "BACK";
    }

    sel = std::abs(sel % args.size());

    std::cin.ignore();
    std::cin.clear();
}

void Menu::ShowMenu(const std::string& title, const std::vector<std::string>& args, int sel) {
    std::cout << title << '\n';
    for (int i = 0; i < args.size(); i++) {
        if (i == sel) {
            std::cout << "> ";
        }
        std::cout << args[i] << '\n';
    }
}

void Menu::AcceptMenu(const std::string& s, GameMode& gM) {
    gM = GameMode::QUIT;
    if (s == menuActions[0]) {
        gM = GameMode::SINGLE_PLAYER;
    }
    else if (s == menuActions[1]) {
        gM = GameMode::TWO_PLAYER;
    }
    else if (s == menuActions[2]) {
        gM = GameMode::CREATE_LEVEL;
    }
    else if (s == menuActions[3]) {
        gM = GameMode::QUIT;
    }
}

void Menu::ShowLevelSelected(Tablero& tab, const std::string& s, int offsetX, int offsetY) {
    try {
        tab.GenerateTablero(s);
        tab.Render(offsetX, offsetY, true);
    }
    catch (...) {
        throw std::runtime_error("No se ha podido leer correctamente el tablero");
    }
}

void Menu::GenerateFile(const std::string& s, Tablero& tab) {
    std::ofstream f(s + ".txt");
    if (!f) {
        throw std::runtime_error("No se ha podido generar el archivo");
    }
    f << tab.GetWidth() << ' ' << tab.GetHeight() << '\n';
    for (int i = 0; i < tab.GetHeight(); i++) {
        for (int j = 0; j < tab.GetWidth(); j++) {
            f << tab.GetBoxState(i, j);
        }
        f << '\n';
    }
    f.close();
}
