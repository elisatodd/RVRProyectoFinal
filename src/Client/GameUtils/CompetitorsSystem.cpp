#include "CompetitorsSystem.h"
#include "Tablero.h"
#include "EnumDefs.h"

#include <thread>
#include <chrono>

CompetitorsSystem::CompetitorsSystem(GameMode gM, int maxScores, Tablero* tab) {
    maxScore_ = maxScores;
    players.resize(2);
    players[0] = new Competitor();
    players[0]->player = new Player(tab->getPlayerOneInitialPosition());
    std::cout << "depurationSeves\n";
    players[0]->player->setControls("W", "S", "D", "A");
    players[0]->points = 0;

    // switch (gM) {
    //     case TWO_PLAYER:
    //         players[1]->player =new Player(tab->getPlayerTwoInitialPosition());
    //         break;
    //     case SINGLE_PLAYER:
    //     default:
    //         players[1]->player = new Player(tab->getPlayerTwoInitialPosition());
    //         break;
    // }
    players[1] = new Competitor();
    players[1]->player = new Player(tab->getPlayerTwoInitialPosition());
    players[1]->player->setControls("W", "S", "D", "A");
    players[1]->points = 0;

//    PlayerColor = { ConsoleColor::DarkMagenta, ConsoleColor::Blue,
//                        ConsoleColor::DarkYellow, ConsoleColor::DarkGreen };
}

CompetitorsSystem::~CompetitorsSystem() {
    for(int i = 0; i < players.size(); ++i){
        delete players[i]->player;
        delete players[i];
    }
}

Winner CompetitorsSystem::CheckCollisions(Tablero*tab, int renderOffsetX, int renderOffsetY) {
    CollisionState colState = NONE_COLLIDED;

    if (players[0]->player->getPlayerHead() == players[1]->player->getPlayerHead())
        colState = BOTH_COLLIDED;

    int i = 0;
    while (i < players.size() && colState != BOTH_COLLIDED) {
        Coor head = players[i]->player->getPlayerHead();
        if (thisCollide(head, tab)) {
            switch (colState) {
                case NONE_COLLIDED:
                    if (i == 0)
                        colState = PLAYER1_COLLIDED;
                    else
                        colState = PLAYER2_COLLIDED;
                    break;
                case PLAYER1_COLLIDED:
                    colState = BOTH_COLLIDED;
                    break;
                case PLAYER2_COLLIDED:
                case BOTH_COLLIDED:
                default:
                    break;
            }
        }
        i++;
    }

    return addPoints(colState, renderOffsetX, renderOffsetY);
}

void CompetitorsSystem::render(int offsetX, int offsetY) {
    // for (int i = 0; i < players.size(); i++) {
    //     players[i].player.render(playerColor[i], offsetX, offsetY);
    // }
    // showPoints(offsetX);
}

void CompetitorsSystem::showPoints(int offsetX) {
    // Console::SetCursorPosition(0, 0);
    // Console::SetBackgroundColor(ConsoleColor::Black);
    // for (int i = 0; i < players.size(); i++) {
    //     Console::SetCursorPosition((offsetX / 2) + (i * offsetX * 2), 0);
    //     Console::Write("Player ");
    //     Console::SetBackgroundColor(getplayerColor(i));
    //     Console::Write("ºº");
    //     Console::SetBackgroundColor(ConsoleColor::Black);
    //     Console::Write(" score: " + std::to_string(players[i].points));
    // }
}

void CompetitorsSystem::Update(Tablero* tab, GameMode& gameMode) {
    std::vector<char> c = getInput2(gameMode);
    for (int i = 0; i < players.size(); i++) {
        //players[i]->player->update(this, tab, c[i], gameMode);
    }
}

bool CompetitorsSystem::thisCollide(Coor c, Tablero* tab) {
    bool b = false;
    int i = 0;

    while (i < players.size() && !b) {
        b = players[i]->player->collisionWithThisBody(c);
        i++;
    }

    return (tab->thereIsWall(c) || b);
}

// ConsoleColor CompetitorsSystem::getplayerColor(int i) {
//     returnPlayerColor[i %PlayerColor.size()];
// }

Winner CompetitorsSystem::addPoints(CollisionState cS, int offsetX, int offsetY) {
    Winner winner = NONE;
    if (cS != NONE_COLLIDED) {
        switch (cS) {
            case PLAYER1_COLLIDED:
                players[1]->points++;
                break;
            case PLAYER2_COLLIDED:
                players[0]->points++;
                break;
            default:
                break;
        }

        // Console::SetCursorPosition(offsetX, offsetY - 1);
        // if (cS == PLAYER1_COLLIDED) {
        //     Console::Write("Player ");
        //     Console::SetBackgroundColor(getplayerColor(1));
        //     Console::Write("ºº");
        //     Console::SetBackgroundColor(ConsoleColor::Black);
        //     Console::WriteLine(" scored!");
        // } else if (cS == PLAYER2_COLLIDED) {
        //     Console::Write("Player ");
        //     Console::SetBackgroundColor(getplayerColor(0));
        //     Console::Write("ºº");
        //     Console::SetBackgroundColor(ConsoleColor::Black);
        //     Console::WriteLine(" scored!");
        // } else {
        //     Console::WriteLine("There was a tie!");
        // }

        std::this_thread::sleep_for(std::chrono::seconds(1));

        RoundOver();

        for (int i = 0; i < players.size(); i++) {
            if (players[i]->points >= maxScore_)
                winner = static_cast<Winner>(i + 1);
        }
    }
    return winner;
}

