#include <chrono>
#include <iostream>
#include <fstream>
#include <SFML/Graphics.hpp>
#include <vector>
#include <ctime>
#include <cstdlib>
#include <thread>

#include "tile.h"

using namespace std;

//Objectives:
//Implement timer
//Make leader board window


void setText(sf::Text &text, float x, float y){
    sf::FloatRect textRect = text.getLocalBounds();
    text.setOrigin(textRect.left + textRect.width/2.0f,
    textRect.top + textRect.height/2.0f);
    text.setPosition(sf::Vector2f(x, y));
}

int checkBombAmount(sf::Vector2f spritePos , vector<vector<int>>& bombTiles) {
    int bombsNear = 0;
    for (vector<int> bombPos : bombTiles) {
        if (spritePos.x < bombPos[0] + 32 && spritePos.x > bombPos[0] - 32 ) {
            if (spritePos.y < bombPos[1] + 32 && spritePos.y > bombPos[1] - 32) {
                bombsNear++;
            }
        }
        return bombsNear;
    }
}

bool isBomb(sf::Vector2f& spritePos , vector<vector<int>>& bombTiles) {
        bool bombActivated = false;
        for (vector<int> bombPos : bombTiles) {
            if (spritePos.x == bombPos[0] && spritePos.y == bombPos[1]) {
                bombActivated = true;
                return bombActivated;
            }
        }
        return bombActivated;
}

void tileClicked(Tile& tileI, bool& bombActivated , sf::Sprite& tileRevealed, sf::Sprite& number, vector<sf::Texture>& numbers, vector<Tile>& tiles,vector<int>& bombPos) {
    sf::Sprite sprite = tileI.getSprite();
    sf::Vector2f spritePos = sprite.getPosition();

    if (!tileI.isRevealed() && !tileI.isflagged()) {
        bombActivated = tileI.reveal();
        if (!bombActivated) {
            tileRevealed.setPosition(spritePos);
            tileI.setSprite(tileRevealed);
            int bombsNear = tileI.getNumberBombNear();
            if (bombsNear != 0) {
                if (bombsNear == 1) {
                    number.setTexture(numbers[0]);
                }else if (bombsNear == 2) {
                    number.setTexture(numbers[1]);
                }else if (bombsNear == 3) {
                    number.setTexture(numbers[2]);
                }else if (bombsNear == 4) {
                    number.setTexture(numbers[3]);
                }else if (bombsNear == 5) {
                    number.setTexture(numbers[4]);
                }else if (bombsNear == 6) {
                    number.setTexture(numbers[5]);
                }else if (bombsNear == 7) {
                    number.setTexture(numbers[6]);
                }else if (bombsNear == 8) {
                    number.setTexture(numbers[7]);
                }
                number.setPosition(spritePos);
                tileI.setNumber(number);
            }else{
                for (Tile* tileI2 : tileI.getNeighbours()) {
                    tileClicked(*tileI2, bombActivated , tileRevealed, number, numbers, tiles, bombPos);
                }
            }
        }else {
            bombPos.push_back(tileI.getSprite().getPosition().x);
            bombPos.push_back(tileI.getSprite().getPosition().y);
        }
    }
}

void updateDigitCounter(int& bombFlagged, sf::Texture& digit, sf::Sprite& digitS, vector<int>& gameWindowInfo, vector<sf::Sprite>& bombCounter) {
    bombCounter.clear();
    string initialBombCounterS = "0" + to_string(gameWindowInfo[2]);
    int digitCount = initialBombCounterS.size();
    string bombCounterS = to_string(bombFlagged);
    int digitI = 0;
    int count = 0;
    int shift = 0;
    digitI = 10;
    for (char num : bombCounterS) {
        if (num == '-') {
            sf::Rect<int> Rect(210, 0,32, 32);
            digitS.setTexture(digit);
            digitS.setTextureRect(Rect);
            digitS.setPosition(12, 32 * (gameWindowInfo[1] + 0.5) + 16);
            shift = 1;
        }else {
            while (count < initialBombCounterS.size() - bombCounterS.size() + shift) {
                sf::IntRect Rect(0, 0,21, 32);
                digitS.setTexture(digit);
                digitS.setTextureRect(Rect);
                digitS.setPosition(33 + count * 21, 32 * (gameWindowInfo[1] + 0.5) + 16);
                count ++;
                bombCounter.push_back(digitS);
            }

            digitI = num - '0';
            sf::IntRect Rect(digitI * 21, 0,21, 32);
            digitS.setTexture(digit);
            digitS.setTextureRect(Rect);
            digitS.setPosition(33 + count * 21, 32 * (gameWindowInfo[1] + 0.5) + 16);
            count++;
        }
        bombCounter.push_back(digitS);
    }
}

