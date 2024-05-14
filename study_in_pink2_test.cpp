#include "study_in_pink2.h"

////////////////////////////////////////////////////////////////////////
/// STUDENT'S ANSWER BEGINS HERE
/// Complete the following functions
/// DO NOT modify any parameters in the functions.
////////////////////////////////////////////////////////////////////////

//3.1 Map's Element
MapElement::MapElement(ElementType in_type) : type(in_type) {}
MapElement::~MapElement() {}
ElementType MapElement::getType() const { return type; }

// Path : public MapElement
Path::Path() : MapElement(PATH) {}
// Wall : public MapElement
Wall::Wall() : MapElement(WALL) {}
// FakeWall : public MapElement
FakeWall::FakeWall(int in_req_exp) : MapElement(FAKE_WALL), req_exp(in_req_exp) {}
int FakeWall::getReqExp() const { return req_exp; }


//3.2 Map
Map::Map(int num_rows, int num_cols, int num_walls, Position * array_walls, int num_fake_walls, Position * array_fake_walls)
        : num_rows(num_rows), num_cols(num_cols) {
    // Allocate memory for the 2D array
    map = new MapElement**[num_rows];
    for (int i = 0; i < num_rows; ++i) {
        map[i] = new MapElement*[num_cols];
        for (int j = 0; j < num_cols; ++j) {
            map[i][j] = new Path(); // Initialize all elements as Path by default
        }
    }
    // Place FakeWalls
    for (int i = 0; i < num_fake_walls; ++i) {
        int r = array_fake_walls[i].getRow();
        int c = array_fake_walls[i].getCol();
        int req_exp = (r * 257 + c * 139 + 89) % 900 + 1; // Calculate required EXP
        delete map[r][c]; // Clean up default Path before placing FakeWall
        map[r][c] = new FakeWall(req_exp);
    }
    // Place Walls
    for (int i = 0; i < num_walls; ++i) {
        int r = array_walls[i].getRow();
        int c = array_walls[i].getCol();
        delete map[r][c]; // Clean up default Path before placing Wall
        map[r][c] = new Wall();
    }
}
Map::~Map() {
    // Deallocate memory for the 2D array
    for (int i = 0; i < num_rows; ++i) {
        for (int j = 0; j < num_cols; ++j) {
            delete map[i][j];
        }
        delete[] map[i];
    }
    delete[] map;
}

bool Map::isValid(const Position & pos, MovingObject * mv_obj) const {
    // Check if the position is within the bounds of the map
    if (pos.getRow() < 0 || pos.getRow() >= num_rows || pos.getCol() < 0 || pos.getCol() >= num_cols) {
        return false;
    }

    // Retrieve the MapElement at the given position
    MapElement* element = map[pos.getRow()][pos.getCol()];
    ElementType type = element->getType();

    // Check the type of MapElement and apply the rules for movement
    switch (type) {
        case PATH:
            // All objects can move on a path
            return true;
        case WALL:
            // No object can move on a wall
            return false;
        case FAKE_WALL: {
            // Sherlock can always detect a fake wall
            if ((mv_obj->getName() == "Sherlock") || (mv_obj->getName() == "Criminal") || (mv_obj->getName() == "Robot")) {
                return true;
            }
            // Watson can move through a fake wall if he has enough EXP
            if (mv_obj->getName() == "Watson") {
                // Cast the MapElement to FakeWall to access its specific methods
                FakeWall* fakeWall = dynamic_cast<FakeWall*>(element);
                if (fakeWall != nullptr) {
                    // Get Watson's EXP and compare with the FakeWall's required EXP
                    int watsonExp = static_cast<Watson*>(mv_obj)->getExp();
                    return watsonExp > fakeWall->getReqExp();
                }
            }
            // Other objects cannot move through a fake wall
            return false;
        }
        default:
            // If the element type is not recognized, disallow movement
            return false;
    }
}

// Helper function to get the MapElement at a specific position
ElementType Map::getElementType(const Position &pos) const{
    return map[pos.getRow()][pos.getCol()]->getType();
}

// 3.3 Position
const Position Position::npos = Position(-1, -1); // Static member initialization
Position::Position(int r, int c) : r(r), c(c) {} // Constructor
Position::Position(const string & str_pos) {
    // Parse the string to extract row and column
    if (str_pos[0] == '(' && str_pos[str_pos.size() - 1] == ')') {
        stringstream ss(str_pos.substr(1, str_pos.size() - 2));
        char comma;
        ss >> r >> comma >> c;
    } else {
        r = c = -1; // Set to invalid position if format is incorrect
    }
}


// Getters and Setters
int Position::getRow() const { return r; }
int Position::getCol() const { return c; }
void Position::setRow(int r) { this->r = r; }
void Position::setCol(int c) { this->c = c; }

// Method to return the string representation of the Position
string Position::str() const {
    stringstream ss;
    ss << "(" << r << "," << c << ")";
    return ss.str();
}

// Method to check if two positions are equal
bool Position::isEqual(int in_r, int in_c) const {
    return r == in_r && c == in_c;
}
bool Position::isEqual(Position in_position) const{
    return (r == in_position.getRow() && c == in_position.getCol());
}

// 3.4 MovingObject
MovingObject::MovingObject(int index, const Position pos, Map * map, const string & name)
        : index(index), pos(pos), map(map), name(name) {}
MovingObject::~MovingObject() {}
Position MovingObject::getCurrentPosition() const { return pos; }


// 3.5 Sherlock
Sherlock::Sherlock(int index, const string & moving_rule, const Position & init_pos, Map * map, int init_hp, int init_exp)
        : Character(index, init_pos, map, "Sherlock"), moving_rule(moving_rule), hp(init_hp > 500 ? 500 : init_hp), exp(init_exp > 900 ? 900 : init_exp), move_index(0) {}
Position Sherlock::getNextPosition() {
    if (move_index >= moving_rule.length()) move_index = 0; // Reset to start if at the end of moving_rule
    char move = moving_rule[move_index++];
    Position current_pos = getCurrentPosition();
    int r = current_pos.getRow();
    int c = current_pos.getCol();

    switch (move) {
        case 'L': c -= 1; break; // Move left
        case 'R': c += 1; break; // Move right
        case 'U': r -= 1; break; // Move up
        case 'D': r += 1; break; // Move down
        default: return Position::npos; // Invalid move
    }

    Position next_pos(r, c);
    if (map->isValid(next_pos, this)) return next_pos;
    else return Position::npos;
}
void Sherlock::move() {
    if (exp <= 0 || hp <= 0) { // Sherlock cannot move if he has no EXP or HP
        return;
    }
    Position next_pos = getNextPosition();
    if (next_pos != Position::npos) { // Fix overload operator != in headers
        pos = next_pos;
    }
}
string Sherlock::str() const {
    stringstream ss;
    ss << "Sherlock[index=" << index << ";pos=" << pos.str() << ";moving_rule=" << moving_rule << "]";
    return ss.str();
}

// 3.6 Watson
Watson::Watson(int index, const string & moving_rule, const Position & init_pos, Map * map, int init_hp, int init_exp)
        : Character(index, init_pos, map, "Watson"), moving_rule(moving_rule), hp(init_hp > 500 ? 500 : init_hp), exp(init_exp > 900 ? 900 : init_exp), move_index(0) {}

Position Watson::getNextPosition() {
    if (move_index >= moving_rule.length()) move_index = 0; // Reset to start if at the end of moving_rule
    char move = moving_rule[move_index++];
    Position current_pos = getCurrentPosition();
    int r = current_pos.getRow();
    int c = current_pos.getCol();

    switch (move) {
        case 'L': c -= 1; break; // Move left
        case 'R': c += 1; break; // Move right
        case 'U': r -= 1; break; // Move up
        case 'D': r += 1; break; // Move down
        default: return Position::npos; // Invalid move
    }

    Position next_pos(r, c);
    if (map->isValid(next_pos, this)) return next_pos;
    else return Position::npos;
}