std::vector<char> CompetitorsSystem::getInput2(GameMode& gameMode) {
    std::vector<char> c(2, ' ');
    InputState iState = NO_PROCESSED;
   Player* m1 = players[0]->player;
   Player* m2 = players[1]->player;

    // if (Console::KeyAvailable()) {
    //     ConsoleKeyInfo keyInfo = Console::ReadKey(true);

    //     switch (keyInfo.KeyChar) {
    //         case 'w':
    //         case 'W':
    //             if (gameMode != SINGLE_PLAYER) {
    //                 if (iState == NO_PROCESSED || iState == PLAYER1_PROCESSED) {
    //                     c[0] = 'w';
    //                     iState = PLAYER1_PROCESSED;
    //                 } else if (iState == PLAYER2_PROCESSED) {
    //                     c[1] = 'w';
    //                     iState = BOTH_PROCESSED;
    //                 }
    //             }
    //             break;
    //         case 's':
    //         case 'S':
    //             if (gameMode != SINGLE_PLAYER) {
    //                 if (iState == NO_PROCESSED || iState == PLAYER1_PROCESSED) {
    //                     c[0] = 's';
    //                     iState = PLAYER1_PROCESSED;
    //                 } else if (iState == PLAYER2_PROCESSED) {
    //                     c[1] = 's';
    //                     iState = BOTH_PROCESSED;
    //                 }
    //             }
    //             break;
    //         case 'd':
    //         case 'D':
    //             if (iState == NO_PROCESSED || iState == PLAYER1_PROCESSED) {
    //                 c[0] = 'd';
    //                 iState = PLAYER1_PROCESSED;
    //             } else if (iState == PLAYER2_PROCESSED) {
    //                 c[1] = 'd';
    //                 iState = BOTH_PROCESSED;
    //             }
    //             break;
    //         case 'a':
    //         case 'A':
    //             if (iState == NO_PROCESSED || iState == PLAYER1_PROCESSED) {
    //                 c[0] = 'a';
    //                 iState = PLAYER1_PROCESSED;
    //             } else if (iState == PLAYER2_PROCESSED) {
    //                 c[1] = 'a';
    //                 iState = BOTH_PROCESSED;
    //             }
    //             break;
    //         case 'q':
    //         case 'Q':
    //             gameMode = QUIT;
    //             break;
    //         case ConsoleKey::UpArrow:
    //             if (gameMode != SINGLE_PLAYER) {
    //                 if (iState == NO_PROCESSED || iState == PLAYER2_PROCESSED) {
    //                     c[1] = 'w';
    //                     iState = PLAYER2_PROCESSED;
    //                 } else if (iState == PLAYER1_PROCESSED) {
    //                     c[0] = 'w';
    //                     iState = BOTH_PROCESSED;
    //                 }
    //             }
    //             break;
    //         case ConsoleKey::DownArrow:
    //             if (gameMode != SINGLE_PLAYER) {
    //                 if (iState == NO_PROCESSED || iState == PLAYER2_PROCESSED) {
    //                     c[1] = 's';
    //                     iState = PLAYER2_PROCESSED;
    //                 } else if (iState == PLAYER1_PROCESSED) {
    //                     c[0] = 's';
    //                     iState = BOTH_PROCESSED;
    //                 }
    //             }
    //             break;
    //         case ConsoleKey::RightArrow:
    //             if (iState == NO_PROCESSED || iState == PLAYER2_PROCESSED) {
    //                 c[1] = 'd';
    //                 iState = PLAYER2_PROCESSED;
    //             } else if (iState == PLAYER1_PROCESSED) {
    //                 c[0] = 'd';
    //                 iState = BOTH_PROCESSED;
    //             }
    //             break;
    //         case ConsoleKey::LeftArrow:
    //             if (iState == NO_PROCESSED || iState == PLAYER2_PROCESSED) {
    //                 c[1] = 'a';
    //                 iState = PLAYER2_PROCESSED;
    //             } else if (iState == PLAYER1_PROCESSED) {
    //                 c[0] = 'a';
    //                 iState = BOTH_PROCESSED;
    //             }
    //             break;
    //         default:
    //             break;
    //     }
    // }

    return c;
}

void CompetitorsSystem::RoundOver() {
    for (int i = 0; i < players.size(); i++) {
        players[i]->player->ResetPosition();
    }
}


