#include <memory>
#include <string>
#include <vector>
#include <array>

#pragma once

struct Move {
    std::string start;
    std::string end;
};

typedef std::array<char, 64> board_array;

const uint to_index(const std::string square);

class Board{
    private:
        class Impl;
        std::unique_ptr<Impl> I;
    public:
        Board();
        Board(const board_array &b);
        ~Board();

        // Disallow cache copies, to simplify memory management.
        Board(const Board&) = delete;
        Board& operator=(const Board&) = delete;

        void play_move(Move mv);
        std::vector<Move> get_past_moves() const;
        board_array get_board() const;
        std::vector<Move> get_legal_moves(const bool amWhite) const;
        std::vector<Move> get_moves(const bool am_white) const;
        std::string to_string() const;
        std::string move_string() const;
        bool game_over() const;
        bool white_wins() const;
        bool black_wins() const;
        bool is_white_turn() const;
        char get_square(std::string square) const;
        void set_board(const board_array &b);
        void reset();
};