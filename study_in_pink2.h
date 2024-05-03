/*
* Ho Chi Minh City University of Technology
* Faculty of Computer Science and Engineering
* Initial code for Assignment 1
* Programming Fundamentals Spring 2023
* Author: Vu Van Tien
* Date: 02.02.2023
*/

//The library here is concretely set, students are not allowed to include any other libraries.
#ifndef _H_STUDY_IN_PINK_2_H_
#define _H_STUDY_IN_PINK_2_H_

#include "main.h"

////////////////////////////////////////////////////////////////////////
/// STUDENT'S ANSWER BEGINS HERE
/// Complete the following functions
/// DO NOT modify any parameters in the functions.
////////////////////////////////////////////////////////////////////////

// Forward declaration
class MovingObject;
class Position;
// class Configuration;
class Map;
// class Criminal;
// class RobotS;
// class RobotW;
// class RobotSW;
// class RobotC;

// class ArrayMovingObject;
// class StudyPinkProgram;

class BaseItem;
class BaseBag;
class SherlockBag;
class WatsonBag;

class TestStudyInPink;

enum ItemType { MAGIC_BOOK, ENERGY_DRINK, FIRST_AID, EXCEMPTION_CARD, PASSING_CARD };
enum ElementType { PATH, WALL, FAKE_WALL };
enum RobotType { C=0, S, W, SW };

class MapElement {
friend class TestStudyInPink;
protected:
    ElementType type;
public:
    MapElement(ElementType in_type);
    virtual ~MapElement();
    virtual ElementType getType() const;
};

class Path : public MapElement {
friend class TestStudyInPink;
public:
    Path();
};

class Wall : public MapElement {
friend class TestStudyInPink;
public:
    Wall();
};

class FakeWall : public MapElement {
friend class TestStudyInPink;
private:
    int req_exp;
public:
    FakeWall(int in_req_exp);
    int getReqExp() const;
};

class Map {
friend class TestStudyInPink;
private:
    int num_rows, num_cols;
    MapElement *** map;

public:
    Map(int num_rows, int num_cols, int num_walls, Position * array_walls, int num_fake_walls, Position * array_fake_walls);
    ~Map();
    bool isValid ( const Position & pos , MovingObject * mv_obj ) const ;
    ElementType getElementType(const Position &pos) const;

};

class Position {
friend class TestStudyInPink;
private:
    int r, c;
public:
    static const Position npos;

    Position(int r=0, int c=0);
    Position(const string & str_pos);

    int getRow() const;
    int getCol() const;
    void setRow(int r);
    void setCol(int c);

    string str() const;

    bool isEqual(int in_r, int in_c) const;
    bool isEqual(const Position in_position) const;
    bool operator!=(const Position& other) const {return (r != other.r || c != other.c);}; // Overload operator !=
};

class MovingObject {
friend class TestStudyInPink;
protected:
    int index;
    Position pos;
    Map * map;
    string name;

public:
    MovingObject(int index, const Position pos, Map * map, const string & name="");
    virtual ~MovingObject();
    virtual Position getNextPosition() = 0;
    Position getCurrentPosition() const;
    virtual void move() = 0;
    virtual string str() const = 0;
    string getName() const { return name; }
};
class Character : public MovingObject {
public:
    Character(int index, const Position pos, Map * map, const string & name="") : MovingObject(index, pos, map, name) {}
    virtual ~Character() {}

};

class Sherlock : public Character {
friend class TestStudyInPink;
private:
    string moving_rule;
    int hp;
    int exp;
    size_t move_index;

public:
    Sherlock(int index, const string & moving_rule, const Position & init_pos, Map * map, int init_hp, int init_exp);
    Position getNextPosition();
    void move();
    string str() const;

    int getHp() const  { return hp; }
    int getExp() const  { return exp; }
    void setHp(int init_hp)   {
        if (init_hp < 0) {
            hp = 0;
        } else if (init_hp > 500) {
            hp = 500;
        } else {
            hp = init_hp;
        }
    }
    void setExp(int init_exp)  {
        if (init_exp < 0) {
            exp = 0;
        } else if (init_exp > 900) {
            exp = 900;
        } else {
            exp = init_exp;
        }
    }

};



