#include "Actor.h"
#include "StudentWorld.h"

// Students:  Add code to this file, Actor.h, StudentWorld.h, and StudentWorld.cpp
#include <iostream>
using namespace std;
Actor::Actor(int imageID, int startX, int startY, int startDirection, StudentWorld* world)
    : GraphObject(imageID, startX, startY, startDirection), m_world(world), m_alive(true) {}

void Actor::setAlive(bool alive) {
    m_alive = alive;
}

bool Actor::Alive() {
    return m_alive;
}

void Actor::beAttacked() {
    return;
}

bool Actor::isKong() {
    return false;
}

bool Actor::applyGravity() {
    if (!getWorld()->isFloorAt(getX(), getY() - 1) && getY() > 0) {
        moveTo(getX(), getY() - 1);
        return true; // Fell
    }
    return false;// Not fall
}

StudentWorld* Actor::getWorld() const {
    return m_world;
}

Floor::Floor(int startX, int startY, StudentWorld* world)
    : Actor(IID_FLOOR, startX, startY, none, world) {}

void Floor::doSomething() { // Must implement pure virtual function, so this does nothing

    return;
}

Ladder::Ladder(int startX, int startY, StudentWorld* world)
    : Actor(IID_LADDER, startX, startY, none, world) {}

void Ladder::doSomething() {
    return; // Do nothing for ladders
}

Player::Player(int startX, int startY, StudentWorld* world, int burps)
    : Actor(IID_PLAYER, startX, startY, right, world), m_burps(burps), m_jumpTicks(0), m_freezeTicks(0)
{
    setAlive(true);
}



int Player::getBurps() const {
    return m_burps;
}

void Player::addBurps(int n) {
    m_burps += n;
}


void Player::freeze(int ticks) {
    m_freezeTicks = ticks;
}

void Player::beAttacked() {
    setAlive(false);
    getWorld()->playSound(SOUND_PLAYER_DIE);
    return;

}

Bonfire::Bonfire(int x, int y, StudentWorld* world)
    : Actor(IID_BONFIRE, x, y, none, world) {}

void Bonfire::doSomething() {

    if (getWorld()->isPlayerAt(getX(), getY()) && getWorld()->getPlayer()->Alive()) {
        getWorld()->getPlayer()->beAttacked();
    }
    increaseAnimationNumber();
}



Goodie::Goodie(int imageID, int startX, int startY, StudentWorld* world)
    : Actor(imageID, startX, startY, none, world) {}

void Goodie::doSomething() {
    if (!Alive()) {
        return;
    }
    if (getWorld()->isPlayerAt(getX(), getY())) {
        getWorld()->increaseScore(getScoreValue());
        setAlive(false);
        getWorld()->playSound(SOUND_GOT_GOODIE);
        conferPowerUp();
    }
}

ExtraLifeGoodie::ExtraLifeGoodie(int startX, int startY, StudentWorld* world)
    : Goodie(IID_EXTRA_LIFE_GOODIE, startX, startY, world) {}

int ExtraLifeGoodie::getScoreValue() const {
    return 50;
}

void ExtraLifeGoodie::conferPowerUp() {
    getWorld()->incLives();
}

GarlicGoodie::GarlicGoodie(int startX, int startY, StudentWorld* world)
    : Goodie(IID_GARLIC_GOODIE, startX, startY, world) {}

int GarlicGoodie::getScoreValue() const {
    return 25;
}

void GarlicGoodie::conferPowerUp() {
    getWorld()->getPlayer()->addBurps(5);
}


Burp::Burp(int startX, int startY, int direction, StudentWorld* world)
    : Actor(IID_BURP, startX, startY, direction, world), m_lifetime(5) {}



Enemy::Enemy(int imageID, int startX, int startY, int startDirection, StudentWorld* world)
    : Actor(imageID, startX, startY, startDirection, world), m_tickCounter(0) {}

void Enemy::reverseDirection(){
    setDirection(getDirection() == right ? left : right);
}