void Watson::move() {
    if (exp <= 0 || hp <= 0) { // Watson cannot move if he has no EXP or HP
        return;
    }
    Position next_pos = getNextPosition();
    if (next_pos != Position::npos) { // Fix overload operator != in headers
        pos = next_pos;
    }
}

string Watson::str() const {
    stringstream ss;
    ss << "Watson[index=" << index << ";pos=" << pos.str() << ";moving_rule=" << moving_rule << "]";
    return ss.str();
}

// 3.7 Criminal
// Criminal constructor
Criminal::Criminal(int index, const Position& init_pos, Map* map, Sherlock* sherlock, Watson* watson)
        : Character(index, init_pos, map, "Criminal"), sherlock(sherlock), watson(watson) {}

// Helper function to calculate Manhattan distance
int manhattanDistance(const Position& a, const Position& b) {
    return abs(a.getRow() - b.getRow()) + abs(a.getCol() - b.getCol());
}

// Criminal::getNextPosition implementation
Position Criminal::getNextPosition() {
    Position currentPos = getCurrentPosition();
    Position bestPos = Position::npos;
    int maxDistance = -1;

    // Check all adjacent positions (U, D, L, R)
    Position directions[4] = {
            Position(currentPos.getRow() - 1, currentPos.getCol()), // Up
            Position(currentPos.getRow(), currentPos.getCol() - 1), // Left
            Position(currentPos.getRow() + 1, currentPos.getCol()), // Down
            Position(currentPos.getRow(), currentPos.getCol() + 1)  // Right
    };
    for (const auto& pos : directions) {
        if (map->isValid(pos, this)) {
            int distanceToSherlock = manhattanDistance(pos, sherlock->getCurrentPosition());
            int distanceToWatson = manhattanDistance(pos, watson->getCurrentPosition());
            int totalDistance = distanceToSherlock + distanceToWatson;

            if (totalDistance > maxDistance) {
                maxDistance = totalDistance;
                bestPos = pos;
            }
        }
    }

    return bestPos != Position::npos ? bestPos : currentPos; // Stay still if no valid move
}

// Criminal::move implementation
void Criminal::move() {
    last_position = pos;
    Position nextPos = getNextPosition();
    if (nextPos != Position::npos && nextPos != getCurrentPosition()) {
        this->pos = nextPos;
        count_criminal_moves++;
    }
}

// Criminal::str implementation
string Criminal::str() const {
    stringstream ss;
    ss << "Criminal[index=" << index << ";pos=" << pos.str() << "]";
    return ss.str();
}

// 3.8 ArrayOfMovingObjects
ArrayMovingObject::ArrayMovingObject(int capacity) : capacity(capacity), count(0) {
    arr_mv_objs = new MovingObject*[capacity];
    for (int i = 0; i < capacity; ++i) {
        arr_mv_objs[i] = nullptr;
    }
}
// Destructor
/*
ArrayMovingObject::~ArrayMovingObject() {
    for (int i = 0; i < count; ++i) {
        delete arr_mv_objs[i];
    }
    delete[] arr_mv_objs;
}
*/

// Check if the array is full
bool ArrayMovingObject::isFull() const {
    return count == capacity;
}

// Add a new moving object to the array
bool ArrayMovingObject::add(MovingObject* mv_obj) {
    if (isFull()) {
        return false;
    }
    arr_mv_objs[count++] = mv_obj;
    return true;
}

void ArrayMovingObject::remove(int index) {
    if (index < 0 || index >= count) {
        return;
    }
    delete arr_mv_objs[index];
    for (int i = index; i < count - 1; ++i) {
        arr_mv_objs[i] = arr_mv_objs[i + 1];
    }
    arr_mv_objs[count - 1] = nullptr;
    count--;
}

// Get a moving object at a specific index
MovingObject* ArrayMovingObject::get(int index) const {
    if (index < 0 || index >= count) {
        return nullptr;
    }
    return arr_mv_objs[index];
}

// Get the current size of the array
int ArrayMovingObject::size() const {
    return count;
}

// Get a string representation of the array
string ArrayMovingObject::str() const {
    stringstream ss;
    ss << "ArrayMovingObject[count=" << count << ";capacity=" << capacity << ";";
    for (int i = 0; i < count -1 ; ++i) {
        ss << arr_mv_objs[i]->str() << ";";
    }
    ss << arr_mv_objs[count - 1]->str() << "]";
    return ss.str();
}

// 3.9 Configuration
Configuration::Configuration(const string & filepath)
        : arr_walls(nullptr), arr_fake_walls(nullptr), num_walls(0), num_fake_walls(0) {
    ifstream file(filepath);
    string line;
    while (getline(file, line)) {
        stringstream ss(line);
        string key;
        getline(ss, key, '=');
        if (key == "MAP_NUM_ROWS") {
            ss >> map_num_rows;
        } else if (key == "MAP_NUM_COLS") {
            ss >> map_num_cols;
        } else if (key == "MAX_NUM_MOVING_OBJECTS") {
            ss >> max_num_moving_objects;
        } else if (key == "ARRAY_WALLS") {
            string walls;
            getline(ss, walls);
            walls = walls.substr(1, walls.length() - 2); // Remove the surrounding brackets
            stringstream wall_stream(walls);
            string wall;
            while (getline(wall_stream, wall, ';')) {
                num_walls++;
            }
            arr_walls = new Position[num_walls];
            wall_stream.clear();
            wall_stream.str(walls);
            int i = 0;
            while (getline(wall_stream, wall, ';')) {
                arr_walls[i++] = Position(wall);
            }
        } else if (key == "ARRAY_FAKE_WALLS") {
            string fake_walls;
            getline(ss, fake_walls);
            fake_walls = fake_walls.substr(1, fake_walls.length() - 2); // Remove the surrounding brackets
            stringstream fake_wall_stream(fake_walls);
            string fake_wall;
            while (getline(fake_wall_stream, fake_wall, ';')) {
                num_fake_walls++;
            }
            arr_fake_walls = new Position[num_fake_walls];
            fake_wall_stream.clear();
            fake_wall_stream.str(fake_walls);
            int i = 0;
            while (getline(fake_wall_stream, fake_wall, ';')) {
                arr_fake_walls[i++] = Position(fake_wall);
            }
        } else if (key == "SHERLOCK_MOVING_RULE") {
            getline(ss, sherlock_moving_rule);
        } else if (key == "SHERLOCK_INIT_POS") {
            string pos;
            getline(ss, pos);
            sherlock_init_pos = Position(pos);
        } else if (key == "WATSON_MOVING_RULE") {
            getline(ss, watson_moving_rule);
        } else if (key == "WATSON_INIT_POS") {
            string pos;
            getline(ss, pos);
            watson_init_pos = Position(pos);
        } else if (key == "WATSON_INIT_HP") {
            ss >> watson_init_hp;
        } else if (key == "WATSON_INIT_EXP") {
            ss >> watson_init_exp;
        } else if (key == "SHERLOCK_INIT_HP") {
            ss >> sherlock_init_hp;
        } else if (key == "SHERLOCK_INIT_EXP") {
            ss >> sherlock_init_exp;
        } else if (key == "CRIMINAL_INIT_POS") {
            string pos;
            getline(ss, pos);
            criminal_init_pos = Position(pos);
        } else if (key == "NUM_STEPS") {
            ss >> num_steps;
        }
    }
    file.close();
}