void updateTimeMinutesCounter(int& timeCount, sf::Texture& digit, sf::Sprite& digitS, vector<int>& gameWindowInfo, vector<sf::Sprite>& timeCounter, bool minutes) {
    timeCounter.clear();
    string timeCounterS = to_string(timeCount);
    int digitI = 0;
    int count = 0;
    int posxI;
    int posy = 32 * (gameWindowInfo[1] + 0.5) + 16;;

    auto posx = [&]() {
        if (minutes) {
            posxI = count* 21  + gameWindowInfo[0] * 32 - 97;
        }else {
            posxI = count * 21 +  gameWindowInfo[0] * 32 - 54;
        }
        return posxI;
    };


    for (char num : timeCounterS) {
        while (count < 2 - timeCounterS.size()) {
            sf::IntRect Rect(0, 0,21, 32);
            digitS.setTexture(digit);
            digitS.setTextureRect(Rect);
            digitS.setPosition(posx(), posy);
            count++;
            timeCounter.push_back(digitS);
        }
        digitI = num - '0';
        sf::IntRect Rect(digitI * 21, 0,21, 32);
        digitS.setTexture(digit);
        digitS.setTextureRect(Rect);
        digitS.setPosition(posx(), posy);
        count++;

        timeCounter.push_back(digitS);
    }
}

void createFunc(vector<Tile>& tiles , sf::Sprite& tile, vector<vector<int>>& bombTiles,
    int& bombFlagged, sf::Texture& digit, sf::Sprite& digitS, vector<int>& gameWindowInfo, vector<sf::Sprite>& bombCounter, bool& pauseGame,
    bool& debugActivate, bool& bombActivate) {

    pauseGame = false;
    debugActivate = false;
    bombActivate = false;

    bombFlagged = gameWindowInfo[2];

    tiles.clear();
    bombTiles.clear();
    bombCounter.clear();


    bool unique = false;
    for (int i = 0; i < gameWindowInfo[2]; i++) {
            int random_spot_x;
            int random_spot_y;
            vector<int> bombTile;

            while (!unique) {
                unique = true;
                random_spot_x = rand() % (gameWindowInfo[0] + 1) * 32;
                random_spot_y = rand() % (gameWindowInfo[1]) * 32;
                bombTile = {random_spot_x, random_spot_y};
                for (int j = 0; j< bombTiles.size(); j++) {
                    if (bombTile == bombTiles[j]) {
                        unique = false;
                    }
                }
            }
            unique = false;

            bombTiles.push_back(bombTile);

        }

        int numRows = gameWindowInfo[1];
        int numCols = gameWindowInfo[0];

        for (int i = 0; i < gameWindowInfo[1]; i++) {
            for (int j = 0; j <  gameWindowInfo[0]; j++) {
                sf::Vector2f pos(j * 32, i * 32);
                tile.setPosition(pos);
                bool bomb = isBomb(pos, bombTiles);
                tiles.emplace_back(tile, bomb);
            }
        }

        std::vector<int> addNumberX = {-32, 0, 32};
        std::vector<int> addNumberY = {-32, 0, 32};

        for (int i = 0; i < numCols; i++) {
            for (int j = 0; j < numRows; j++) {
                int currentIndex = j * numCols + i;
                sf::Vector2f pos(i * 32, j * 32);

                for (int addX : addNumberX) {
                    for (int addY : addNumberY) {
                        if (addX == 0 && addY == 0) continue; // skip self

                        float neighborX = pos.x + addX;
                        float neighborY = pos.y + addY;

                        // Check bounds
                        if (neighborX >= 0 && neighborX < numCols * 32 &&
                            neighborY >= 0 && neighborY < numRows * 32) {

                            int ni = neighborX / 32;
                            int nj = neighborY / 32;
                            int neighborIndex = nj * numCols + ni;

                            tiles[currentIndex].addNeighbour(&tiles[neighborIndex]);
                            }
                    }
                }
            }
        }

        for (Tile& tile : tiles) {
            tile.createNumberBombNear();
        }

        updateDigitCounter( bombFlagged, digit,  digitS, gameWindowInfo,  bombCounter);
}