void Enemy::increaseTickCounter() {
    m_tickCounter++;
}
void Enemy::setTickCounter(int m) {
    m_tickCounter = m;
}
int Enemy::getTickCounter() const {
    return m_tickCounter;
}

void Enemy::attackPlayer() const{
    getWorld()->getPlayer()->beAttacked();
}

void Enemy::die() {
    setAlive(false);
    getWorld()->playSound(SOUND_ENEMY_DIE);
    getWorld()->increaseScore(100);
}

bool Enemy::attackPlayerIfAtSamePosition() {
    if (getWorld()->isPlayerAt(getX(), getY())) {
        attackPlayer();
        return true;
    }
    return false;
}

Koopa::Koopa(int startX, int startY, StudentWorld* world)
    : Enemy(IID_KOOPA, startX, startY, randInt(0, 1) == 0 ? left : right, world), m_freezeCooldown(0) {}



void Burp::doSomething() {
    if (!Alive()) {
        return;
    }

    m_lifetime--;
    if (m_lifetime <= 0) {
        setAlive(false);
        return;
    }

    for (Actor* actor : getWorld()->getActors()) {
        if (actor != this && actor->getX() == getX() && actor->getY() == getY() && actor->Alive()) {
            actor->beAttacked();


        }
    }
}

void Koopa::doSomething() {
    if (!Alive()) {
        return;
    }

    // Freeze Player if on same square and cooldown is 0
    if (getWorld()->isPlayerAt(getX(), getY()) && m_freezeCooldown == 0) {
        getWorld()->getPlayer()->freeze(50);
        m_freezeCooldown = 50;
        return;
    }

    // Decrement freeze cooldown if active
    if (m_freezeCooldown > 0) {
        m_freezeCooldown--;
    }

    // Move every 10 ticks
    increaseTickCounter();
    if (getTickCounter() >= 10) {
        setTickCounter(0);
        int newX = (getDirection() == right) ? getX() + 1 : getX() - 1;

        // reverse direction if next square is solid or unsupported
        if (getWorld()->isFloorAt(newX, getY()) || (!getWorld()->isFloorAt(newX, getY() - 1) && !getWorld()->isLadderAt(newX, getY() - 1))) {
            reverseDirection();
        }
        else {
            moveTo(newX, getY()); // Move in current direction
        }

        
        if (getWorld()->isPlayerAt(getX(), getY()) && m_freezeCooldown == 0) {
            getWorld()->getPlayer()->freeze(50);
            m_freezeCooldown = 50;
            return;
        }
    }
}

void Koopa::beAttacked() {
    die();
    if (randInt(1, 3) == 1) {
        getWorld()->addActor(new ExtraLifeGoodie(getX(), getY(), getWorld()));
    }
}


