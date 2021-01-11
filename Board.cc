#include "Board.hh"

#include <cassert>
#include <algorithm>
#include <iterator>
#include <iostream>

const board_array START_BOARD = {
    'R','N','B','Q','K','B','N','R',
    'P','P','P','P','P','P','P','P',
    ' ',' ',' ',' ',' ',' ',' ',' ',
    ' ',' ',' ',' ',' ',' ',' ',' ',
    ' ',' ',' ',' ',' ',' ',' ',' ',
    ' ',' ',' ',' ',' ',' ',' ',' ',
    'p','p','p','p','p','p','p','p',
    'r','n','b','q','k','b','n','r'
};
const std::string FILES = "abcdefgh";
enum PLAYER {WHITE = true, BLACK = false};

class Board::Impl {
    private:
        bool square_attacked(std::vector<Move> &opp_moves, std::string sqaure) const; // Check
        bool is_legal_move(const Move m);                                   // Check
        bool is_occupied(std::string square, const bool by_white) const;    // Check
        bool is_occupied(std::string square) const;                         // Check
        bool am_in_check(const bool am_white) const;                        // Check
        std::vector<Move> knight_moves(std::string square) const;           // Check
        std::vector<Move> pawn_moves(std::string square) const;             // Check
        std::vector<Move> bishop_moves(std::string square) const;           // Check
        std::vector<Move> rook_moves(std::string square) const;             // Check
        std::vector<Move> queen_moves(std::string square) const;            // Check
        std::vector<Move> king_moves(std::string square) const;             // Check
        std::vector<Move> castleing(const bool am_white) const;             // Check
        void execute_move(const Move m, char *capt_piece, char *mov_piece); // Check
        bool threefold_rep(const std::string last) const;                   // Check
    public:
        board_array b;
        bool turn;
        std::vector<Move> moves;
        std::string augmoves;
        bool white_castle[3];  ///   A Rook has moved, King has moved, H Rook has moved
        bool black_castle[3];
        std::string white_king;
        std::string black_king;
        std::vector<std::string> past_states;

        Impl();                                                             // Check
        ~Impl();                                                            // Check
        void play_move(Move mv);                                            // Check
        std::vector<Move> get_past_moves() const;                           // Check
        board_array get_board() const;                                      // Check
        std::vector<Move> get_legal_moves(const bool amWhite);              // Check
        std::vector<Move> get_moves(const bool am_white) const;             // Check
        std::string to_string() const;                                      // Check
        std::string move_string() const;                                    // Check
        bool game_over();                                                   // Check
        bool white_wins();                                                  // Check
        bool black_wins();                                                  // Check
        bool is_white_turn() const;                                         // Check
        char get_square(std::string square) const;                          // Check
        void set_board(const board_array &b);   
};

// converts a string like "f6" into a board_array index like 47 (or whatever that would be)
const uint to_index(const std::string square){
    assert(square.size() >= 2);
    return (square[0] - 'a') + (square[1] - '1') * 8;
}

// converts an index to the board array like 47 into a string like "f6"
const std::string to_square(const uint index) {
    int file = index % 8;
    int rank = (index / 8) % 8;
    char a = file + 'a';
    char b = rank + '1';
    //std::cout << file << rank << std::string({a,b}) << std::endl;
    return std::move(std::string({a, b}));
}

// converts a pair of coords like (0,7) into a string like "a8"
const std::string to_square(const std::pair<int, int> &p){
    char file = p.first + 'a';
    char rank = p.second + '1';
    //std::cout << std::string({file, rank}) << std::endl;
    return std::move(std::string({file, rank}));
}

// same as above, but they aren't a tuple
const std::string to_square(const uint file, const uint rank){
    char a = file + 'a';
    char b = rank + '1';
    //std::cout << std::string({a,b}) << std::endl;
    return std::move(std::string({a, b}));
}