Configuration::~Configuration() {
    // Clean up dynamically allocated memory
    delete[] arr_walls;
    delete[] arr_fake_walls;
}
string Configuration::str() const {
    stringstream ss;
    ss << "Configuration[\n"
       << "MAP_NUM_ROWS=" << map_num_rows << "\n"
       << "MAP_NUM_COLS=" << map_num_cols << "\n"
       << "MAX_NUM_MOVING_OBJECTS=" << max_num_moving_objects << "\n"
       << "NUM_WALLS=" << num_walls << "\n"
       << "ARRAY_WALLS=[";
    for (int i = 0; i < num_walls; ++i) {
        if (i > 0) ss << ";";
        ss << arr_walls[i].str();
    }
    ss << "]\n"
       << "NUM_FAKE_WALLS=" << num_fake_walls << "\n"
       << "ARRAY_FAKE_WALLS=[";
    for (int i = 0; i < num_fake_walls; ++i) {
        if (i > 0) ss << ";";
        ss << arr_fake_walls[i].str();
    }
    ss << "]\n"
       << "SHERLOCK_MOVING_RULE=" << sherlock_moving_rule << "\n"
       << "SHERLOCK_INIT_POS=" << sherlock_init_pos.str() << "\n"
       << "SHERLOCK_INIT_HP=" << sherlock_init_hp << "\n"
       << "SHERLOCK_INIT_EXP=" << sherlock_init_exp << "\n"
       << "WATSON_MOVING_RULE=" << watson_moving_rule << "\n"
       << "WATSON_INIT_POS=" << watson_init_pos.str() << "\n"
       << "WATSON_INIT_HP=" << watson_init_hp << "\n"
       << "WATSON_INIT_EXP=" << watson_init_exp << "\n"
       << "CRIMINAL_INIT_POS=" << criminal_init_pos.str() << "\n"
       << "NUM_STEPS=" << num_steps
       << "\n]";
    return ss.str();
}



// 3.10 Robot

// Robot
Robot::Robot(int index, const Position & init_pos, Map * map, RobotType robot_type)
        : MovingObject(index, init_pos, map, "Robot"), robot_type(robot_type), item(nullptr), init_pos(init_pos){
    int i = init_pos.getRow();
    int j = init_pos.getCol();
    int p = i * j;
    int s = 0;
    while (p != 0) {
        s += p % 10;
        p /= 10;
    }
    while (s >= 10) {
        int temp = s;
        s = 0;
        while (temp != 0) {
            s += temp % 10;
            temp /= 10;
        }
    }
    //If s is in the range [0, 1], it will create MagicBook
    //If s is in the segment [2, 3], it will generate EnergyDrink
    //If s is in the segment [4, 5], it will generate FirstAid
    //If s is in the segment [6,7], it will generate ExcemptionCard
    //BaseItem* item = nullptr;
    if (s <= 1) {
        MagicBook* item = new MagicBook();
    } else if (s <= 3) {
        EnergyDrink* item = new EnergyDrink();
    } else if (s <= 5) {
        FirstAid* item = new FirstAid();
    } else if (s <= 7) {
        ExcemptionCard* item = new ExcemptionCard();
        item = new ExcemptionCard();
    } else {
        int t = (i * 11 + j) % 4;
        string challenge;
        switch (t) {
            case 0: challenge = "RobotS"; break;
            case 1: challenge = "RobotC"; break;
            case 2: challenge = "RobotSW"; break;
            case 3: challenge = "all"; break;
        }
        item = new PassingCard(challenge);
    }
    this -> item = item;
}
// Constructor
Robot::~Robot() {
    delete item;
}
void Robot::move() {
    Position next_pos = getNextPosition();
    if (next_pos != Position::npos) {
        pos = next_pos;
    }
}
string Robot::robotTypeToString() const {
    switch (robot_type) {
        case C: return "C";
        case S: return "S";
        case W: return "W";
        case SW: return "SW";
        default: return "Unknown";
    }
}
string Robot::str() const {
    stringstream ss;
    ss << "Robot" << this->robotTypeToString() << "[pos=" << pos.str() << ";type=" << this->robotTypeToString() << ";dist=" << getDistance() << "]";
    return ss.str();
}

void Robot::setItem(BaseItem* item) {
    if (this->item != nullptr) {
        delete this->item;
    }
    this->item = item;
}

// RobotC
RobotC::RobotC(int index, const Position & init_pos, Map * map , Criminal * criminal)
        : Robot(index, init_pos, map, C), criminal(criminal) {}

Position RobotC::getNextPosition() {
    // Criminal's position
    return criminal->getCurrentPosition();
}
int RobotC::getDistance() const {
    return manhattanDistance(this->getCurrentPosition(), criminal->getCurrentPosition());
}
int RobotC::getDistance(MovingObject* obj) const { // Get the Manhattan distance to Sherlock or Watson based on the pointer
    if (dynamic_cast<Sherlock*>(obj)) {
        return manhattanDistance(pos, sherlock->getCurrentPosition());
    } else if (dynamic_cast<Watson*>(obj)) {
        return manhattanDistance(pos, watson->getCurrentPosition());
    } else {
        return -1; // Debug: Return -1 or throw an exception if obj is not Sherlock or Watson
    }
}

string RobotC::str() const {
    return "RobotC[pos=" + pos.str() + ";type=" + this->robotTypeToString() + ";dist=" + "]";
}



// RobotS
// Move to the next location 1 unit away from the original and closest to
// Sherlock’s current location. Note that when we say the location is 1 unit away from
// the original location, the distance referred to is the Manhattan distance. If there are
// multiple nearest locations, the order of selection clockwise rotation, starting from
// the upwards direction, and selecting the first location .

RobotS::RobotS(int index, const Position & init_pos, Map * map, Criminal * criminal, Sherlock * sherlock)
        : Robot(index, init_pos, map, S), criminal(criminal), sherlock(sherlock) {}
Position RobotS::getNextPosition() {
    Position currentPos = getCurrentPosition();
    Position bestPos = Position::npos;
    int minDistance = INT_MAX;

    // Position clockwise order
    Position clockwiseOrder[4] = {
            Position(currentPos.getRow() - 1, currentPos.getCol()), // Up
            Position(currentPos.getRow(), currentPos.getCol() + 1), // Right
            Position(currentPos.getRow() + 1, currentPos.getCol()), // Down
            Position(currentPos.getRow(), currentPos.getCol() - 1)  // Left
    };

    for (const auto& pos : clockwiseOrder) {
        if (map->isValid(pos, this)) {
            int distance = manhattanDistance(pos, sherlock->getCurrentPosition());
            if (distance < minDistance) {
                minDistance = distance;
                bestPos = pos;
            } // If the distance is the same, the first one is selected so no need to check to make sure the order is clockwise
        }
    }

    return bestPos != Position::npos ? bestPos : currentPos; // Stay still if no valid move

}

int RobotS::getDistance() const {
    return manhattanDistance(getCurrentPosition(), sherlock->getCurrentPosition());
}


// RobotW
// RobotW: Moves to the next location 1 unit away from the original and closest to
//  Watson’s next location. If there are multiple suitable positions, the order of selection
//  is as if in RobotS.

// Constructor
RobotW::RobotW(int index, const Position & init_pos, Map * map, Criminal * criminal, Watson * watson)
        : Robot(index, init_pos, map, W), criminal(criminal), watson(watson) {}
// Get the next position
Position RobotW::getNextPosition() {
    Position currentPos = getCurrentPosition();
    Position bestPos = Position::npos;
    int minDistance = INT_MAX;

    // Position clockwise order
    Position clockwiseOrder[4] = {
            Position(currentPos.getRow() - 1, currentPos.getCol()), // Up
            Position(currentPos.getRow(), currentPos.getCol() + 1), // Right
            Position(currentPos.getRow() + 1, currentPos.getCol()), // Down
            Position(currentPos.getRow(), currentPos.getCol() - 1)  // Left
    };

    for (const auto& pos : clockwiseOrder) {
        if (map->isValid(pos, this)) {
            int distance = manhattanDistance(pos, watson->getCurrentPosition());
            if (distance < minDistance) {
                minDistance = distance;
                bestPos = pos;
            } // If the distance is the same, the first one is selected so no need to check to make sure the order is clockwise
        }
    }

    return bestPos != Position::npos ? bestPos : currentPos; // Stay still if no valid move
}