void openLeaderBoard(int width, int height, sf::Font &font, int min = 0, int sec = 0, string nameE = "", bool newRec = false, bool show = true) {
    ifstream file("../files/leaderboard.txt");

    int position = 1;
    bool changed = false;
    string line;
    vector<string> leaderBoardStrings;
    vector<string> leaderBoardWrite;
    while (getline(file , line)) {
        if (position <= 5) {
            string name;
            string time = line.substr(0, line.find(','));
            if (newRec) {
                int minO = stoi(time.substr(0, time.find(':')));
                int secO = stoi(time.substr( time.find(':') + 1));
                if (min < minO || (min == minO && sec < secO)) {
                    string newMin =  to_string(min);
                    if (newMin.size() == 1) {
                        newMin = "0" + newMin;
                    }
                    string newSec =  to_string(sec);
                    if (newSec.size() == 1) {
                        newSec = "0" + newSec;
                    }
                    time = newMin + ":" + newSec;
                    name = nameE + "*";
                    newRec = false;
                    changed = true;
                    leaderBoardWrite.push_back(time + "," + nameE);
                }else {
                    name = line.substr(line.find(',') + 1);
                }

            }else {
                name = line.substr(line.find(',') + 1);
            }

            leaderBoardStrings.push_back(to_string(position) + "\t" + time + "\t" + name + "\n\n");
            if (changed) {
                changed = false;
            }else {
                leaderBoardWrite.push_back(time + "," + name);
            }
            position++;

        }
    }

    file.close();
    ofstream outFile("../files/leaderboard.txt");
    for (string line : leaderBoardWrite) {
        outFile << line << endl;
    }

    outFile.close();
    string leaderBoardStringsCombined;
    for (int i = 0; i < leaderBoardStrings.size(); i++) {
        leaderBoardStringsCombined += leaderBoardStrings[i];
    }

    if (show) {
        sf::RenderWindow window3(sf::VideoMode(width + 50, height - 100), "Leaderboard Screen");

        sf::Text welcomeText;
        welcomeText.setCharacterSize(20);
        welcomeText.setString("LEADERBOARD");
        welcomeText.setPosition(sf::Vector2f(width / 2, height / 2 - 120));
        welcomeText.setColor(sf::Color::White);
        welcomeText.setFont(font);
        setText(welcomeText, width / 2, height / 2 - 120);
        welcomeText.setStyle(sf::Text::Bold);
        welcomeText.setStyle(sf::Text::Underlined);

        sf::Text mainText;
        mainText.setCharacterSize(18);
        mainText.setString(leaderBoardStringsCombined);
        mainText.setPosition(sf::Vector2f(width / 2, height / 2 +20));
        mainText.setColor(sf::Color::White);
        mainText.setFont(font);
        setText(mainText, width / 2, height / 2 + 20);
        mainText.setStyle(sf::Text::Bold);

        while (window3.isOpen()) {
            sf::Event event;
            while(window3.pollEvent(event)) {
                if(event.type == sf::Event::Closed) {
                    window3.close();
                }

                window3.clear(sf::Color::Blue);
                window3.draw(welcomeText);
                window3.draw(mainText);
                window3.display();
            }
        }
    }
    }