// constructor. all boards start out the same. unless this is like 
// chess 960 or something and we don't worry about that
Board::Impl::Impl() :
    b(START_BOARD), turn{true}, moves(std::vector<Move>()),
    white_castle{false, false, false}, black_castle{false, false, false},
    white_king{"e1"}, black_king{"e8"}, augmoves{""}, 
    past_states{std::string(START_BOARD.begin(), START_BOARD.end())}
{}

Board::Impl::~Impl(){}

// Checks whether a certain square is occupied by a certain side
bool Board::Impl::is_occupied(std::string square, const bool am_white) const {
    char me = b[to_index(square)];
    if (me == ' ') {
        return false;
    }
    return (me < 'a') == am_white;
}

// Checks wheter there is anything in the square at all
bool Board::Impl::is_occupied(std::string square) const {
    char me = b[to_index(square)];
    //std::cout << "is_occupied(" << square << "): " << (me != ' ') << " char is \'" << me << "\'" << std::endl;
    return me != ' ';
}

// What piece is on this square?
char Board::Impl::get_square(std::string square) const {
    return b[to_index(square)];
}

// Calculates all possible knight moves. Figure out what they could be and then 
// eliminate the bad ones.
std::vector<Move> Board::Impl::knight_moves(std::string square) const {
    short file = square[0] - 'a';
    short rank = square[1] - '1';
    char me = b[to_index(square)];
    std::vector<Move> ans(0);
    std::vector<std::pair<int, int>> pos(0);
    pos.push_back(std::make_pair(file - 2, rank - 1));
    pos.push_back(std::make_pair(file - 2, rank + 1));
    pos.push_back(std::make_pair(file + 2, rank - 1));
    pos.push_back(std::make_pair(file + 2, rank + 1));
    pos.push_back(std::make_pair(file - 1, rank - 2));
    pos.push_back(std::make_pair(file - 1, rank + 2));
    pos.push_back(std::make_pair(file + 1, rank - 2));
    pos.push_back(std::make_pair(file + 1, rank + 2));
    for (auto const &p : pos) {
        if (p.first >= 0 && p.first < 8 && p.second >= 0 && p.second < 8) {
            std::string sq = to_square(p);
            if (!is_occupied(sq, (me < 'a'))) {          /// Only add the square if it is not occupied by one of my teammates
                ans.push_back({square, sq});
            }
        }
    }
    return ans;
}

