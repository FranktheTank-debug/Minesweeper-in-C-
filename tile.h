class Tile {
private:
    bool revealed = false;
    sf::Sprite tile;
    bool bombTile;
    bool flagged = false;
    sf::Sprite bombNumber;
    std::vector<Tile*> neighbors;
    int bombNear = 0;

public:
    Tile(sf::Sprite sprite, bool bombOn) {
        tile = sprite;
        bombTile = bombOn;
    }

    bool reveal() {
        revealed = true;
        return bombTile;
    }

    void flag() {
        flagged = true;
    }

    void unflag() {
        flagged = false;
    }

    bool isflagged() {
        return flagged;
    }

    void unreveal() {
        revealed = false;
    }

    bool isRevealed() const {
        return revealed;
    }

    void setSprite(sf::Sprite sprite) {
        tile = sprite;
    }

    sf::Sprite getSprite() const {
        return tile;
    }

    void setNumber(sf::Sprite sprite) {
        bombNumber = sprite;
    }

    bool isBomb() {
        return bombTile;
    }
    sf::Sprite getNumber() const {
        return bombNumber;
    }

    bool getBombTile() const {
        return bombTile;
    }

    void addNeighbour(Tile* tile) {
        neighbors.push_back(tile);
    }

    void createNumberBombNear() {
        bombNear = 0;
        for (Tile* tile : neighbors) {
            if (tile->getBombTile()) {
                bombNear++;
            }
        }
    }

    int getNumberBombNear() const {
        return bombNear;
    }

    const std::vector<Tile*>& getNeighbours() const {
        return neighbors;
    }
};
