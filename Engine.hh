#include <string>
#include <functional>
#include <memory>
#include <array>

#pragma once

#include "Board.hh"
#include "evaluation.hh"

class Engine {
    protected:
        Board &board;
        bool white;
    public:
        Engine(Board& board, bool amWhite);
        ~Engine();

        // Disallow engine copies, to simplify memory management.
        Engine(const Engine&) = delete;
        Engine& operator=(const Engine&) = delete;

        virtual Move get_move(Move opp_move)=0;
        virtual double evaluate()=0;
        virtual void process_result(const bool I_win, const bool opponent_wins)=0;
};