// Caculate the pawn moves
std::vector<Move> Board::Impl::pawn_moves(std::string square) const {
    std::vector<std::string> ans(0);
    short file = square[0] - 'a';
    short rank = square[1] - '1';
    char me = b[to_index(square)];
    // black pawns first
    if (me == 'p') {
        // Move one square forward, needs empty space
        if (! is_occupied(to_square(file, rank - 1))) {
            if ( rank > 1 ){
                ans.push_back(to_square(file, rank - 1));
                // Move 2 squares forward iff empty spaces and going from x7 to x5
                if ( (rank == 6) && (!is_occupied(to_square(file, 4)))) {
                        ans.push_back(to_square(file, 4));
                }
            } 
        } 
        // capturing left
        if (file > 0) {
            // standard capture
            if (is_occupied(to_square(file - 1, rank - 1), true)) {
                ans.push_back(to_square(file - 1, rank - 1));
            }
            // en passant
            if (get_square(to_square(file - 1, 3)) == 'P' && rank == 3 &&
                moves.back().start == to_square(file-1, 1) &&
                moves.back().end == to_square(file-1, 3)) {
                    ans.push_back(to_square(file - 1, 2));
            }
        }
        // cpaturing right
        if (file < 7) {
            // standard capture
            if (is_occupied(to_square(file + 1, rank - 1), true)) {
                ans.push_back(to_square(file + 1, rank - 1));
            }
            // en passant
            if (get_square(to_square(file + 1, 3)) == 'P' && rank == 3 &&
                moves.back().start == to_square(file + 1, 1) &&
                moves.back().end == to_square(file + 1, 3)) {
                    ans.push_back(to_square(file + 1, 2));
            }
        }
        // Promoting : for each move, replace with promoting to each piece
        if (rank == 1) {
            uint num_items = ans.size();
            for(uint i = 0; i < num_items; ++i){
                std::string m = ans[0];
                ans.erase(ans.begin());
                ans.push_back(m + "=Q");
                ans.push_back(m + "=R");
                ans.push_back(m + "=B");
                ans.push_back(m + "=N");
            }
        }
    } else if (me == 'P') {
        if (! is_occupied(to_square(file, rank + 1))) {
            if ( rank >= 1 ){
                ans.push_back(to_square(file, rank + 1));
                if ( (rank == 1) && (!is_occupied(to_square(file, 3)))) {
                        ans.push_back(to_square(file, 3));
                }
            } 
        }
        // capturing left
        if (file > 0) {
            // standard capture
            if (is_occupied(to_square(file - 1, rank + 1), false)) {
                ans.push_back(to_square(file - 1, rank + 1));
            }
            // en passant
            if (get_square(to_square(file - 1, 4)) == 'p' && rank == 4 &&
                moves.back().start == to_square(file - 1, 6) &&
                moves.back().end == to_square(file - 1, 4)) {
                    ans.push_back(to_square(file - 1, 5));
            }
        }
        // cpaturing right
        if (file < 7) {
            // standard capture
            if (is_occupied(to_square(file + 1, rank + 1), false)) {
                ans.push_back(to_square(file + 1, rank + 1));
            }
            // en passant
            if (get_square(to_square(file + 1, 4)) == 'p' && rank == 4 &&
                moves.back().start == to_square(file + 1, 6) &&
                moves.back().end == to_square(file + 1, 4)) {
                    ans.push_back(to_square(file + 1, 5));
            }
        }
        if (rank == 6) {
            uint num_items = ans.size();
            for(uint i = 0; i < num_items; ++i){
                std::string m = ans[0];
                ans.erase(ans.begin());
                ans.push_back(m + "=Q");
                ans.push_back(m + "=R");
                ans.push_back(m + "=B");
                ans.push_back(m + "=N");
            }
        }
    }
    std::vector<Move> mvs(0);
    for (const auto &dest : ans) {
        mvs.push_back({square, dest});
    }
    return std::move(mvs);
}

// calculate bishop moves: from current position, travel out on the diagonals until
// hit a wall or another piece (which I can maybe capture)
std::vector<Move> Board::Impl::bishop_moves( std::string square ) const {
    auto me = get_square(square);
    bool player = me < 'a';
    short file = square[0] - 'a';
    short rank = square[1] - '1';
    std::vector<Move> ans (0);
    // down, left
    short f = file; short r = rank;
    while(f >= 1 && f < 8 && r >= 1 && r < 8){
        f--; r--;
        if (is_occupied(to_square(f, r), !player )) {
            ans.push_back({square, to_square(f,r)});
            break;
        } if (is_occupied(to_square(f,r), player )) {
            break;
        }
        ans.push_back({square, to_square(f,r)});
    }
    // down, right
    f = file; r = rank;
    while(f >= 0 && f < 7 && r >= 1 && r < 8){
        f++; r--;
        if (is_occupied(to_square(f, r), !player )) {
            ans.push_back({square, to_square(f,r)});
            break;
        } if (is_occupied(to_square(f,r), player )) {
            break;
        }
        ans.push_back({square, to_square(f,r)});
    }
    // up, left
    f = file; r = rank;
    while(f >= 1 && f < 8 && r >= 0 && r < 7){
        f--; r++;
        if (is_occupied(to_square(f, r), !player )) {
            ans.push_back({square, to_square(f,r)});
            break;
        } if (is_occupied(to_square(f,r), player )) {
            break;
        }
        ans.push_back({square, to_square(f,r)});
    }
    // up, right
    f = file; r = rank;
    while(f >= 0 && f < 7 && r >= 0 && r < 7){
        f++; r++;
        if (is_occupied(to_square(f, r), !player )) {
            ans.push_back({square, to_square(f,r)});
            break;
        } if (is_occupied(to_square(f,r), player )) {
            break;
        }
        ans.push_back({square, to_square(f,r)});
    }
    return ans;
}