int RobotW::getDistance() const {
    return manhattanDistance(getCurrentPosition(), watson->getNextPosition());
}

// RobotSW: Move to the next location that is 2 units away from the original and has
// the closest total distance to both Sherlock and Watson. If there are multiple suitable
// positions, the order of selection is as if in RobotS.


// Constructor
RobotSW::RobotSW(int index, const Position & init_pos, Map * map, Criminal * criminal, Sherlock * sherlock, Watson * watson)
        : Robot(index, init_pos, map, SW), criminal(criminal), sherlock(sherlock), watson(watson) {}

Position RobotSW::getNextPosition() {
    Position currentPos = getCurrentPosition();
    Position sherlockPos = sherlock->getCurrentPosition();
    Position watsonPos = watson->getCurrentPosition();
    Position bestPos = Position::npos;
    int minTotalDistance = INT_MAX;

    // Direction clockwise order, starting from Up 2, Up-right, Right 2, Down-right, Down 2, Down-left, Left 2, Up-left
    Position clockwiseOrder[8] = {
            Position(currentPos.getRow() - 2, currentPos.getCol()), // Up 2
            Position(currentPos.getRow() - 1, currentPos.getCol() + 1), // Up-right
            Position(currentPos.getRow(), currentPos.getCol() + 2), // Right 2
            Position(currentPos.getRow() + 1, currentPos.getCol() + 1), // Down-right
            Position(currentPos.getRow() + 2, currentPos.getCol()), // Down 2
            Position(currentPos.getRow() + 1, currentPos.getCol() - 1), // Down-left
            Position(currentPos.getRow(), currentPos.getCol() - 2), // Left 2
            Position(currentPos.getRow() - 1, currentPos.getCol() - 1)  // Up-left
    };

    for (const auto& pos : clockwiseOrder) {
        if (map->isValid(pos, this)) {
            int totalDistance = manhattanDistance(pos, sherlockPos) + manhattanDistance(pos, watsonPos);
            if (totalDistance < minTotalDistance) {
                minTotalDistance = totalDistance;
                bestPos = pos;
            } // If the distance is the same, the first one is selected so no need to check to make sure the order is clockwise
        }
    }

    return bestPos != Position::npos ? bestPos : currentPos; // Stay still if no valid move
}

int RobotSW::getDistance() const {
    return manhattanDistance(getCurrentPosition(), sherlock->getCurrentPosition()) + manhattanDistance(getCurrentPosition(), watson->getCurrentPosition());
}


// 3.11 Item

bool MagicBook::canUse(Character* obj, Robot * robot) {
    return dynamic_cast<Watson*>(obj)->getExp() <= 350;
}

void MagicBook::use(Character* obj, Robot * robot) {
    Watson* watson = dynamic_cast<Watson*>(obj);
    int exp = watson->getExp();
    watson->setExp(exp + exp * 0.25);
}

bool EnergyDrink::canUse(Character* obj, Robot * robot) {
    return dynamic_cast<Watson*>(obj)->getHp() <= 100;
}

void EnergyDrink::use(Character* obj, Robot * robot) {
    Watson* watson = dynamic_cast<Watson*>(obj);
    int hp = watson->getHp();
    watson->setHp(hp + hp * 0.2);
}

bool FirstAid::canUse(Character* obj, Robot * robot) {
    Watson* watson = dynamic_cast<Watson*>(obj);
    return watson->getHp() <= 100 || watson->getExp() <= 350;
}

void FirstAid::use(Character* obj, Robot * robot) {
    Watson* watson = dynamic_cast<Watson*>(obj);
    int hp = watson->getHp();
    watson->setHp(hp + hp * 0.5);
}

bool ExcemptionCard::canUse(Character* obj, Robot * robot) {
    return dynamic_cast<Sherlock*>(obj) && dynamic_cast<Sherlock*>(obj)->getHp() % 2 == 1;
}

void ExcemptionCard::use(Character* obj, Robot * robot) {
    // The ExcemptionCard has no effect on the character's parameters
}

bool PassingCard::canUse(Character* obj, Robot * robot) {
    return dynamic_cast<Watson*>(obj) && dynamic_cast<Watson*>(obj)->getHp() % 2 == 0;
}

void PassingCard::use(Character* obj, Robot * robot) {
    string typerobot;
    switch (robot->getRobotType()) {
        case C: {
            typerobot = "RobotC";
            break;
        }
        case S: {
            typerobot = "RobotS";
            break;
        }
        case W: {
            typerobot = "RobotW";
            break;
        }
        case SW: {
            typerobot = "RobotSW";
            break;
        }
        default: {
            typerobot = "Unknown";
            break;
        }
    }
    Watson* watson = dynamic_cast<Watson*>(obj);
    if (typerobot != challenge) {
        int exp = watson->getExp();
        watson->setExp(exp - 50);
    } else if (challenge == "all") { // If the challenge is "all", it passes all the challenges
        return;
    }
}

// 3.12 Bag
// Constructor

BaseBag::BaseBag(Character* obj, int capacity) : obj(obj), capacity(capacity), count(0) {
    items = new BaseItem*[capacity]; // Allocate memory for items array
    for (int i = 0; i < capacity; ++i) {
        items[i] = nullptr; // Initialize all items as nullptr
    }
}

BaseBag::~BaseBag() {
    for (int i = 0; i < count; ++i) {
        delete items[i]; // Delete all items
    }
    delete[] items; // Delete the items array
}

bool BaseBag::insert(BaseItem* item) {
    if (count >= capacity) return false; // Check if the bag is full
    // Insert the item at the beginning of the list (array)
    for (int i = count; i > 0; --i) {
        items[i] = items[i - 1]; // Shift items to the right
    }
    items[0] = item; // Insert the new item at the beginning
    ++count;
    return true;
}

BaseItem* BaseBag::get() {
    if (count == 0) return nullptr; // Check if the bag is empty
    BaseItem* item = items[0]; // Get the first item
    // Remove the first item from the list (array)
    for (int i = 0; i < count - 1; ++i) {
        items[i] = items[i + 1]; // Shift items to the left
    }
    items[count - 1] = nullptr; // Set the last item as nullptr
    --count;
    return item;
}

BaseItem* BaseBag::check() {
    if (count == 0) return nullptr; // Check if the bag is empty
    BaseItem* item = items[0]; // Get the first item
    for (int i = 0; i < count ; ++i) {
        // If the item is a PassingCard or ExcemptionCard ignore it, else return the item
        if (!dynamic_cast<PassingCard*>(item) || !dynamic_cast<ExcemptionCard*>(item)) {
            return item;
        }
    }
}