void Player::doSomething() {
    if (!Alive()) {
        return;
    }

    if (m_jumpTicks > 0) {
        if (m_jumpTicks == 5) {
            if (!getWorld()->isFloorAt(getX(), getY() + 1) && getY() < 19) {
                moveTo(getX(), getY() + 1);
                // if new position is a ladder, stop jump
                if (getWorld()->isLadderAt(getX(), getY())) {
                    m_jumpTicks = 0;
                }
            }
        }
        else if (m_jumpTicks >= 2 && m_jumpTicks <= 4) {
            int newX;
            if (getDirection() == right) {
                newX = getX() + 1;
            }
            else {
                newX = getX() - 1;
            }
            if (!getWorld()->isFloorAt(newX, getY()) && newX >= 0 && newX < 20) {
                moveTo(newX, getY());
                // Check if new position is a ladder. if so, stop jump
                if (getWorld()->isLadderAt(newX, getY())) {
                    m_jumpTicks = 0;
                }
            }
            else {
                m_jumpTicks = 1; // Abort
            }
        }
        else if (m_jumpTicks == 1) {
            if (!getWorld()->isFloorAt(getX(), getY() - 1) && getY() > 0) {
                moveTo(getX(), getY() - 1);
                // Check if new position is a ladder. if so, stop jump
                if (getWorld()->isLadderAt(getX(), getY())) {
                    m_jumpTicks = 0;
                }
            }
        }
        m_jumpTicks--;
        return;
    }

    if (m_freezeTicks > 0) {
        m_freezeTicks--;
        return; // Skip all actions while frozen
    }
    //gravity
    if (!getWorld()->isFloorAt(getX(), getY() - 1) && !getWorld()->isLadderAt(getX(), getY() - 1) && !getWorld()->isLadderAt(getX(), getY()) && getY() > 0) {

        applyGravity();
        return;
    }

    int ch;
    if (getWorld()->getKey(ch)) {
        switch (ch) {
        case KEY_PRESS_LEFT:
            if (getDirection() != left) {
                setDirection(left);
            }
            else if (!getWorld()->isFloorAt(getX() - 1, getY())) { // Check for floor
                moveTo(getX() - 1, getY()); // left
            }
            break;


        case KEY_PRESS_RIGHT:
            if (getDirection() != right) {
                setDirection(right);
            }
            else if (!getWorld()->isFloorAt(getX() + 1, getY())) { // Check for floor
                moveTo(getX() + 1, getY()); // right
            }
            break;


        case KEY_PRESS_SPACE:
            if (!getWorld()->isFloorAt(getX(), getY() + 1)) {
                if (getWorld()->isFloorAt(getX(), getY() - 1) || getWorld()->isLadderAt(getX(), getY() - 1)) { // Have to have floor or ladder directly below to jump
                    m_jumpTicks = 5;
                    

                }
                else if (getWorld()->isLadderAt(getX(), getY())) {
                    // check if at top rung
                    if (!getWorld()->isLadderAt(getX(), getY() + 1) && getY() < 19) {


                        // Top rung gets complete jump
                        m_jumpTicks = 5;
                        
                    }
                    else if (getY() < 19 && !getWorld()->isFloorAt(getX(), getY() + 1)) {

                        // Not top rung, move up one rung
                        moveTo(getX(), getY() + 1);
                    }
                }
            }
            getWorld()->playSound(SOUND_JUMP);

            break;


        case KEY_PRESS_UP:
            // ladder up
            if (getWorld()->isLadderAt(getX(), getY())) {

                int newY = getY() + 1;
                moveTo(getX(), newY);

            }
            break;


        case KEY_PRESS_DOWN: // ladder down
            if (!getWorld()->isFloorAt(getX(), getY() - 1)) {
                int newY = getY() - 1;
                if (newY >= 0 && !getWorld()->isFloorAt(getX(), newY)) {
                    moveTo(getX(), newY);
                }

            }
            break;


        case KEY_PRESS_TAB:  // burping
            if (m_burps > 0) {
                getWorld()->playSound(SOUND_BURP);
                int burpX, burpY;
                getPositionInThisDirection(getDirection(), 1, burpX, burpY);
                if (burpX >= 0 && burpX < 20 && burpY >= 0 && burpY < 20) {
                    getWorld()->addActor(new Burp(burpX, burpY, getDirection(), getWorld()));
                    m_burps--;
                }
            }
            break;
        default:
            break;
        }
    }
    
    


}


Kong::Kong(int startX, int startY, int direction, StudentWorld* world)
    : Actor(IID_KONG, startX, startY, direction, world), m_throwCounter(0), m_fleeTickCounter(0), m_fleeing(false)
{
    m_ticksBetweenBarrelThrows = max(200 - 50 * world->getLevel(), 50); 
}

bool Kong::isKong() {
    return true;
}

void Kong::doSomething() {
    if (!Alive()) {
        return;
    }
        

    increaseAnimationNumber();

    double dx = getWorld()->getPlayer()->getX() - getX();
    double dy = getWorld()->getPlayer()->getY() - getY();
    double distance = sqrt(dx * dx + dy * dy);

    if (distance <= 2) {
        m_fleeing = true;
    }

    if (!m_fleeing) {
        m_throwCounter++;
        if (m_throwCounter >= m_ticksBetweenBarrelThrows) {
            int barrelX, barrelY;
            getPositionInThisDirection(getDirection(), 1, barrelX, barrelY);
            getWorld()->addActor(new Barrel(barrelX, barrelY, getDirection(), getWorld()));
            m_throwCounter = 0;
        }
    }

    if (m_fleeing) {
        m_fleeTickCounter++;
        if (m_fleeTickCounter >= 5) {
            int newY = getY() + 1;
            if (newY > VIEW_HEIGHT) {
                getWorld()->increaseScore(1000);
                getWorld()->playSound(SOUND_FINISHED_LEVEL);
                setAlive(false);
            }
            else {
                moveTo(getX(), newY);
            }
            m_fleeTickCounter = 0;
        }
    }
}