// calculate rook moves, similar to bishoping
std::vector<Move> Board::Impl::rook_moves(std::string square) const {
    auto me = get_square(square);
    bool player = me < 'a';
    short file = square[0] - 'a';
    short rank = square[1] - '1';
    std::vector<Move> ans (0);
    // left
    short f = file; short r = rank;
    while (f > 0) {
        f--;
        if (is_occupied(to_square(f,r), player)) {
            break;
        }
        if (is_occupied(to_square(f,r), !player)) {
            ans.push_back({square, to_square(f,r)});
            break;
        }
        ans.push_back({square, to_square(f,r)});
    }
    // right
    f = file; r = rank;
    while (f < 7) {
        f++;
        if (is_occupied(to_square(f,r), player)) {
            break;
        }
        if (is_occupied(to_square(f,r), !player)) {
            ans.push_back({square, to_square(f,r)});
            break;
        }
        ans.push_back({square, to_square(f,r)});
    }
    // down
    f = file; r = rank;
    while (r > 0) {
        r--;
        if (is_occupied(to_square(f,r), player)) {
            break;
        }
        if (is_occupied(to_square(f,r), !player)) {
            ans.push_back({square, to_square(f,r)});
            break;
        }
        ans.push_back({square, to_square(f,r)});
    }
    // up
    f = file; r = rank;
    while (r < 7) {
        r++;
        if (is_occupied(to_square(f,r), player)) {
            break;
        }
        if (is_occupied(to_square(f,r), !player)) {
            ans.push_back({square, to_square(f,r)});
            break;
        }
        ans.push_back({square, to_square(f,r)});
    }
    return ans;
}

// Queen: Rook moves + Bishop moves
std::vector<Move> Board::Impl::queen_moves(std::string square) const {
    auto v1 = bishop_moves(square);
    auto v2 = rook_moves(square);
    v1.insert(v1.end(), v2.begin(), v2.end());
    return v1;
}

// King: one square in any direction, no castling calculated here
std::vector<Move> Board::Impl::king_moves(std::string square) const {
    char me = get_square(square);
    bool player = me < 'a';
    short file = square[0] - 'a';
    short rank = square[1] - '1';
    std::vector<std::pair<int, int>> coords({
        std::make_pair(file + 1, rank + 1),
        std::make_pair(file    , rank + 1),
        std::make_pair(file - 1, rank + 1),
        std::make_pair(file + 1, rank    ),
        std::make_pair(file - 1, rank    ),
        std::make_pair(file + 1, rank - 1),
        std::make_pair(file    , rank - 1),
        std::make_pair(file - 1, rank - 1)
    });
    std::vector<Move> ans(0);
    for (const std::pair<int, int> &p : coords) {
        if (
            p.first < 8 && p.first >= 0 &&
            p.second < 8 && p.second >= 0 &&
            ! is_occupied(to_square(p), player)
        ) {
            ans.push_back({square, to_square(p)});
        }
    }
    return ans;
}

// Given that the opponent can make the moves in $moves, can they
// move anything to $square
bool Board::Impl::square_attacked(std::vector<Move> &moves, std::string square) const {
    for (const Move &mv : moves) {
        if (mv.end.rfind(square, 0) != -1) {
            if (get_square(mv.start) % 32 != 'P' % 32 || mv.start[0] != square[0]) {
                return true;
            }
        }
    }
    return false;
}

