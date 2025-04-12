#include "StudentWorld.h"
#include "GameConstants.h"
#include <string>
using namespace std;

#include <iostream>
#include <sstream>
#include <iomanip>
GameWorld* createStudentWorld(string assetPath)
{
    return new StudentWorld(assetPath);
}

// Students:  Add code to this file, StudentWorld.h, Actor.h, and Actor.cpp

StudentWorld::StudentWorld(string assetPath)
: GameWorld(assetPath), m_player(nullptr)
{}

Level::MazeEntry StudentWorld::getLevelContents(int x, int y) const {
    Level lev(assetPath());
    string levelFile = (getLevel() < 10 ? "level0" : "level") + to_string(getLevel()) + ".txt";
    if (lev.loadLevel(levelFile) == Level::load_success) {
        
        return lev.getContentsOf(x, y);
    }
    return Level::empty; // default case, should not happen unless file does not load
}


bool StudentWorld::isPlayerAt(int x, int y) const {
    return (m_player->getX() == x && m_player->getY() == y);
}

bool StudentWorld::isFloorAt(int x, int y) const {
    return getLevelContents(x, y) == Level::floor;

}
Player* StudentWorld::getPlayer() const { 
    return m_player; 
}
int StudentWorld::init()
{
    Level lev(assetPath());

    int levelNumber = getLevel();
    string levelFile = (getLevel() < 10 ? "level0" : "level") + to_string(getLevel()) + ".txt";
   
    Level::LoadResult result = lev.loadLevel(levelFile);

    if (result == Level::load_fail_file_not_found) {
        return GWSTATUS_PLAYER_WON; // No more levels
    }
    if (result == Level::load_fail_bad_format) {
        return GWSTATUS_LEVEL_ERROR;
    }

    for (int x = 0; x < 20; x++) {
        for (int y = 0; y < 20; y++) {
            Level::MazeEntry item = lev.getContentsOf(x, y);

            switch (item)
            {
            case Level::empty:
                break;
            case Level::floor:
                    
                m_actors.push_back(new Floor(x, y, this)); 
                break;
            case Level::player:
            {
        
                m_player = new Player(x, y, this, 0);
                m_actors.push_back(m_player);
                break;
            }
                
            case Level::ladder:
                m_actors.push_back(new Ladder(x, y, this));
                break;


            case Level::bonfire:
                m_actors.push_back(new Bonfire(x, y, this));
                break;


            case Level::extra_life:
                m_actors.push_back(new ExtraLifeGoodie(x, y, this));
                break;
                

            case Level::garlic:
                m_actors.push_back(new GarlicGoodie(x, y, this));
                break;

            case Level::koopa:
                m_actors.push_back(new Koopa(x, y, this));
                break;

            case Level::left_kong:
                m_actors.push_back(new Kong(x, y, GraphObject::left, this));
                break;
            case Level::right_kong:
                m_actors.push_back(new Kong(x, y, GraphObject::right, this));
                break;

            case Level::fireball:
                m_actors.push_back(new Fireball(x, y, this));
                break;
            default:
                break;
            }
        }
    }

    return GWSTATUS_CONTINUE_GAME;
}


bool StudentWorld::isLadderAt(int x, int y) const {
    return getLevelContents(x, y) == Level::ladder;
}

bool StudentWorld::isBonfireAt(int x, int y) const {
    return getLevelContents(x, y) == Level::bonfire;
}

std::vector<Actor*> StudentWorld::getActors() const {
    return m_actors;
}
void StudentWorld::addActor(Actor* actor) {
    m_actors.push_back(actor);
}

int StudentWorld::move()
{
    for (Actor* actor : m_actors) {
        if (actor->Alive()) {
            actor->doSomething();
        }          
    }

    if (!m_player->Alive()) {
        decLives();
        return GWSTATUS_PLAYER_DIED;
    }
    for (Actor* actor : m_actors) { //flees
        if (actor->isKong() && !actor->Alive()) {
            return GWSTATUS_FINISHED_LEVEL;
        }
    }

    for (vector<Actor*>::iterator it = m_actors.begin(); it != m_actors.end(); /* increment handled in code*/) {
        if (!(*it)->Alive()) {
            delete* it;
            it = m_actors.erase(it);
        }
        else {
            it++;
        }
    }

    stringstream oss;
    oss.fill('0');
    oss << "Score: " << setw(7) << getScore() << "  Level: " << setw(2) << getLevel() << "  Lives: " << setw(2) << getLives() << "  Burps: " << setw(2) << m_player->getBurps();

    setGameStatText(oss.str());
    
    return GWSTATUS_CONTINUE_GAME;
}

void StudentWorld::cleanUp()
{   
    while (!m_actors.empty()) {
        delete m_actors.front(); 
        m_actors.erase(m_actors.begin());
    }   
}

StudentWorld::~StudentWorld() {
    cleanUp();
}