Barrel::Barrel(int startX, int startY, int direction, StudentWorld* world)
    : Enemy(IID_BARREL, startX, startY, direction, world) {}

void Barrel::doSomething() {
    if (!Alive()) {
        return;
    }

    if (attackPlayerIfAtSamePosition()) {
        return;
    }

    // is Barrel is on a Bonfire
    for (Actor* actor : getWorld()->getActors()) {
        if (actor->getX() == getX() && actor->getY() == getY() && actor->Alive()) {
            if (getWorld()->isBonfireAt(getX(), getY())) {
                setAlive(false);
                return;
            }
        }
    }

    // Fall if no floor
    if (applyGravity()) {
        if (getWorld()->isFloorAt(getX(), getY() - 1)) {
            reverseDirection();
        }
        return;
    }

    // Move horizontally every 10 ticks 
    increaseTickCounter();
    if (getTickCounter() >= 10) {
        setTickCounter(0);
        int newX = (getDirection() == right) ? getX() + 1 : getX() - 1;
        if (getWorld()->isFloorAt(newX, getY())) {
            reverseDirection(); // Reverse if hitting a wall
        }
        else {
            moveTo(newX, getY()); // Move in current direction
            
            if (attackPlayerIfAtSamePosition()) {
                return;
            }
        }
    }
}

void Barrel::beAttacked() {
    die();
}

Fireball::Fireball(int startX, int startY, StudentWorld* world)
    : Enemy(IID_FIREBALL, startX, startY, randInt(0, 1) == 0 ? left : right, world), m_climbingState(0) {}

void Fireball::doSomething() {
    if (!Alive()) {
        return;
    }

    if (attackPlayerIfAtSamePosition()) {
        return;
    }

    increaseTickCounter();
    if (getTickCounter() < 10) {
        return;
    }
    setTickCounter(0);
     //climbing up
    if (getWorld()->isLadderAt(getX(), getY()) && !getWorld()->isFloorAt(getX(), getY() + 1) && m_climbingState != 2) {
        if (m_climbingState == 1 || randInt(1, 3) == 1) {
            moveTo(getX(), getY() + 1);
            m_climbingState = 1;
            return;
        }
    }
    //climb down
    if (getWorld()->isLadderAt(getX(), getY() - 1) && m_climbingState != 1) {
        if (m_climbingState == 2 || randInt(1, 3) == 1) {
            moveTo(getX(), getY() - 1);
            m_climbingState = 2;
            return;
        }
    }

    // stop climbing
    if ((m_climbingState == 1 && (!getWorld()->isLadderAt(getX(), getY()) || getWorld()->isFloorAt(getX(), getY() + 1))) || (m_climbingState == 2 && !getWorld()->isLadderAt(getX(), getY() - 1))) {
        m_climbingState = 0;
    }
    // horizontal movement
    int newX = (getDirection() == right) ? getX() + 1 : getX() - 1;
    // reverse direction
    if (getWorld()->isFloorAt(newX, getY()) || (!getWorld()->isFloorAt(newX, getY() - 1) && !getWorld()->isLadderAt(newX, getY() - 1))) {
        reverseDirection();
        return;
    }

    moveTo(newX, getY());

    if (attackPlayerIfAtSamePosition()) {
        return;
    }
}


void Fireball::beAttacked() {
    die();
    if (randInt(1, 3) == 1) { // 1/3 chance to drop GarlicGoodie
        getWorld()->addActor(new GarlicGoodie(getX(), getY(), getWorld()));
    }
}