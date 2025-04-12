#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "Level.h"
#include <string>
#include <vector>
#include "Actor.h"
// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp
class Actor;
class Player;

class StudentWorld : public GameWorld
{
	public:
	  StudentWorld(std::string assetPath);
	  virtual int init();
	  virtual int move();
	  virtual void cleanUp();
	  bool isFloorAt(int x, int y) const;
	  bool isLadderAt(int x, int y) const;
	  bool isPlayerAt(int x, int y) const;
	  bool isBonfireAt(int x, int y) const;
	  Level::MazeEntry getLevelContents(int x, int y) const;
	  ~StudentWorld();

	  Player* getPlayer() const;
	  std::vector<Actor*> getActors() const;
	  void addActor(Actor* actor);

	private:
		std::vector<Actor*> m_actors;
		Player* m_player;
		
};

#endif // STUDENTWORLD_H_
