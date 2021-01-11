#include "Board.hh"
#include "evaluation.hh"

class State_Machine {
    private:
        std::unique_ptr<Board> board;
        eval_fn eval;
        std::vector<board_array> past_states;
        bool turn;

    public:
        State_Machine(eval_fn e=null_eval, bool t=true);
        State_Machine(const board_array &b, eval_fn e=null_eval, bool t=true);
        ~State_Machine();

        // Disallow cache copies, to simplify memory management.
        State_Machine(const State_Machine&) = delete;
        State_Machine& operator=(const State_Machine&) = delete;

        // What does the eval function return in this state
        double evaluate();

        // What are my possible moves
        std::vector<Move> get_actions();

        // execute some move
        void perform_action(const Move &m);

        // What staate am I in now
        board_array get_board();

        // Undo the last move, so I can check other moves
        void undo();

        // Set the current state to something
        void set_state(const board_array &b);

        // returns whether this is a final state
        bool is_terminal();
};

