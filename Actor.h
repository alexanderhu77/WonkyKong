#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"

// Students:  Add code to this file, Actor.cpp, StudentWorld.h, and StudentWorld.cpp
class StudentWorld;
//   base class for all of of the game's actors

class Actor : public GraphObject {
    public:
        Actor(int imageID, int startX, int startY, int startDirection, StudentWorld* world);
        virtual void doSomething() = 0;
        bool Alive();
        void setAlive(bool alive);
        virtual void beAttacked();
        virtual bool isKong();
        StudentWorld* getWorld() const;
        bool applyGravity();

    private:
        bool m_alive;
        StudentWorld* m_world;
};

class Enemy : public Actor {
    public:
        Enemy(int imageID, int startX, int startY, int startDirection, StudentWorld* world);
        virtual void doSomething() = 0;
        virtual void beAttacked() = 0;
        void reverseDirection();
        int getTickCounter() const;
        void increaseTickCounter();
        void setTickCounter(int m);
        void attackPlayer() const;
        bool attackPlayerIfAtSamePosition();
        void die();

    private:
        int m_tickCounter;
};

class Floor : public Actor {
    public:
        Floor(int startX, int startY, StudentWorld* world);
        virtual void doSomething();
};

class Ladder : public Actor {
    public:
        Ladder(int startX, int startY, StudentWorld* world);
        virtual void doSomething();
};


class Goodie : public Actor {
public:
    Goodie(int imageID, int startX, int startY, StudentWorld* world);
    virtual void doSomething();
    virtual int getScoreValue() const = 0;
    virtual void conferPowerUp() = 0;
};

class ExtraLifeGoodie : public Goodie {
public:
    ExtraLifeGoodie(int startX, int startY, StudentWorld* world);
    virtual int getScoreValue() const;
    virtual void conferPowerUp();
};

class GarlicGoodie : public Goodie {
public:
    GarlicGoodie(int startX, int startY, StudentWorld* world);
    virtual int getScoreValue() const;
    virtual void conferPowerUp();
};


class Burp : public Actor {
    public:
        Burp(int startX, int startY, int direction, StudentWorld* world);
        virtual void doSomething();
    private:
        int m_lifetime;
};

class Player : public Actor {
    public:
        Player(int startX, int startY, StudentWorld* world, int burps);
        virtual void doSomething();
        void addBurps(int n);
        void freeze(int ticks);
        int getBurps() const;
        virtual void beAttacked();
    private:
        int m_burps;
        int m_jumpTicks;
        int m_freezeTicks;
};

class Bonfire : public Actor {
    public:
        Bonfire(int x, int y, StudentWorld* world);
        virtual void doSomething();
};

class Koopa : public Enemy {
    public:
        Koopa(int startX, int startY, StudentWorld* world);
        virtual void doSomething();
        virtual void beAttacked();
    private: 
        int m_freezeCooldown; // ticks until Koopa can freeze Player again
};

class Kong : public Actor {
    public:
        Kong(int startX, int startY, int direction, StudentWorld* world);
        virtual bool isKong();
        virtual void doSomething();
    private:
        int m_throwCounter; // Counts ticks for throwing barrels
        int m_fleeTickCounter;
        bool m_fleeing;
        int m_ticksBetweenBarrelThrows; // named N in spec
};

class Barrel : public Enemy {
    public:
        Barrel(int startX, int startY, int direction, StudentWorld* world);
        virtual void doSomething();
        virtual void beAttacked();
};

class Fireball : public Enemy {
    public:
        Fireball(int startX, int startY, StudentWorld* world);
        virtual void doSomething();
        virtual void beAttacked();
    private:
        int m_climbingState; // 0 = not climbing, 1 = climbing up, 2 = climbing down
};

#endif // ACTOR_H_