
#include "state_machine.hh"

State_Machine::State_Machine(eval_fn e, bool t) :
    board(std::make_unique<Board>()), eval(e), past_states(std::vector<board_array>(0)), turn(t)
{
    past_states.push_back(board->get_board());
}

State_Machine::State_Machine(const board_array &b, eval_fn e=null_eval, bool t) :
    board(std::make_unique<Board>(b)), eval(e), past_states(std::vector<board_array>(0)), turn(t)
{
    past_states.push_back(board->get_board());
}

State_Machine::~State_Machine()
{}

double State_Machine::evaluate(){
    if (is_terminal()) {
        if (board->white_wins()){
            return 100;
        }
        if (board->black_wins()){
            return -100;
        }
        return 0;
    }
    return eval(*board);
}

std::vector<Move> State_Machine::get_actions() {
    return board->get_legal_moves(turn);
}

void State_Machine::perform_action(const Move &m){
    board->play_move(m);
    past_states.push_back(board->get_board());
}

board_array State_Machine::get_board() {
    return board->get_board();
}

void State_Machine::undo(){
    past_states.pop_back();
    board->set_board(past_states.back());    
}

void State_Machine::set_state(const board_array &b){
    board->set_board(b);
    while(past_states.size() > 0){
        past_states.pop_back();
    }
    past_states.push_back(board->get_board());
}

bool State_Machine::is_terminal() {
    return board->game_over()
}

