#include "brain.h"
#include <cmath>

Brain::Brain(): last_move(4), has_A(false), last_stage(-1), got_B(false), door_found(false), door_h(0), door_w(0)
{
}

int Brain::getNextMove(GameState &gamestate)
{
    int px = -1;
    int py = -1;
    char dir = ' ';

    if (!player_direction(gamestate, px, py, dir)){
        last_move = 0;
        return 0;
    }

    if (gamestate.stage != last_stage){
        last_stage = gamestate.stage;
        has_A = false;
        got_B = false;
        door_found = false;
    }

    update_door_memory(gamestate, px, py);

    if (gamestate.stage == 1){
        int fx = -1;
        int fy = -1;

        if (find_food(gamestate, px, py, fx, fy)){
            int food_move = go_to_target(px, py, fx, fy, gamestate);
            if (food_move != 0){
                last_move = food_move;
                return food_move;
            }
        }
        else{
            int door_move = move_toward_right(gamestate);
            if (door_move != 0){
                last_move = door_move;
                return door_move;
            }
        }
    }

    int tx = -1;
    int ty = -1;

    if (!has_A && !got_B){
        if (find_target(gamestate, px, py, 'A', tx, ty)){
            int m = go_to_target(px, py, tx, ty, gamestate);

            if (m != 0){
                int nx = px;
                int ny = py;

                if (m == 1){
                    nx = px - 1;
                    ny = py;
                }
                else if (m == 2){
                    nx = px;
                    ny = py - 1;
                }
                else if (m == 3){
                    nx = px + 1;
                    ny = py;
                }
                else if (m == 4){
                    nx = px;
                    ny = py + 1;
                }

                char step_tile = safe_vision(nx, ny, gamestate);
                if (step_tile == 'A'){
                    has_A = true;
                }

                last_move = m;
                return m;
            }
        }
    }

    if (has_A && !got_B){
        if (find_target(gamestate, px, py, 'B', tx, ty)){
            int m = go_to_target(px, py, tx, ty, gamestate);
            if (m != 0){
                int nx = px;
                int ny = py;

                if (m == 1){
                    nx = px - 1;
                    ny = py;
                }
                else if (m == 2){
                    nx = px;
                    ny = py - 1;
                }
                else if (m == 3){
                    nx = px + 1;
                    ny = py;
                }
                else if (m == 4){
                    nx = px;
                    ny = py + 1;
                }

                char step_tile = safe_vision(nx, ny, gamestate);
                if (step_tile == 'B'){
                    got_B = true;
                    has_A = false;
                }

                last_move = m;
                return m;
            }
        }
    }

    if (got_B){
        int m = move_toward_door(gamestate);
        if (m != 0){
            last_move = m;
            return m;
        }
    }

    int right = 0;
    int forward = 0;
    int left = 0;
    int back = 0;

    if (dir == '^'){
        right = 4;
        forward = 1;
        left = 2;
        back = 3;
    }
    else if (dir == 'v'){
        right = 2;
        forward = 3;
        left = 4;
        back = 1;
    }
    else if (dir == '>'){
        right = 3;
        forward = 4;
        left = 1;
        back = 2;
    }
    else if (dir == '<'){
        right = 1;
        forward = 2;
        left = 3;
        back = 4;
    }

    if (!direction_of_obstacle(right, gamestate)){
        last_move = right;
        return right;
    }
    if (!direction_of_obstacle(forward, gamestate)){
        last_move = forward;
        return forward;
    }
    if (!direction_of_obstacle(left, gamestate)){
        last_move = left;
        return left;
    }
    if (!direction_of_obstacle(back, gamestate)){
        last_move = back;
        return back;
    }

    last_move = 0;
    return 0;
}

char Brain::safe_vision(int x, int y, const GameState &game_state) const{
    if (game_state.vision.empty()){
        return ' ';
    }

    if (x < 0 || x >= static_cast<int>(game_state.vision.size())){
        return ' ';
    }

    if (game_state.vision[x].empty() || y < 0 || y>= static_cast<int>(game_state.vision[x].size())){
        return ' ';
    }

    return game_state.vision[x][y];
}

bool Brain::is_walkable(char tile) const{
    if (tile == '+'){
        return false;
    }
    if (tile == 'D'){
        return false;
    }
    if (tile == 'X'){
        return false;
    }
    if (tile == 'T'){
        return false;
    }

    return true;
}

bool Brain::player_direction(const GameState &game_state, int &player_x, int &player_y, char &direction) const{
    for (int x = 0; x < static_cast<int>(game_state.vision.size()); ++x){
        const auto &row = game_state.vision[x];
        for (int y = 0; y < static_cast<int>(row.size()); ++y){
            char ch = row[y];
            if (ch =='^' || ch =='v' || ch =='>' || ch == '<'){
                player_x = x;
                player_y = y;
                direction = ch;
                return true;
            }
        }
    }
    return false;
}