int main() {
    unsigned seed = time(0);
    srand(seed);
    vector<int> gameWindowInfo;

    ifstream configFile("../files/config.cfg");
    if (configFile.is_open()) {
        string line;
        while (getline (configFile, line)) {
            if (!line.empty()) {
                gameWindowInfo.push_back(stoi(line));
            }
        }
    }


    float width = gameWindowInfo[0] * 32;
    float height = gameWindowInfo[1] * 32 + 100;


    //Welcome Window
    sf::RenderWindow window(sf::VideoMode(width, height), "Welcome Screen");
    sf::Font font;
    font.loadFromFile("../files/font.ttf");

    sf::Text welcomeText;
    welcomeText.setCharacterSize(24);
    welcomeText.setString("WELCOME TO MINESWEEPER");
    welcomeText.setPosition(sf::Vector2f(width / 2, height / 2 - 150));
    welcomeText.setColor(sf::Color::White);
    welcomeText.setFont(font);
    setText(welcomeText, width / 2, height / 2 - 150);
    welcomeText.setStyle(sf::Text::Bold);
    welcomeText.setStyle(sf::Text::Underlined);

    sf::Text enter_name_text("Enter your name:", font, 20);
    enter_name_text.setColor(sf::Color::White);
    enter_name_text.setPosition(sf::Vector2f(width / 2, height / 2 - 75));
    setText(enter_name_text, width / 2, height / 2 - 75);

    string name_string = "|";
    sf::Text name(name_string, font, 18);
    name.setColor(sf::Color::White);
    name.setPosition(sf::Vector2f(width / 2, height / 2 - 45));
    setText(name, width / 2, height / 2 - 45);

    char cursor = '|';
    bool enteredGame = false;
    while(window.isOpen()) {
        sf::Event event;
        while(window.pollEvent(event)) {
            if(event.type == sf::Event::Closed) {
                window.close();
            }else if(event.type == sf::Event::KeyPressed) {
                string letterString = sf::Keyboard::getDescription(event.key.scancode);
                if (letterString.size() == 1 and name_string.size() < 11) {
                    char letter = letterString[0];
                    if (isalpha(letter)) {
                        if (name_string.size() != 1) {
                            letter = char(tolower(letter));
                        }
                        name_string += letter;
                        name.setString(name_string);
                        setText(name, width / 2, height / 2 - 45);
                    }
                }else if (letterString == "Enter") {
                    if (name_string.size() > 1) {
                        window.close();
                        enteredGame = true;
                    }
                }else if (letterString == "Backspace") {
                    if (name_string.size() > 1) {
                        name_string.pop_back();
                    }
                    name.setString(name_string);
                    setText(name, width / 2, height / 2 - 45);

                }
            }

            window.clear(sf::Color::Blue);
            window.draw(welcomeText);
            window.draw(enter_name_text);
            window.draw(name);
            window.display();
        }
    }


    //Game Window
    if (enteredGame) {
        name_string = name_string.substr(1);
        bool pauseGame = false;
        bool debugActivate = false;
        bool bombActivated = false;
        bool deleteFlag = false;
        bool justUnpause = false;
        bool gameWin = false;
        bool leaderBoardB = false;
        bool updatedLeaderBoard = false;

        int bombFlagged = gameWindowInfo[2];
        int countI = 0;
        int grid = gameWindowInfo[0] * gameWindowInfo[1];

        vector<int> bombPosAct;
        vector<vector<int>> bombTiles;
        vector<Tile> tiles;
        vector<sf::Sprite> tilesRevealed;
        vector<sf::Sprite> flags;
        vector<sf::Sprite> minesNear;
        vector<sf::Texture>numbers;
        vector<sf::Sprite> bombCounter;
        vector<sf::Sprite> timerMinutes;
        vector<sf::Sprite> timerSeconds;


        sf::RenderWindow window2(sf::VideoMode(width, height), "Game Screen");



        sf:: Texture tile_hidden;
        tile_hidden.loadFromFile("../files/images/tile_hidden.png");
        sf::Texture happyFace;
        happyFace.loadFromFile("../files/images/face_happy.png");
        sf::Texture debugTexture;
        debugTexture.loadFromFile("../files/images/debug.png");
        sf::Texture pauseTexture;
        pauseTexture.loadFromFile("../files/images/pause.png");
        sf::Texture play;
        play.loadFromFile("../files/images/play.png");
        sf::Texture leaderBoard;
        leaderBoard.loadFromFile("../files/images/leaderboard.png");
        sf::Texture mine;
        mine.loadFromFile("../files/images/mine.png");
        sf::Texture tile_revealed;
        tile_revealed.loadFromFile("../files/images/tile_revealed.png");
        sf::Texture face_lose;
        face_lose.loadFromFile("../files/images/face_lose.png");
        sf::Texture face_win;
        face_win.loadFromFile("../files/images/face_win.png");
        sf::Texture flagT;
        flagT.loadFromFile("../files/images/flag.png");
        sf::Texture number1T;
        number1T.loadFromFile("../files/images/number_1.png");
        sf::Texture number2T;
        number2T.loadFromFile("../files/images/number_2.png");
        sf::Texture number3T;
        number3T.loadFromFile("../files/images/number_3.png");
        sf::Texture number4T;
        number4T.loadFromFile("../files/images/number_4.png");
        sf::Texture number5T;
        number5T.loadFromFile("../files/images/number_5.png");
        sf::Texture number6T;
        number6T.loadFromFile("../files/images/number_6.png");
        sf::Texture number7T;
        number7T.loadFromFile("../files/images/number_7.png");
        sf::Texture number8T;
        number8T.loadFromFile("../files/images/number_8.png");

        sf::Texture digit;
        digit.loadFromFile("../files/images/digits.png");



        numbers.push_back(number1T);
        numbers.push_back(number2T);
        numbers.push_back(number3T);
        numbers.push_back(number4T);
        numbers.push_back(number5T);
        numbers.push_back(number6T);
        numbers.push_back(number7T);
        numbers.push_back(number8T);

        sf::Sprite tile(tile_hidden);
        sf::Sprite happy_face(happyFace);
        sf::Sprite debug(debugTexture);
        sf::Sprite pauseButton(pauseTexture);
        sf::Sprite playButton(play);
        sf::Sprite leaderboard(leaderBoard);
        sf::Sprite mineButton(mine);
        sf::Sprite tileRevealed(tile_revealed);
        sf::Sprite faceLose(face_lose);
        sf::Sprite faceWin(face_win);
        sf::Sprite flag(flagT);
        sf::Sprite number;
        sf::Sprite digitS;
        sf::Sprite negative;

        happy_face.setPosition((gameWindowInfo[0] * 16) - 32,  32 *(gameWindowInfo[1] + 0.5));
        debug.setPosition((gameWindowInfo[0] * 32) - 304, 32 * (gameWindowInfo[1] + 0.5));
        pauseButton.setPosition((gameWindowInfo[0] * 32) - 240, 32 * (gameWindowInfo[1] + 0.5));
        playButton.setPosition((gameWindowInfo[0] * 32) - 240, 32 * (gameWindowInfo[1] + 0.5));
        faceLose.setPosition((gameWindowInfo[0] * 16) - 32,  32 *(gameWindowInfo[1] + 0.5));
        leaderboard.setPosition(gameWindowInfo[0] * 32 - 176, 32 * (gameWindowInfo[1] + 0.5));
        faceWin.setPosition((gameWindowInfo[0] * 16) - 32,  32 *(gameWindowInfo[1] + 0.5));


        createFunc( tiles , tile,  bombTiles,
        bombFlagged, digit, digitS, gameWindowInfo, bombCounter, pauseGame,
         debugActivate, bombActivated);

        int min = 0;
        int seconds = 0;
        sf::Vector2i mouse;
        auto start = chrono::high_resolution_clock::now();
        auto end = chrono::high_resolution_clock::now();
        chrono::duration<float> duration = end - start;

        updateTimeMinutesCounter(min, digit,  digitS, gameWindowInfo,timerMinutes,true);
        updateTimeMinutesCounter(seconds, digit,  digitS, gameWindowInfo,timerSeconds,false);

        float adder = 0;

        while(window2.isOpen()) {
            sf::Event event;
            while(window2.pollEvent(event)) {
                if (!pauseGame && !gameWin) {
                    if (justUnpause) {
                        justUnpause = false;
                        start = chrono::high_resolution_clock::now();
                    }
                    end = chrono::high_resolution_clock::now();
                    duration = end - start;

                    if (duration.count() >= 1) {
                        seconds++;
                        if (seconds >= 60) {
                            min++;
                            seconds -= 60;
                            updateTimeMinutesCounter(min, digit,  digitS, gameWindowInfo,timerMinutes,true);
                        }
                        updateTimeMinutesCounter(seconds, digit,  digitS, gameWindowInfo,timerSeconds,false);
                        start = end;
                    }
                }
                if(event.type == sf::Event::Closed) {
                    window2.close();
                }else if (event.type == sf::Event::MouseButtonPressed) {
                    if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
                        mouse = sf::Mouse::getPosition(window2);
                        if (mouse.x >= (gameWindowInfo[0] * 16) - 32 && mouse.x < (gameWindowInfo[0] * 16) - 32 + 64) {
                            if (mouse.y >32 * (gameWindowInfo[1] + 0.5)  && mouse.y < 32 * (gameWindowInfo[1] + 0.5) + 64) {
                                createFunc( tiles , tile,  bombTiles,
                                bombFlagged, digit, digitS, gameWindowInfo, bombCounter, pauseGame,
                                 debugActivate, bombActivated);
                            }
                        }

                        if (mouse.x >= gameWindowInfo[0] * 32 - 176  && mouse.x < gameWindowInfo[0] * 32 - 176 + 64) {
                            if (mouse.y >32 * (gameWindowInfo[1] + 0.5)  && mouse.y < 32 * (gameWindowInfo[1] + 0.5) + 64) {
                                if (!gameWin) {
                                    openLeaderBoard(width, height, font);
                                }else {
                                    openLeaderBoard(height, width, font);
                                }

                            }
                        }

                        if (!bombActivated && !gameWin) {
                            if (!pauseGame) {
                                if (mouse.x >= (gameWindowInfo[0] * 32) - 304  && mouse.x <= (gameWindowInfo[0] * 32) - 304 + 64) {
                                    if (mouse.y > 32 * (gameWindowInfo[1] + 0.5)  && mouse.y < 32 * (gameWindowInfo[1] + 0.5) + 64) {
                                        if(debugActivate) {
                                            debugActivate = false;
                                        }else {
                                            debugActivate = true;
                                        }
                                    }
                                }




                                for (int i =0 ; i < tiles.size(); i++) {
                                    Tile& tileI = tiles[i];
                                    sf::Sprite sprite = tileI.getSprite();
                                    sf::Vector2f spritePos = sprite.getPosition();

                                    if(mouse.y >= spritePos.y && mouse.y < spritePos.y + 32 && mouse.x >= spritePos.x && mouse.x < spritePos.x + 32 ) {
                                        tileClicked(tileI, bombActivated , tileRevealed, number, numbers, tiles, bombPosAct);
                                    }
                                }
                            }




                            if (mouse.x >= (gameWindowInfo[0] * 32) - 240  && mouse.x < (gameWindowInfo[0] * 32) - 240 + 64) {
                                if (mouse.y >32 * (gameWindowInfo[1] + 0.5)  && mouse.y < 32 * (gameWindowInfo[1] + 0.5) + 64) {
                                    if (pauseGame) {
                                        pauseGame = false;
                                        justUnpause= true;
                                    }else {
                                        pauseGame = true;
                                    }
                                }
                            }


                        }




                    }else if (sf::Mouse::isButtonPressed(sf::Mouse::Right)) {
                        mouse = sf::Mouse::getPosition(window2);
                        vector<sf::Sprite> flagsTemp;
                        for (Tile& tileI : tiles) {
                            sf::Sprite sprite = tileI.getSprite();
                            sf::Vector2f spritePos = sprite.getPosition();

                            if(mouse.y > spritePos.y && mouse.y < spritePos.y + 32 && mouse.x > spritePos.x && mouse.x < spritePos.x + 32 ) {
                                for (sf::Sprite flagI : flags) {
                                    sf::Vector2f flagPos = flagI.getPosition();
                                    if (spritePos.x == flagPos.x && spritePos.y == flagPos.y) {
                                        deleteFlag = true;
                                    }else {
                                        flagsTemp.push_back(flagI);
                                    }
                                }
                                if (!deleteFlag) {
                                    tileI.flag();
                                    flag.setPosition(spritePos);
                                    flags.push_back(flag);
                                    bombFlagged--;
                                }else {
                                    tileI.unflag();
                                    bombFlagged++;
                                    flags.clear();
                                    flags = flagsTemp;
                                    deleteFlag = false;
                                }
                                updateDigitCounter( bombFlagged, digit,  digitS, gameWindowInfo,  bombCounter);
                            }

                        }


                    }
                }
            }

            window2.clear(sf::Color::White);
            int i = 0;



            int j = 0;
            for (Tile tile : tiles) {
                if (pauseGame) {
                    tileRevealed.setPosition(tile.getSprite().getPosition());
                    window2.draw(tileRevealed);
                }else {
                    window2.draw(tile.getSprite());
                    if (tile.isRevealed()) {
                        countI++;
                        if (tile.getNumberBombNear() > 0) {
                            window2.draw(tile.getNumber());
                        }
                    }
                }

            }

            if (countI >= tiles.size() - bombTiles.size()) {
                bool flagPosRight = false;
                vector<sf::Sprite> flagsTemp;
                bombFlagged = 0;
                updateDigitCounter( bombFlagged, digit,  digitS, gameWindowInfo,  bombCounter);
                for (vector<int> bombTile : bombTiles) {
                    flag.setPosition(bombTile[0], bombTile[1]);
                    flagsTemp.push_back(flag);

                }
                flags.clear();
                flags = flagsTemp;
                gameWin = true;

            }
            countI = 0;

            for (sf::Sprite sprite : minesNear) {
                window2.draw(sprite);
            }

            for (sf::Sprite sprite : bombCounter) {
                window2.draw(sprite);
            }

            for (sf::Sprite sprite : flags) {
                window2.draw(sprite);
            }

            for (sf::Sprite sprite : timerMinutes) {
                window2.draw(sprite);
            }

            for (sf::Sprite sprite : timerSeconds) {
                window2.draw(sprite);
            }

            if ((debugActivate || bombActivated) && !pauseGame) {
                for (vector<int> bombPos : bombTiles) {
                    mineButton.setPosition(bombPos[0], bombPos[1]);
                    window2.draw(mineButton);
                    i++;
                }
            }


            if (!bombActivated) {
                if (gameWin) {
                    window2.draw(faceWin);
                    if (!updatedLeaderBoard) {

                        openLeaderBoard(height, width, font, min, seconds, name_string, true, false);
                        updatedLeaderBoard = true;
                    }

                }else {
                    window2.draw(happy_face);
                }

            }else {
                for (vector<int> bombTile : bombTiles) {
                    if (bombPosAct[0] == bombTile[0] && bombPosAct[1] == bombTile[1]) {
                        mineButton.setPosition(bombPosAct[0], bombPosAct[1]);
                        window2.draw(mineButton);
                    }
                }
                window2.draw(faceLose);
            }

            if (!pauseGame) {
                window2.draw(pauseButton);
            }else {
                window2.draw(playButton);
            }

            window2.draw(debug);
            window2.draw(leaderboard);
            window2.display();
        }
    }

    return 0;
}