BaseItem* BaseBag::get(ItemType itemType) {
    for (int i = 0; i < count; ++i) {
        // Use dynamic casting to check the type of the item
        BaseItem* item = nullptr;
        switch (itemType) {
            case MAGIC_BOOK:
                item = dynamic_cast<MagicBook*>(items[i]);
                break;
            case ENERGY_DRINK:
                item = dynamic_cast<EnergyDrink*>(items[i]);
                break;
            case FIRST_AID:
                item = dynamic_cast<FirstAid*>(items[i]);
                break;
            case EXCEMPTION_CARD:
                item = dynamic_cast<ExcemptionCard*>(items[i]);
                break;
            case PASSING_CARD:
                item = dynamic_cast<PassingCard*>(items[i]);
                break;
        }
        if (item != nullptr) {
            // Remove the found item from the list (array)
            for (int j = i; j < count - 1; ++j) {
                items[j] = items[j + 1]; // Shift items to the left
            }
            items[count - 1] = nullptr; // Set the last item as nullptr
            --count;
            return item;
        }
    }
    return nullptr; // Item not found
}
BaseItem* BaseBag::get(BaseItem* item) {
    for (int i = 0; i < count; ++i) {
        if (items[i] == item) {
            // Remove the found item from the list (array)
            for (int j = i; j < count - 1; ++j) {
                items[j] = items[j + 1]; // Shift items to the left
            }
            items[count - 1] = nullptr; // Set the last item as nullptr
            --count;
            return item;
        }
    }
    return nullptr; // Item not found
}
BaseItem* BaseBag::check(ItemType itemType) {
    for (int i = 0; i < count; ++i) {
        // Use dynamic casting to check the type of the item
        BaseItem* item = nullptr;
        switch (itemType) {
            case MAGIC_BOOK:
                item = dynamic_cast<MagicBook*>(items[i]);
                break;
            case ENERGY_DRINK:
                item = dynamic_cast<EnergyDrink*>(items[i]);
                break;
            case FIRST_AID:
                item = dynamic_cast<FirstAid*>(items[i]);
                break;
            case EXCEMPTION_CARD:
                item = dynamic_cast<ExcemptionCard*>(items[i]);
                break;
            case PASSING_CARD:
                item = dynamic_cast<PassingCard*>(items[i]);
                break;
        }
        return item;
    }
    return nullptr; // Item not found
}

string BaseBag::getItemName(BaseItem* item) const {
    if (item == nullptr) {
        return "Item is nullptr";
    }
    if (dynamic_cast<MagicBook*>(item)) return "MagicBook";
    if (dynamic_cast<EnergyDrink*>(item)) return "EnergyDrink";
    if (dynamic_cast<FirstAid*>(item)) return "FirstAid";
    if (dynamic_cast<ExcemptionCard*>(item)) return "ExcemptionCard";
    if (dynamic_cast<PassingCard*>(item)) return "PassingCard";
    return "Unknown item type";
}



string BaseBag::str() const {
    stringstream ss;
    ss << "Bag[count=" << count << ";";
    for (int i = 0; i < count; ++i) {
        if (i> 0 || i < count - 1) ss << ",";
        ss << getItemName(items[i]); // Get the name of the item
    }
    ss << "]";
    return ss.str();
}

SherlockBag::SherlockBag(Sherlock* obj) : BaseBag(obj, 13) {
    // Sherlock's bag can hold up to 13 items
}
WatsonBag::WatsonBag(Watson* obj) : BaseBag(obj, 15) {
    // Watson's bag can hold up to 15 items
}




// Get the item that the robot holds
BaseItem* Robot::getItem() const {
    return item;
}


// 3.13 StudyInPinkProgram

StudyPinkProgram::StudyPinkProgram(const string &config_file_path) {
    // Load configuration from the file
    config = new Configuration(config_file_path);

    // Create the map based on the configuration
    map = new Map(config->map_num_rows, config->map_num_cols, config->num_walls, config->arr_walls, config->num_fake_walls, config->arr_fake_walls);

    // Create Sherlock, Watson, and Criminal based on the configuration
    sherlock = new Sherlock(1, config->sherlock_moving_rule, config->sherlock_init_pos, map, config->sherlock_init_hp, config->sherlock_init_exp);
    watson = new Watson(2, config->watson_moving_rule, config->watson_init_pos, map, config->watson_init_hp, config->watson_init_exp);
    criminal = new Criminal(0, config->criminal_init_pos, map, sherlock, watson);

    // Initialize the array of moving objects with the maximum size from the configuration
    arr_mv_objs = new ArrayMovingObject(config->max_num_moving_objects);

    // Add the moving objects to the array
    arr_mv_objs->add(criminal);
    arr_mv_objs->add(sherlock);
    arr_mv_objs->add(watson);

    // Sherlock's bag
    sherlock_bag = new SherlockBag(sherlock);
    // Watson's bag
    watson_bag = new WatsonBag(watson);
}


StudyPinkProgram::~StudyPinkProgram() {
    // Clean up dynamically allocated memory
    delete config;
    delete map;
    delete sherlock;
    delete watson;
    delete criminal;
    delete arr_mv_objs;
}

bool StudyPinkProgram::isStop() const {
    // Check if Sherlock or Watson caught the criminal or if their HP is 0
    return sherlock->getHp() == 0 || watson->getHp() == 0 ||
           sherlock->getCurrentPosition().isEqual(criminal->getCurrentPosition()) ||
           watson->getCurrentPosition().isEqual(criminal->getCurrentPosition());
}