// Castling.... Ooo boy.
std::vector<Move> Board::Impl::castleing(bool am_white) const {
    std::vector<Move> ans(0);
    std::vector<Move> opp_moves = get_moves(!am_white);
    if (am_white){
        /// No castling if the king has moved, no castling out of check
        if (white_castle[1] || square_attacked(opp_moves, "e1")) {
            return ans;
        }
        /// Queenside Castle
        /// None of the squares are occupied by anyone, and no castling into or through check
        if (! white_castle[0] &&
            ! is_occupied("b1") && 
            ! is_occupied("c1") && !square_attacked(opp_moves, "c1") &&
            ! is_occupied("d1") && !square_attacked(opp_moves, "d1") &&
            ! (get_square("e2") == 'p') && ! (get_square("b2") == 'p')) {
                ans.push_back({"e1","c1"});
        }
        /// Kingside Castle
        /// Same dealio
        if (! white_castle[2] &&
            ! is_occupied("f1") && !square_attacked(opp_moves, "f1") &&
            ! is_occupied("g1") && !square_attacked(opp_moves, "g1") &&
            ! (get_square("e2") == 'p') && ! (get_square("h2") == 'p')) {
                ans.push_back({"e1","g1"});
        }
    } else {
        /// No castling if the king has moved, no castling out of check
        if (black_castle[1] || square_attacked(opp_moves, "e8")) {
            return ans;
        }
        /// Queenside Castle
        /// None of the squares are occupied by anyone, and no castling into or through check
        if (! black_castle[0] &&
            ! is_occupied("b8") && 
            ! is_occupied("c8") && !square_attacked(opp_moves, "c8") &&
            ! is_occupied("d8") && !square_attacked(opp_moves, "d8") &&
            ! (get_square("e7") == 'P') && ! (get_square("b7") == 'P')) {
                ans.push_back({"e8","c8"});
        }
        /// Kingside Castle
        /// Same dealio
        if (! black_castle[2] &&
            ! is_occupied("f8") && !square_attacked(opp_moves, "f8") &&
            ! is_occupied("g8") && !square_attacked(opp_moves, "g8") &&
            ! (get_square("e7") == 'P') && ! (get_square("h7") == 'P')) {
                ans.push_back({"e8","g8"});
        }
    }
    return ans;
}

/// Go through each square on the board. If it is one of my pieces, figure out where
/// it can go. Don't worry about whether the move is legal from a checking perspective.
/// Also don't compute castling. This method is mainly used to determing whether there
/// are checks on the board, so castling will never matter
std::vector<Move> Board::Impl::get_moves(const bool am_white) const {
    std::vector<Move> ans(0);
    for(uint i = 0; i < 64; ++i) {
        char piece = b[i];
        if ((piece < 'a') == am_white) {
            if (piece % 32 == 'P' % 32) {
                auto v = pawn_moves(to_square(i));
                ans.insert(ans.end(), v.begin(), v.end());
            } else if (piece % 32 == 'B' % 32) {
                auto v = bishop_moves(to_square(i));
                ans.insert(ans.end(), v.begin(), v.end());
            } else if (piece % 32 == 'N' % 32) {
                auto v = knight_moves(to_square(i));
                ans.insert(ans.end(), v.begin(), v.end());
            } else if (piece % 32 == 'R' % 32) {
                auto v = rook_moves(to_square(i));
                ans.insert(ans.end(), v.begin(), v.end());
            } else if (piece % 32 == 'Q' % 32) {
                auto v = queen_moves(to_square(i));
                ans.insert(ans.end(), v.begin(), v.end());
            } else if (piece % 32 == 'K' % 32) {
                auto v = king_moves(to_square(i));
                ans.insert(ans.end(), v.begin(), v.end());
            }
        }
    }
    return ans;
}

/// See what my opponent's moves are. If any of them capture the king, I'm in check
bool Board::Impl::am_in_check(const bool am_white) const {
    auto opp_moves = get_moves(! am_white);
    if (am_white) {
        return square_attacked(opp_moves, white_king);
    } else {
        return square_attacked(opp_moves, black_king);
    }
}