class Watson : public Character {
    friend class TestStudyInPink;
private:
    string moving_rule;
    int hp;
    int exp;
    size_t move_index;

public:
    Watson(int index, const string & moving_rule, const Position & init_pos, Map * map, int init_hp, int init_exp);
    Position getNextPosition();
    void move();
    string str() const;
    int getHp() const { return hp; }
    int getExp() const { return exp; }
    void setHp(int init_hp)  {
        if (init_hp < 0) {
            hp = 0;
        } else if (init_hp > 500) {
            hp = 500;
        } else {
            hp = init_hp;
        }
    }
    void setExp(int init_exp)  {
        if (init_exp < 0) {
            exp = 0;
        } else if (init_exp > 900) {
            exp = 900;
        } else {
            exp = init_exp;
        }
    }

};

class Criminal : public Character {
    friend class TestStudyInPink;
private:
    Sherlock* sherlock;
    Watson* watson;
    int count_criminal_moves;
    Position last_position;
public:
    Criminal(int index, const Position& init_pos, Map* map, Sherlock* sherlock, Watson* watson);
    Position getNextPosition() override;
    void move() override;
    string str() const override;
    int getCountCriminalMoves() const { return count_criminal_moves; }
    Position getPreviousPosition() const { return last_position; }
};


class ArrayMovingObject {
friend class TestStudyInPink;
private:
    MovingObject** arr_mv_objs; // Array of moving object pointers
    int count;                  // Current number of elements in the array
    int capacity;               // Maximum number of elements in the array

public:
    ArrayMovingObject(int capacity);
    //~ArrayMovingObject() ;
    ~ArrayMovingObject() = default ;
    bool isFull() const;
    bool add(MovingObject * mv_obj);
    MovingObject * get(int index) const;
    int size() const; // Return current number of elements in the array
    string str() const;
};

class Configuration {
    friend class StudyPinkProgram;
    friend class TestStudyInPink;
    friend int main();
    

private:
    int map_num_rows, map_num_cols;
    int max_num_moving_objects;
    int num_walls;
    Position *arr_walls;
    int num_fake_walls;
    Position *arr_fake_walls;
    string sherlock_moving_rule;
    Position sherlock_init_pos;
    int sherlock_init_hp;
    int sherlock_init_exp;
    string watson_moving_rule;
    Position watson_init_pos;
    int watson_init_hp;
    int watson_init_exp;
    Position criminal_init_pos;
    int num_steps;

public:
    Configuration(const string & filepath);
    ~Configuration();
    string str() const;
};

// Start part: Robot, BaseItem, BaseBag,...

class Robot : public MovingObject {
protected:
    RobotType robot_type;
    BaseItem* item; // Pointer to the item the robot holds

public:
    Robot(int index, const Position& init_pos, Map* map, RobotType robot_type);
    virtual ~Robot();
    virtual int getDistance() const = 0;
    virtual void move() override;
    virtual string str() const override;
    virtual Position getNextPosition() = 0;
    RobotType getRobotType() const { return robot_type; }
    // Convert Robot type to string
    string robotTypeToString() const;
    // function create new robot
    static Robot* create(int index, Map* map, Criminal* criminal, Sherlock* sherlock, Watson* watson);

};

// RobotC class
class RobotC : public Robot {
private:
    Criminal* criminal; // Pointer to the criminal
    Sherlock* sherlock; // Pointer to Sherlock
    Watson* watson; // Pointer to Watson

public:
    RobotC(int index, const Position& init_pos, Map* map, Criminal* criminal);
    virtual Position getNextPosition() override;
    virtual int getDistance() const override;
    int getDistance(MovingObject* obj) const;
    string str() const override;
    // str and move are inherited from the base class
};

// RobotS class
class RobotS : public Robot {
private:
    Criminal* criminal; // Pointer to the criminal
    Sherlock* sherlock; // Pointer to Sherlock

public:
    RobotS(int index, const Position& init_pos, Map* map, Criminal* criminal, Sherlock* sherlock);
    virtual Position getNextPosition() override;
    virtual int getDistance() const override;
};

// RobotW class
class RobotW : public Robot {
private:
    Criminal* criminal; // Pointer to the criminal
    Watson* watson; // Pointer to Watson

public:
    RobotW(int index, const Position& init_pos, Map* map, Criminal* criminal, Watson* watson);
    virtual Position getNextPosition() override;
    virtual int getDistance() const override;
};

