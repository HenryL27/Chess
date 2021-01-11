#include "Board.hh"
#include "Engine.hh"

void play_game(Board& board, Engine& e1, Engine& e2) {
    board.reset();
    Move m{"",""};
    while( !board.game_over() ){
        board.play_move(e1.get_move(m));
        if(board.game_over()){
            break;
        }
        board.play_move(e2.get_move(m));
    }
    e1.process_result(board.white_wins(), board.black_wins());
    e2.process_result(board.black_wins(), board.white_wins());
}

int main(int args, char** argv) {
    Board board{};
    Engine e1{board, true};
    Engine e2{board, false};
    play_game(board, e1, e2);
    return 0;
}