// Test the program
// Helper function about robot creation, if Criminal's total moves is 3 then create RobotC, else if % 3 == 0, compare to the distance between Sherlock and Watson. If the distance is the same, create RobotSW, if the distance is different, if the distance between Watson and the robot is less than the distance between Sherlock and the robot, create RobotW, else create RobotS
Robot* Robot::create(int index, Map* map, Criminal* criminal, Sherlock* sherlock, Watson* watson){
    Position init_pos = criminal->getPreviousPosition();
    int totalMoves = criminal->getCountCriminalMoves();
    if (totalMoves == 3) {
        return new RobotC(index, init_pos, map, criminal);
    }
    if (totalMoves % 3 == 0 && totalMoves > 3) {
        int distanceSherlock = manhattanDistance(init_pos, sherlock->getCurrentPosition());
        int distanceWatson = manhattanDistance(init_pos, watson->getCurrentPosition());
        if (distanceSherlock == distanceWatson) {
            return new RobotSW(index, init_pos, map, criminal, sherlock, watson);
        } else if (distanceWatson < distanceSherlock) {
            return new RobotW(index, init_pos, map, criminal, watson);
        } else {
            return new RobotS(index, init_pos, map, criminal, sherlock);
        }
    }
    return nullptr;
}
// Helper function to detect the type of item by position of Robot
BaseItem* createItem(Position pos){
    int i = pos.getRow();
    int j = pos.getCol();
    int p = i * j;
    int s = 0;
    while (p != 0) {
        s += p % 10;
        p /= 10;
    }
    while (s >= 10) {
        int temp = s;
        s = 0;
        while (temp != 0) {
            s += temp % 10;
            temp /= 10;
        }
    }
    //If s is in the range [0, 1], it will create MagicBook
    //If s is in the segment [2, 3], it will generate EnergyDrink
    //If s is in the segment [4, 5], it will generate FirstAid
    //If s is in the segment [6,7], it will generate ExcemptionCard
    BaseItem* item = nullptr;
    if (s <= 1) {
        item = new MagicBook();
    } else if (s <= 3) {
        item = new EnergyDrink();
    } else if (s <= 5) {
        item = new FirstAid();
    } else if (s <= 7) {
        item = new ExcemptionCard();
    } else {
        int t = (i * 11 + j) % 4;
        string challenge;
        switch (t) {
            case 0: challenge = "RobotS"; break;
            case 1: challenge = "RobotC"; break;
            case 2: challenge = "RobotSW"; break;
            case 3: challenge = "all"; break;
        }
        item = new PassingCard(challenge);
    }
    return item;

}
bool StudyPinkProgram::checkMeet(int i) const {
    // Case 1: Criminal
    if (arr_mv_objs->get(i)->getName() == "Criminal") {
        // If the Criminal is at the same position as Sherlock, and Sherlock's HP is greater than 500, the Criminal will be caught
        if (arr_mv_objs->get(i)->getCurrentPosition().isEqual(sherlock->getCurrentPosition()) &&
            sherlock->getHp() > 500) {
            return true;
        }
    }
    // Case 2: Sherlock
    if (arr_mv_objs->get(i)->getName() == "Sherlock") {
        for (int j = 0; j < arr_mv_objs->size(); ++j) {
            if (arr_mv_objs->get(j)->getName().substr(0, 5) == "Robot" &&
                arr_mv_objs->get(i)->getCurrentPosition().isEqual(arr_mv_objs->get(j)->getCurrentPosition()) && dynamic_cast<Robot *>(arr_mv_objs->get(j))->getIsDetected() == false) {
                cout << "Sherlock meets Robot" << endl;

                Robot *robot = dynamic_cast<Robot *>(arr_mv_objs->get(j));
                // Check if Sherlock has an ExemptionCard
                BaseItem *exemptionCard = sherlock_bag->check(EXCEMPTION_CARD);
                if (exemptionCard != nullptr && exemptionCard->canUse(sherlock, robot)) {
                    exemptionCard->use(sherlock, robot);
                    sherlock_bag->get(exemptionCard);
                    // Receive the item from Robot
                    sherlock_bag->insert(robot->getItem());
                    // Remove RobotS from the array of moving objects
                    //arr_mv_objs->remove(j);
                    return false;
                }
                // Handle each type of robot
                switch (robot->getRobotType()) {
                    case S: // RobotS
                        if (sherlock->getExp() > 400) {
                            sherlock_bag->insert(robot->getItem());
                            //arr_mv_objs->remove(j);
                            return false;
                        } else {
                            sherlock->setExp(sherlock->getExp() - sherlock->getExp() * 0.1);
                        }
                        break;
                    case W: // RobotW
                        sherlock_bag->insert(robot->getItem());
                        //arr_mv_objs->remove(j);
                        return false;
                    case SW: // RobotSW
                        if (sherlock->getExp() > 300 && sherlock->getHp() > 335) {
                            sherlock_bag->insert(robot->getItem());
                            //arr_mv_objs->remove(j);
                            return false;

                        } else {
                            sherlock->setExp(sherlock->getExp() - sherlock->getExp() * 0.15);
                            sherlock->setHp(sherlock->getHp() - sherlock->getHp() * 0.15);
                        }
                        break;
                    case C: // RobotC
                        if (sherlock->getExp() > 500) {
                            //arr_mv_objs->remove(j);
                            return true; // Sherlock caught the criminal
                        } else {
                            sherlock_bag->insert(robot->getItem());
                        }
                        break;
                }
                // Check if Sherlock has any item to heal or increase HP or EXP
                BaseItem *item = sherlock_bag->check();
                if (item != nullptr && item->canUse(sherlock, robot)) {
                    item->use(sherlock, robot);
                    sherlock_bag->get(item);
                }
//            } else if (arr_mv_objs->get(j)->getName() == "Watson" &&
//                     arr_mv_objs->get(i)->getCurrentPosition().isEqual(arr_mv_objs->get(j)->getCurrentPosition())) {
//                // Check if Sherlock has PassingCard and Watson has ExcepmtionCard, if so, Sherlock will give the PassingCard to Watson, if more than 1 item, Sherlock will give all PassingCard to Watson, and remove the item from Sherlock's bag, then use insert function to add the item to Watson's bag. Then Watson will give the ExemptionCard to Sherlock, if more than 1 item, Watson will give all ExemptionCard to Sherlock, and remove the item from Watson's bag, then use insert function to add the item to Sherlock's bag
//                BaseItem *passingCard = sherlock_bag->check(PASSING_CARD);
//                BaseItem *exemptionCard = watson_bag->check(EXCEMPTION_CARD);
//                if (passingCard != nullptr && exemptionCard != nullptr) {
//                    while (passingCard != nullptr) {
//                        sherlock_bag->get(passingCard);
//                        watson_bag->insert(passingCard);
//                        passingCard = sherlock_bag->check(PASSING_CARD);
//                    }
//                    while (exemptionCard != nullptr) {
//                        watson_bag->get(exemptionCard);
//                        sherlock_bag->insert(exemptionCard);
//                        exemptionCard = watson_bag->check(EXCEMPTION_CARD);
//                    }
//                }
                // Sherlock meets the Criminal, return true
            } else if (arr_mv_objs->get(j)->getName() == "Criminal" &&
                       arr_mv_objs->get(i)->getCurrentPosition().isEqual(
                               arr_mv_objs->get(j)->getCurrentPosition())) {
                return true;
            }
        }
    }
    // Case 3: Watson
    if (arr_mv_objs->get(i)->getName() == "Watson") {
        for (int j = 0; j < arr_mv_objs->size(); ++j) {
            // Watson meets the Criminal, return true
            if (arr_mv_objs->get(j)->getName() == "Criminal" &&
                arr_mv_objs->get(i)->getCurrentPosition().isEqual(arr_mv_objs->get(j)->getCurrentPosition())) {
                return true;
            } else if (arr_mv_objs->get(j)->getName().substr(0, 5) == "Robot" &&
                       arr_mv_objs->get(i)->getCurrentPosition().isEqual(
                               arr_mv_objs->get(j)->getCurrentPosition())) {
                Robot *robot = dynamic_cast<Robot *>(arr_mv_objs->get(j));
                // Check if Watson has a PassingCard
                cout << "Watson meets Robot" << endl;
                cout << watson_bag->str() << endl;
                BaseItem *passingCard = watson_bag->check(PASSING_CARD);
                if (passingCard != nullptr && passingCard->canUse(watson, robot)) {
                    passingCard->use(watson, robot);
                    watson_bag->get(passingCard);
                    // Remove Robot from the array of moving objects
                    //arr_mv_objs->remove(j);
                    // Receive the item from Robot
                    watson_bag->insert(robot->getItem());
                    cout << "Used PassingCard" << endl;
                    return false;
                }
                // Handle each type of robot
                switch (robot->getRobotType()) {
                    case S: // RobotS
                        // Watson does not perform any actions with RobotS and does not receive items held by this robot.
                        return false;
                        break;
                    case W: // RobotW
                        if (watson->getHp() > 350) {
                            watson_bag->insert(robot->getItem());
                            //arr_mv_objs->remove(j);
                            return false;
                        } else {
                            watson->setHp(watson->getHp() - watson->getHp() * 0.05);
                        }
                        break;
                    case SW: // RobotSW
                        if (watson->getExp() > 600 && watson->getHp() > 165) {
                            watson_bag->insert(robot->getItem());
                            //arr_mv_objs->remove(j);
                            return false;
                        } else {
                            watson->setExp(watson->getExp() - watson->getExp() * 0.15);
                            watson->setHp(watson->getHp() - watson->getHp() * 0.15);
                        }
                        break;
                    case C: // RobotC
                        // Watson meets RobotC, which means he has met the location adjacent to the criminal.
                        // Watson could not catch the criminal because he was held back by RobotC.
                        // However, Watson will still destroy the robot and receive the item this robot holds.
                        watson_bag->insert(robot->getItem());
                        //arr_mv_objs->remove(j);
                        break;
                }
                // Check if Watson has any item to heal or increase HP or EXP
                BaseItem *item = watson_bag->check();
                if (item != nullptr && item->canUse(watson, robot)) {
                    item->use(watson, robot);
                    watson_bag->get(item);
                } // Watson meets Sherlock
            } else if (arr_mv_objs->get(j)->getName() == "Sherlock" &&
                       arr_mv_objs->get(i)->getCurrentPosition().isEqual(arr_mv_objs->get(j)->getCurrentPosition())) {
                // Check if Sherlock has PassingCard and Watson has ExcepmtionCard, if so, Sherlock will give the PassingCard to Watson, if more than 1 item, Sherlock will give all PassingCard to Watson, and remove the item from Sherlock's bag, then use insert function to add the item to Watson's bag. Then Watson will give the ExemptionCard to Sherlock, if more than 1 item, Watson will give all ExemptionCard to Sherlock, and remove the item from Watson's bag, then use insert function to add the item to Sherlock's bag
                BaseItem *passingCard = sherlock_bag->check(PASSING_CARD);
                BaseItem *exemptionCard = watson_bag->check(EXCEMPTION_CARD);
                if (passingCard != nullptr && exemptionCard != nullptr) {
                    while (passingCard != nullptr) {
                        sherlock_bag->get(passingCard);
                        watson_bag->insert(passingCard);
                        passingCard = sherlock_bag->check(PASSING_CARD);
                    }
                    while (exemptionCard != nullptr) {
                        watson_bag->get(exemptionCard);
                        sherlock_bag->insert(exemptionCard);
                        exemptionCard = watson_bag->check(EXCEMPTION_CARD);
                    }
                }
            }
        }
    }
    // Case 4: Robot
    if (arr_mv_objs->get(i)->getName().substr(0, 5) == "Robot") {
        cout << "Robot moves, then check" << endl;
        for (int j = 0; j < arr_mv_objs->size(); ++j) {
            if (arr_mv_objs->get(j)->getName() == "Sherlock" &&
                arr_mv_objs->get(i)->getCurrentPosition().isEqual(arr_mv_objs->get(j)->getCurrentPosition())) {
                Robot *robot = dynamic_cast<Robot *>(arr_mv_objs->get(i));
                // Check if Sherlock has an ExemptionCard
                cout << "Robot meets Sherlock" << endl;

                BaseItem *exemptionCard = sherlock_bag->check(EXCEMPTION_CARD);
                if (exemptionCard != nullptr && exemptionCard->canUse(sherlock, robot)) {
                    exemptionCard->use(sherlock, robot);
                    sherlock_bag->get(exemptionCard);
                    // Receive the item from Robot
                    sherlock_bag->insert(robot->getItem());
                    // Remove RobotS from the array of moving objects
                    //arr_mv_objs->remove(j);
                    return false;
                }
                // Handle each type of robot
                switch (robot->getRobotType()) {
                    case S: // RobotS
                        if (sherlock->getExp() > 400) {
                            sherlock_bag->insert(robot->getItem());
                            //arr_mv_objs->remove(j);
                            return false;
                        } else {
                            sherlock->setExp(sherlock->getExp() - sherlock->getExp() * 0.1);
                        }
                        break;
                    case W: // RobotW
                        sherlock_bag->insert(robot->getItem());
                        //arr_mv_objs->remove(j);
                        return false;
                    case SW: // RobotSW
                        if (sherlock->getExp() > 300 && sherlock->getHp() > 335) {
                            sherlock_bag->insert(robot->getItem());
                            //arr_mv_objs->remove(j);
                            return false;

                        } else {
                            sherlock->setExp(sherlock->getExp() - sherlock->getExp() * 0.15);
                            sherlock->setHp(sherlock->getHp() - sherlock->getHp() * 0.15);
                        }
                        break;
                    case C: // RobotC
                        if (sherlock->getExp() > 500) {
                            //arr_mv_objs->remove(j);
                            return true; // Sherlock caught the criminal
                        } else {
                            sherlock_bag->insert(robot->getItem());
                        }
                        break;
                }
                // Check if Sherlock has any item to heal or increase HP or EXP
                BaseItem *item = sherlock_bag->check();
                if (item != nullptr && item->canUse(sherlock, robot)) {
                    item->use(sherlock, robot);
                    sherlock_bag->get(item);
                }
            } if (arr_mv_objs->get(j)->getName() == "Watson" &&
                 arr_mv_objs->get(i)->getCurrentPosition().isEqual(
                         arr_mv_objs->get(j)->getCurrentPosition())) {
                Robot *robot = dynamic_cast<Robot *>(arr_mv_objs->get(j));
                // Check if Watson has a PassingCard
                cout << "Robot meets Watson" << endl;

                BaseItem *passingCard = watson_bag->check(PASSING_CARD);
                if (passingCard != nullptr && passingCard->canUse(watson, robot)) {
                    passingCard->use(watson, robot);
                    watson_bag->get(passingCard);
                    // Remove Robot from the array of moving objects
                    //arr_mv_objs->remove(j);
                    // Receive the item from Robot
                    watson_bag->insert(robot->getItem());
                    return false;
                }
                // Handle each type of robot
                switch (robot->getRobotType()) {
                    case S: // RobotS
                        // Watson does not perform any actions with RobotS and does not receive items held by this robot.
                        return false;
                        break;
                    case W: // RobotW
                        if (watson->getHp() > 350) {
                            watson_bag->insert(robot->getItem());
                            //arr_mv_objs->remove(j);
                            return false;
                        } else {
                            watson->setHp(watson->getHp() - watson->getHp() * 0.05);
                        }
                        break;
                    case SW: // RobotSW
                        if (watson->getExp() > 600 && watson->getHp() > 165) {
                            watson_bag->insert(robot->getItem());
                            //arr_mv_objs->remove(j);
                            return false;
                        } else {
                            watson->setExp(watson->getExp() - watson->getExp() * 0.15);
                            watson->setHp(watson->getHp() - watson->getHp() * 0.15);
                        }
                        break;
                    case C: // RobotC
                        // Watson meets RobotC, which means he has met the location adjacent to the criminal.
                        // Watson could not catch the criminal because he was held back by RobotC.
                        // However, Watson will still destroy the robot and receive the item this robot holds.
                        watson_bag->insert(robot->getItem());
                        //arr_mv_objs->remove(j);
                        break;
                }
                // Check if Watson has any item to heal or increase HP or EXP
                BaseItem *item = watson_bag->check();
                if (item != nullptr && item->canUse(watson, robot)) {
                    item->use(watson, robot);
                    watson_bag->get(item);
                }
            }
        }
    }
    return false; // Sherlock did not catch the criminal

}
    void StudyPinkProgram::printMap(ostream &OUTPUT, int roundSize) const
{
    for (int i = 0; i < config->map_num_cols; i++)
    {
        if (i == -1)
            OUTPUT << setw(7) << ""
                   << "|";
        else
            OUTPUT << setw(7) << i << "|";
    }
    OUTPUT << endl;
    for (int i = 0; i < config->map_num_rows; i++)
    {
        //OUTPUT << setw(7) << i << "|";
        for (int j = 0; j < config->map_num_cols; j++)
        {
            Position pos(i, j);
            int idx = map->getElementType(pos);
            string nameElement[3] = {"     ", "IIIII", "-----"};
            string nameChar[4] = {"S", "W", "C", "R"};
            string robotName[4] = {"0", "1", "2", "3"};
            string cellValue = nameElement[idx];
            Position charPos(i, j);
            for (int k = 0; k < roundSize; k++)
            {
                if (arr_mv_objs->get(k)->getCurrentPosition().isEqual(charPos))
                {
                    if (cellValue == "     " || cellValue == "-----" || cellValue == "IIIII")
                        cellValue = "";
                    idx = arr_mv_objs->get(k)->getName() == "Criminal" ? 2 : arr_mv_objs->get(k)->getName() == "Sherlock" ? 0 : arr_mv_objs->get(k)->getName() == "Watson" ? 1 : 3;
                    if (idx == 3)
                    {
                        MovingObject *temp = arr_mv_objs->get(k);
                        cellValue += robotName[dynamic_cast<Robot *>(temp)->getRobotType()];
                        continue;
                    }
                    cellValue += nameChar[idx];
                }
            }
            if (!(cellValue == "     " || cellValue == "-----" || cellValue == "IIIII"))
                cellValue = "" + cellValue + "";
            OUTPUT << setw(7) << cellValue << "|";
        }
        OUTPUT << endl;
    }
}