// RobotSW class
class RobotSW : public Robot {
private:
    Criminal* criminal; // Pointer to the criminal
    Sherlock* sherlock; // Pointer to Sherlock
    Watson* watson; // Pointer to Watson

public:
    RobotSW(int index, const Position& init_pos, Map* map, Criminal* criminal, Sherlock* sherlock, Watson* watson);
    virtual Position getNextPosition() override;
    virtual int getDistance() const override;
};


// 3.11 Item classes
class BaseItem {
public:
    virtual bool canUse(Character* obj, Robot * robot) = 0;
    virtual void use(Character* obj, Robot * robot) = 0;
};

class MagicBook : public BaseItem {
public:
    bool canUse(Character* obj, Robot * robot) override;
    void use(Character* obj, Robot * robot) override;
};

class EnergyDrink : public BaseItem {
public:
    bool canUse(Character* obj, Robot * robot) override;
    void use(Character* obj, Robot * robot) override;
};

class FirstAid : public BaseItem {
public:
    bool canUse(Character* obj, Robot * robot) override;
    void use(Character* obj, Robot * robot) override;
};

class ExcemptionCard : public BaseItem {
public:
    bool canUse(Character* obj, Robot * robot) override;
    void use(Character* obj, Robot * robot) override;
};

class PassingCard : public BaseItem {
private:
    string challenge;
public:
    PassingCard(const string & challenge) : challenge(challenge) {}
    bool canUse(Character* obj, Robot * robot) override;
    void use(Character* obj, Robot * robot) override;
};




// 3.12 Bag classes

class BaseBag {
    friend class TestStudyInPink;
protected:
    Character* obj; // The character that owns the bag
    BaseItem** items; // Array to store items, simulating a singly linked list
    int count; // Current number of items in the bag
    int capacity; // Maximum number of items the bag can hold

public:
    BaseBag(Character* obj, int capacity);
    virtual ~BaseBag();
    virtual bool insert(BaseItem* item); // Add an item to the inventory
    virtual BaseItem* get(); // Use any item
    virtual BaseItem* get(ItemType itemType); // Use a specific item
    virtual string str() const; // String representation of the bag
    string getItemName(BaseItem* item) const; // New member function declaration

};

// SherlockBag class
class SherlockBag : public BaseBag {
public:
    SherlockBag(Sherlock* sherlock);
};

// WatsonBag class
class WatsonBag : public BaseBag {
public:
    WatsonBag(Watson* watson);
};




// End part: Robot, BaseItem, BaseBag,...
class StudyPinkProgram {
friend class TestStudyInPink;
friend int main();
private:
    // Sample attributes
    Configuration * config;
    
    Sherlock * sherlock;
    Watson * watson;
    Criminal * criminal;
    
    Map * map;
    ArrayMovingObject * arr_mv_objs;


public:
    StudyPinkProgram(const string & config_file_path);

    bool isStop() const;

    void printResult() const {
        if (sherlock->getCurrentPosition().isEqual(criminal->getCurrentPosition())) {
            cout << "Sherlock caught the criminal" << endl;
        }
        else if (watson->getCurrentPosition().isEqual(criminal->getCurrentPosition())) {
            cout << "Watson caught the criminal" << endl;
        }
        else {
            cout << "The criminal escaped" << endl;
        }
    }

    void printStep(int si) const {
        cout << "Step: " << setw(4) << setfill('0') << si
            << "--"
            << sherlock->str() << "--|--" << watson->str() << "--|--" << criminal->str() << endl;
    }
    void run(std::ostream &OUTPUT);
    void run(bool verbose) {
        // Note: This is a sample code. You can change the implementation as you like.
        // TODO
        for (int istep = 0; istep < config->num_steps; ++istep) {
            for (int i = 0; i < arr_mv_objs->size(); ++i) {
                arr_mv_objs->get(i)->move();
                if (isStop()) {
                    printStep(istep);
                    break;
                }
                if (verbose) {
                    printStep(istep);
                }
            }
        }
        printResult();
    }
    void printMap(ostream &OUTPUT, int roundSize) const;

    ~StudyPinkProgram();
};



////////////////////////////////////////////////
/// END OF STUDENT'S ANSWER
////////////////////////////////////////////////
#endif /* _H_STUDY_IN_PINK_2_H_ */