/// Make a move, recording what (if any) piece was captured, also what piece was moved
void Board::Impl::execute_move(const Move m, char *capt_piece, char *mov_piece) {
    *capt_piece = get_square(m.end);
    *mov_piece = get_square(m.start);
    b[to_index(m.start)] = ' ';
    if (m.end.size() != 2) {
        b[to_index(m.end)] = m.end[3];
    } else {
        b[to_index(m.end)] = *mov_piece;
    }
    // en passant makes everything more complicated
    if ((*mov_piece == 'p' || *mov_piece == 'P') &&
        (m.start[0] != m.end[0]) && (*capt_piece == ' ')) {
        *capt_piece = '!';
        if( *mov_piece == 'p'){
            b[to_index(std::string({m.end[0], '4'}))] = ' ';
        } else {
            b[to_index(std::string({m.end[0], '5'}))] = ' ';
        }
    }
}

/// Test out a move with execute_move, then see if I'm in check.
/// Then put the board back together. Use to see if a given move is 
/// legal, i.e. if performing it leads to the opponent capturing the king.
/// Again, castling is computed separately, and already checks for the various legalities
bool Board::Impl::is_legal_move(const Move m) {
    char capt_piece;
    char mov_piece;
    execute_move(m, &capt_piece, &mov_piece);
    bool ans;
    if (mov_piece == 'k' || mov_piece == 'K'){
        auto opp_moves = get_moves(mov_piece > 'a');
        ans = square_attacked(opp_moves, m.end);
    } else {
        ans = am_in_check(mov_piece < 'a');
    }
    b[to_index(m.start)] = mov_piece;
    // Again en passant makes everything more complicated
    if (capt_piece != '!') {
        b[to_index(m.end)] = capt_piece;
    } else {
        b[to_index(m.end)] = ' ';
        if( mov_piece == 'p'){
            b[to_index(std::string({m.end[0], '4'}))] = 'P';
        } else {
            b[to_index(std::string({m.end[0], '5'}))] = 'p';
        }
    }
    return !ans;
}

/// Returns the move history of the game
std::vector<Move> Board::Impl::get_past_moves() const {
    return moves;
}

/// Returns the board in array form
board_array Board::Impl::get_board() const {
    return b;
}

/// Returns all legal moves in the position
std::vector<Move> Board::Impl::get_legal_moves(const bool am_white) {
    
    auto v = get_moves(am_white);
    std::vector<Move> ans(0);
    for(uint i = 0; i < v.size(); ++i) {  
        if (is_legal_move(v[i])) {
            ans.push_back({v[i].start,v[i].end});
        }
    }
    auto castles = castleing(am_white);
    for(const Move & m : castles) {  
        if (is_legal_move(m)) {
            ans.push_back(std::move(m));
        }
    }
    return std::move(ans);
}

bool is_in_list_of_moves(std::vector<Move> mv_ls, const Move mv) {
    for (const Move &m : mv_ls){
        if (m.start == mv.start && m.end == mv.end) { 
            return true;
        }
    } 
    return false;
}

std::string upper(char c) {
    if (c >= 'a') {
        c -= 32;
    }
    return std::string(1, c);
}