char Brain::direction_of_tile(int move, const GameState &game_state) const{
    int px = -1;
    int py = -1;
    char direct = ' ';

    if (!player_direction(game_state, px, py, direct)){
        return ' ';
    }
    int a = px;
    int b = py;

    if (move == 1){
        a = px - 1;
        b = py;
    }else if(move == 2){
        a = px;
        b = py - 1;
    }else if(move == 3){
        a = px + 1;
        b = py;
    } else if (move== 4){
        a = px;
        b = py+1;
    } else{
        a = px;
        b = py;
    }
    return safe_vision(a, b, game_state);
}

bool Brain::direction_of_obstacle(int move, const GameState &game_state) const{
    if (move == 0){
        return false;
    }
    char tile = direction_of_tile(move, game_state);
    return !is_walkable(tile);
}

int Brain::opposite_move(int move) const{
    if (move ==1){
        return 3;
    }

    if (move == 3){
        return 1;
    }
    if (move == 2){
        return 4;
    }

    if (move == 4){
        return 2;
    }
    return 0;
}

bool Brain::find_food(const GameState &game_state, int px, int py, int &food_x, int &food_y) const{
    bool found = false;
    int best_distance = 1000000;

    for (int x = 0; x <static_cast<int>(game_state.vision.size()); ++x){
        const auto &row = game_state.vision[x];
        for (int y =0; y < static_cast<int>(row.size()); ++y){
            if (row[y]== '0'){
                int dist = std::abs(x-px) + std::abs(y-py);
                if (!found || dist < best_distance){
                    found = true;
                    best_distance = dist;
                    food_x = x;
                    food_y = y;
                }
            }
        }
    }
    return found;
}

int Brain::go_to_target(int px, int py, int tx, int ty, const GameState &game_state) const{
    int dx = ty - py;
    int dy = tx - px;
    int primary = 0;
    int secondary = 0;

    if (std::abs(dx) >= std::abs(dy)){
        if (dx > 0){
            primary = 4;
        } else if (dx < 0){
            primary = 2;
        }
        if (dy > 0){
            secondary = 3;
        } else if (dy < 0){
            secondary = 1;
        }
    } else{
        if (dy > 0){
            primary = 3;
        } else if (dy < 0){
            primary = 1;
        }
        if (dx > 0){
            secondary = 4;
        } else if (dx < 0){
            secondary = 2;
        }
    }
    if (primary != 0 && !direction_of_obstacle(primary, game_state)){
        return primary;
    }
    if (secondary != 0 && !direction_of_obstacle(secondary, game_state)){
        return secondary;
    }
    return 0;
}

int Brain::move_toward_right(const GameState & game_state) const{
    int w = game_state.pos[1];
    int best_move = 0;
    int best_score = -1000000;
    int moves[4] = {4, 3, 1, 2};

    for (int i = 0; i < 4; ++i){
        int m = moves[i];
        if (direction_of_obstacle(m, game_state)){
            continue;
        }

        int new_w = w;
        if (m==4){
            new_w = w+1;
        } else if( m == 2){
            new_w = w-1;
        }
        int score = new_w;
        if (score > best_score){
            best_score = score;
            best_move = m;
        }
    }
    return best_move;
}

bool Brain::find_target(const GameState &game_state, int px, int py, char target, int &tx, int &ty) const{
    bool found = false;
    int best_distance = 1000000;
    for (int x = 0; x < static_cast<int>(game_state.vision.size()); ++x){
        const auto &row = game_state.vision[x];
        for (int y = 0; y < static_cast<int>(row.size()); ++y){
            if (row[y]== target){
                int distance = std::abs(x-px) + std::abs(y-py);
                if (!found || distance < best_distance){
                    found = true;
                    best_distance = distance;
                    tx = x;
                    ty = y;
                }
            }
        }
    }
    return found;
}

void Brain::update_door_memory(const GameState &game_state, int px, int py){
    for (int x = 0; x < static_cast<int>(game_state.vision.size()); ++x){
        const auto &row = game_state.vision[x];
        for (int y = 0; y < static_cast<int>(row.size()); ++y){
            if (row[y] == 'D'){
                int global_h = game_state.pos[0] + (x - px);
                int global_w = game_state.pos[1] + (y - py);
                door_h = global_h;
                door_w = global_w;
                door_found = true;
                return;
            }
        }
    }
}

int Brain::move_toward_door(const GameState &game_state) const{
    if (!door_found){
        return 0;
    }
    int ph = game_state.pos[0];
    int pw = game_state.pos[1];

    int dx = door_w - pw;
    int dy = door_h - ph;

    int primary = 0;
    int secondary = 0;

    if (std::abs(dx) >= std::abs(dy)){
        if (dx > 0){
            primary = 4;
        }
        else if (dx < 0){
            primary = 2;
        }

        if (dy > 0){
            secondary = 3;
        }
        else if (dy < 0){
            secondary = 1;
        }
    }
    else{
        if (dy > 0){
            primary = 3;
        }
        else if (dy < 0){
            primary = 1;
        }

        if (dx > 0){
            secondary = 4;
        }
        else if (dx < 0){
            secondary = 2;
        }
    }

    if (primary != 0 && !direction_of_obstacle(primary, game_state)){
        return primary;
    }
    if (secondary != 0 && !direction_of_obstacle(secondary, game_state)){
        return secondary;
    }

    return 0;
}
