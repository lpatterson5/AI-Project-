  #ifndef BRAIN_H
  #define BRAIN_H

  #include <string>
  #include "../Game/game.h"
  #include <cstdlib>
  #include <ctime>
	#include <vector>
	#include <iostream>
	#include <cmath> 

class Brain
{
	private: 
		int last_move;
		bool has_A;
		int last_stage;
		bool got_B;
		bool door_found; 
		int door_h;
		int door_w; 
		
		char safe_vision(int x, int y, const GameState &game_state) const; 
		bool is_walkable(char tile) const; 
		bool player_direction(const GameState &gs, int &player_x, int &player_y, char &direction) const;
		char direction_of_tile(int move, const GameState &game_state ) const;
		bool direction_of_obstacle(int move, const GameState &game_state) const; 
		int opposite_move(int move) const;
		bool find_food(const GameState &game_state, int px, int py, int &food_x, int &food_y) const; 
		int go_to_target(int px, int py, int tx, int ty, const GameState &game_state) const; 
		int move_toward_right(const GameState & game_state) const; 
		bool find_target(const GameState &game_state, int px, int py, char target, int &tx, int &ty) const; 
		void update_door_memory(const GameState &game_state, int px, int py);
		int move_toward_door(const GameState & game_state)const; 
  public:
  	Brain();                               
  	int getNextMove(GameState &gamestate); 
};

  #endif 