/*
void StudyPinkProgram::run(std::ostream &OUTPUT)
{
    // Print the configuration
    OUTPUT << config->str() << endl;
    for (int istep = 0; istep < config->num_steps; ++istep)
    {
        OUTPUT << "__________________NEW STEP__________________" << endl;
        Robot *robot = nullptr;
        int roundSize = arr_mv_objs->size();
        for (int i = 0; i < roundSize; ++i)
        {

            OUTPUT << "ROUND : " << istep << " - TURN : " << i << endl;
            stringstream ss(arr_mv_objs->str());
            string lineArr = "";
            getline(ss, lineArr, 'C');
            OUTPUT << lineArr << "]" << endl;
            getline(ss, lineArr, ']');
            OUTPUT << "\tC" << lineArr.substr(1) << "]" << endl;
            while (getline(ss, lineArr, ']'))
            {
                if (lineArr.length() > 0)
                    OUTPUT << "\t" << lineArr.substr(1) << "]" << endl;
            }
            if (i == 0)
                OUTPUT << "Criminal current count : " << criminal->getCountCriminalMoves() << endl;
            if (i == 1)
                OUTPUT << "Sherlock move direction : " << config->sherlock_moving_rule[istep % config->sherlock_moving_rule.length()] << endl;
            if (i == 2)
                OUTPUT << "Watson move direction : " << config->watson_moving_rule[istep % config->watson_moving_rule.length()] << endl;
            arr_mv_objs->get(i)->move();
            if (i == 0)
                OUTPUT << "Criminal count after moving : " << criminal->getCountCriminalMoves() << endl;
            if (arr_mv_objs->get(i)->getName() == "Criminal")
            {
                robot = Robot::create(arr_mv_objs->size(), map, criminal, sherlock, watson);
            }
            printMap(OUTPUT, roundSize);
            OUTPUT << "*********************************************************" << endl;
        }
        if (robot != nullptr)
        {
            if ((criminal->getCountCriminalMoves()) % 3 == 0 && criminal->getCountCriminalMoves() > 0)
            {
                BaseItem*item = createItem(criminal->getPreviousPosition());
                robot->setItem(item);
                arr_mv_objs->add(robot);
                string robotType[4] = {"C", "S", "W", "SW"};
                //OUTPUT << "Robot " << robotType[dynamic_cast<Robot *>(robot)->getType()] << " has been created" << endl;
                //OUTPUT << "Robot " << robotType[dynamic_cast<Robot *>(robot)->getRobotType()] << " has been created" << endl;
                // Use robotTypeToString()
                Robot* robotInstance = dynamic_cast<Robot*>(robot);
                OUTPUT << "Robot " << robotInstance->robotTypeToString() << " has been created" << endl;
            }
            else
            {
                // delete robot;
            }
        }
    }
}

*/