/// Plays the given move
void Board::Impl::play_move(const Move mv){
    // assert is a legal move
    assert(is_in_list_of_moves(get_legal_moves(turn), mv));
    // assert is my turn
    assert((get_square(mv.start) < 'a') == turn);
    if(turn) {
        augmoves += "\n" + std::to_string(moves.size() / 2 + 1) + ". ";
    }
    std::string augmv = "";
    char capt; char mov;
    // If its a castle, the rook also moves
    if (mv.start == "e1" && !white_castle[1]) {
        if (mv.end == "c1") {
            execute_move({"a1","d1"}, &capt, &mov);
            augmv += "O-O-O";
        } else if (mv.end == "g1") {
            execute_move({"h1","f1"}, &capt, &mov);
            augmv += "O-O";
        }
    } else if (mv.start == "e8" && !black_castle[1]) {
        if (mv.end == "c8") {
            execute_move({"a8","d8"}, &capt, &mov);
            augmv += "O-O-O";
        } else if (mv.end == "g8") {
            execute_move({"h8","f8"}, &capt, &mov);
            augmv += "O-O";
        }
    }
    // Record the move
    moves.push_back(mv);
    // Execute it
    execute_move(mv, &capt, &mov);
    // If a rook or king moved, update castling states appropriately
    if (mv.start == "a1") {
        white_castle[0] = true;
    } else if (mv.start == "e1") {
        white_castle[1] = true;
    } else if (mv.start == "h1") {
        white_castle[2] = true;
    } else if (mv.start == "a8") {
        black_castle[0] = true;
    } else if (mv.start == "e8") {
        black_castle[1] = true;
    } else if (mv.start == "h8") {
        black_castle[2] = true;
    }
    // If the king moves, update white_king of black_king
    if (mov == 'k') {
        black_king = mv.end;
    }
    if (mov == 'K') {
        white_king = mv.end;
    }
    // Record the Augmented (human-readable) move
    if (augmv.size() == 0){
        if (mov == 'P' || mov == 'p'){
            if (capt != ' ') {
                augmv = mv.start.substr(0,1) + "x" + mv.end;     // Like exd5
            } else {
                augmv = mv.end;                         // Like d5
            }
        } else {
            if (capt != ' ') {
                augmv = upper(mov) + mv.end;            // Like Nc3
            } else {
                augmv = upper(mov) + "x" + mv.end;      // Like Nxd5
            }
        }
    } if (am_in_check(!turn)) {
        if (get_legal_moves(!turn).size() == 0) {
            augmv += "# ";
        } else {
            augmv += "+ ";
        }
    }
    augmoves += augmv;
    // Add this board state to the bank of previous board states
    past_states.push_back(to_string());
    // Increment whose turn it is now
    turn = !turn;
}

// Returns the string of human-readable moves
std::string Board::Impl::move_string() const {
    return augmoves;
}

// Returns true if the player has no moves
bool Board::Impl::game_over() {
    return get_legal_moves(turn).size() == 0 || threefold_rep(past_states.back());
}

// Returns true if the game is over, black is in check, and it is black's turn
bool Board::Impl::white_wins() {
    return !turn && game_over() && am_in_check(false) && !threefold_rep(past_states.back());
}

// Returns true if the game is over, white is in check, and it is white's turn
bool Board::Impl::black_wins() {
    return turn && game_over() && am_in_check(true) && !threefold_rep(past_states.back());
}

bool Board::Impl::is_white_turn() const {
    return turn;
}

// Converts the board to a 64-long string
std::string Board::Impl::to_string() const {
    return std::string(b.begin(), b.end());
}

// Returns true if the state given has been repeated at least 3 times
bool Board::Impl::threefold_rep(const std::string last) const {
    return std::count(past_states.begin(), past_states.end(), last) >= 3;
}

// Sets the internal board state to the one given
void Board::Impl::set_board(const board_array &new_b) {
    for(char i = 0; i < 64; ++i){
        b[i] = new_b[i];
    }
}

Board::Board() : 
I(std::make_unique<Impl>())
{}

Board::Board(const board_array &b) :
I(std::make_unique<Impl>())
{ 
    I->set_board(b); 
}

Board::~Board(){}

void Board::play_move(Move mv){
    I->play_move(mv);
}

std::vector<Move> Board::get_past_moves() const{
    return I->get_past_moves();
}

board_array Board::get_board() const{
    return I->get_board();
}

std::vector<Move> Board::get_legal_moves(const bool amWhite) const{
    return I->get_legal_moves(amWhite);
}

std::string Board::to_string() const{
    return I->to_string();
}

std::string Board::move_string() const{
    return I->move_string();
}

bool Board::game_over() const{
    return I->game_over();
}

bool Board::white_wins() const{
    return I->white_wins();
}

bool Board::black_wins() const{
    return I->black_wins();
}

bool Board::is_white_turn() const{
    return I->is_white_turn();
}

char Board::get_square(std::string square) const{
    return I->get_square(square);
}

// Reset by simply throwing out the old board and getting a new one
void Board::reset(){
    I = std::make_unique<Impl>();
}

void Board::set_board(const board_array &b){
    I->set_board(b);
}