void StudyPinkProgram::run(ostream &OUTPUT){
    bool stop = checkMeet(0) || sherlock->getHp() == 0 || watson->getHp() == 0;
    OUTPUT << config->str() << endl;
    for (int istep = 0; istep < config->num_steps && !stop; ++istep)
    {
        for (int i = 0; i < arr_mv_objs->size() && !stop; ++i)
        {

            if (stop)
                break;
            OUTPUT << endl
                   << "*********************************************************" << endl;
            OUTPUT << "ROUND : " << istep << " - TURN : " << i << endl;

            Robot *robot = nullptr;
            arr_mv_objs->get(i)->move();
            if ((arr_mv_objs->get(i)->getName())== "Criminal")
            {
                robot = Robot::create(arr_mv_objs->size(), map, criminal, sherlock, watson);
            }
            if (robot != nullptr)
            {
                if (criminal->getCountCriminalMoves() % 3 == 0 && criminal->getCountCriminalMoves() > 0)
                {
                    //BaseItem*item = createItem(criminal->getPreviousPosition());
                    //robot->setItem(item);
                    arr_mv_objs->add(robot);
                }
                else
                {
                    delete robot;
                }
            }
            stop = checkMeet(i);
            int roundSize = arr_mv_objs->size();
            printMap(OUTPUT, roundSize);
            OUTPUT << "---------------" << endl
                   << "LOG : " << endl
                   << arr_mv_objs->str() << endl
                   << "Sherlock HP_" << sherlock->getHp() << " EXP_" << sherlock->getExp() << endl
                   << "Watson HP_" << watson->getHp() << " EXP_" << watson->getExp() << endl
                   << "SherlockBag : " << sherlock_bag->str() << endl
                   << "WatsonBag : " << sherlock_bag->str() << endl;
        }
    }
    OUTPUT << "---------------" << endl
           << "RESULT : ";

    if (watson->getHp() == 0)
        OUTPUT << "Watson can not continue the journey";
    else if (sherlock->getHp() == 0)
        OUTPUT << "Sherlock can not continue the journey";
    else if (stop)
        OUTPUT << "The Criminal was caught";
    else
        OUTPUT << "The Criminal escaped";
}





int main() {

/*
    // Testcase configuration file
    string config_file_path = "config.txt";
    // Import the configuration from the file
    Configuration config(config_file_path);
    cout << config.str() << endl;
    // Run the program


    // Create a 5x5 map with some walls and fake walls
    int num_rows = 5;
    int num_cols = 5;
    int num_walls = 2;
    Position array_walls[2] = {Position(1, 1), Position(3, 3)};
    int num_fake_walls = 1;
    Position array_fake_walls[1] = {Position(2, 2)};
    Map map(num_rows, num_cols, num_walls, array_walls, num_fake_walls, array_fake_walls);

    // Create some moving objects
    Sherlock sherlock(0, "LRUD", Position(0, 0), &map, 100, 100);
    Watson watson(1, "LRUD", Position(0, 1), &map, 100, 100);
    Criminal criminal(2, Position(0, 2), &map, &sherlock, &watson);

    // Test the isValid function with various positions and moving objects
    assert(map.isValid(Position(0, 0), &sherlock) == true); // Path
    assert(map.isValid(Position(1, 1), &sherlock) == false); // Wall
    assert(map.isValid(Position(2, 2), &sherlock) == true); // FakeWall (Sherlock can always detect a fake wall)
    assert(map.isValid(Position(2, 2), &watson) == false); // FakeWall (Watson does not have enough EXP)
    assert(map.isValid(Position(0, 0), &criminal) == true); // Path
    assert(map.isValid(Position(1, 1), &criminal) == false); // Wall
    assert(map.isValid(Position(2, 2), &criminal) == true); // FakeWall (Criminal can always detect a fake wall)
    assert(map.isValid(Position(5, 5), &sherlock) == false); // Out of bounds
    assert(map.isValid(Position(-1, -1), &sherlock) == false); // Out of bounds

    cout << "All tests passed!" << endl;

    return 0;

    // Create a 5x5 map with some walls and fake walls
    int num_rows = 5;
    int num_cols = 5;
    int num_walls = 2;
    Position array_walls[2] = {Position(1, 1), Position(3, 3)};
    int num_fake_walls = 1;
    Position array_fake_walls[1] = {Position(2, 2)};
    Map map(num_rows, num_cols, num_walls, array_walls, num_fake_walls, array_fake_walls);

    // Create some moving objects
    Sherlock sherlock(0, "LRUD", Position(0, 0), &map, 100, 100);
    Watson watson(1, "LRUD", Position(0, 1), &map, 100, 100);
    Criminal criminal(2, Position(0, 2), &map, &sherlock, &watson);

    // Create an ArrayMovingObject with a capacity of 3
    ArrayMovingObject arr_mv_objs(3);

    // Test the add function
    assert(arr_mv_objs.add(&sherlock) == true);
    assert(arr_mv_objs.add(&watson) == true);
    assert(arr_mv_objs.add(&criminal) == true);

    // Test the isFull function
    assert(arr_mv_objs.isFull() == true);

    // Test the size function
    assert(arr_mv_objs.size() == 3);

    // Test the get function
    assert(arr_mv_objs.get(0) == &sherlock);
    assert(arr_mv_objs.get(1) == &watson);
    assert(arr_mv_objs.get(2) == &criminal);

    cout << "All tests passed!" << endl;

    return 0;

*/
    StudyPinkProgram program("config.txt");
    program.run(cout);

//
}


////////////////////////////////////////////////
/// END OF STUDENT'S ANSWER
////////////////////////////////////////////////