#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include <functional>

#include "Board.hh"

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
const board_array E4 = {
    'R','N','B','Q','K','B','N','R',
    'P','P','P','P',' ','P','P','P',
    ' ',' ',' ',' ',' ',' ',' ',' ',
    ' ',' ',' ',' ','P',' ',' ',' ',
    ' ',' ',' ',' ',' ',' ',' ',' ',
    ' ',' ',' ',' ',' ',' ',' ',' ',
    'p','p','p','p','p','p','p','p',
    'r','n','b','q','k','b','n','r'
};

const board_array ExD5 = {
    'R','N','B','Q','K','B','N','R',
    'P','P','P','P',' ','P','P','P',
    ' ',' ',' ',' ',' ',' ',' ',' ',
    ' ',' ',' ',' ',' ',' ',' ',' ',
    ' ',' ',' ','P',' ',' ',' ',' ',
    ' ',' ',' ',' ',' ',' ',' ',' ',
    'p','p','p',' ','p','p','p','p',
    'r','n','b','q','k','b','n','r'
};

const board_array EN_PASSANT = {
    'R','N','B','Q','K','B','N','R',
    'P','P','P','P',' ','P','P','P',
    ' ',' ',' ',' ',' ',' ',' ',' ',
    ' ',' ',' ',' ',' ',' ',' ',' ',
    ' ',' ',' ',' ',' ',' ',' ',' ',
    ' ',' ',' ',' ','P',' ',' ',' ',
    'p','p','p',' ',' ','p','p','p',
    'r','n','b','q','k','b','n','r'
};

bool vec_contains(std::vector<Move> v, const Move m){
    auto lamb = [&m](Move const &m2){return m.start == m2.start && m.end == m2.end; };
    return std::any_of(v.begin(), v.end(), lamb) > 0;
}


auto b = std::make_unique<Board>();

TEST_CASE( "board setup" ){
    REQUIRE( b->get_past_moves().size() == 0 );
    REQUIRE( b->get_board() == START_BOARD );
    REQUIRE( !b->game_over() );
    REQUIRE( b->is_white_turn() );
}

TEST_CASE( "initial move calculation" ){
    REQUIRE( b->get_legal_moves(true).size() == 20 );
    REQUIRE( b->get_legal_moves(false).size() == 20 );
}

TEST_CASE( "moves update board state" ){
    b->play_move({"e2","e4"});
    REQUIRE( b->get_board() == E4);
    REQUIRE( !b->is_white_turn() );
    b->play_move({"d7","d5"});
    REQUIRE( b->get_past_moves().size() == 2 );
}

TEST_CASE( "captures happen" ){
    REQUIRE( vec_contains(b->get_legal_moves(true), {"e4","d5"}) );
    b->play_move({"e4","d5"});
    REQUIRE( b->get_board() == ExD5 );
    REQUIRE( !vec_contains(b->get_legal_moves(false), {"d5","d4"}) );
}

TEST_CASE( "en passant" ){
    b->play_move({"e7","e5"});
    REQUIRE( vec_contains(b->get_legal_moves(true), {"d5","e6"}) );
    b->play_move({"d5","e6"});
}

TEST_CASE ( "bishops" ){
    REQUIRE( vec_contains(b->get_legal_moves(false), {"c8","d7"}) );
    b->play_move({"c8", "d7"});
    b->play_move({"f1","a6"});
}

TEST_CASE ( "knights" ) {
    REQUIRE( vec_contains(b->get_legal_moves(false), {"b8", "c6"}) );
    b->play_move({"b8", "c6"});
    b->play_move({"g1","f3"});
}

TEST_CASE ( "queens" ) {
    REQUIRE( vec_contains(b->get_legal_moves(false), {"d8","h4"}) );
    b->play_move({"d8","h4"});
    b->play_move({"d1","e2"});
    REQUIRE( vec_contains(b->get_legal_moves(false), {"h4","a4"}) );
    REQUIRE( vec_contains(b->get_legal_moves(false), {"h4","h5"}) );
}

TEST_CASE ( "check" ) {
    b->play_move({"h4","f2"});
    REQUIRE( b->get_legal_moves(true).size() == 3);
    b->play_move({"e2","f2"});
}

TEST_CASE ( "castling" ) {
    REQUIRE( vec_contains(b->get_legal_moves(false), {"e8","c8"}) );
    b->play_move({"b7","b6"});
    b->play_move({"e1","g1"});
    // can't castle into check
    REQUIRE( !vec_contains(b->get_legal_moves(false), {"e8","c8"}) );
    b->play_move({"h7","h6"});
    b->play_move({"a6","b5"});
    b->play_move({"h6","h5"});
    b->play_move({"e6","e7"});
    // can't castle through check
    REQUIRE( !vec_contains(b->get_legal_moves(false), {"e8","c8"}) );
    b->play_move({"h5","h4"});
}

TEST_CASE( "promoting" ) {
    REQUIRE( vec_contains(b->get_legal_moves(true), {"e7","f8=Q"}) );
    REQUIRE( vec_contains(b->get_legal_moves(true), {"e7","f8=B"}) );
    REQUIRE( vec_contains(b->get_legal_moves(true), {"e7","f8=N"}) );
    REQUIRE( vec_contains(b->get_legal_moves(true), {"e7","f8=R"}) );
    b->play_move({"e7","f8=Q"});
}

TEST_CASE( "castling pt 2" ) {
    // Can't castle out of check
    REQUIRE( !vec_contains(b->get_legal_moves(false), {"e8","c8"}) );
    b->play_move({"e8","f8"});
    b->play_move({"b2","b3"});
    // Can't castle once have moved king
    REQUIRE( !vec_contains(b->get_legal_moves(false), {"f8","c8"}) );
    b->play_move({"a7","a6"});
}

TEST_CASE( "checkmate" ) {
    b->play_move({"f1","e1"});
    b->play_move({"h4","h3"});
    b->play_move({"c1","a3"});
    b->play_move({"c6","b4"});
    b->play_move({"a3","b4"});
    b->play_move({"c7","c5"});
    b->play_move({"b4","c5"});
    b->play_move({"b6","c5"});
    b->play_move({"f2","c5"});
    b->play_move({"g8","e7"});
    b->play_move({"c5","e7"});
    b->play_move({"f8","g8"});
    b->play_move({"f3","g5"});
    b->play_move({"a8","e8"});
    b->play_move({"e7","e8"});
    b->play_move({"d7","e8"});
    b->play_move({"e1","e8"});
    REQUIRE( b->game_over() );
    REQUIRE( b->white_wins() );
}

TEST_CASE( "reset" ) {
    b->reset();
    REQUIRE( b->get_past_moves().size() == 0 );
    REQUIRE( b->get_board() == START_BOARD );
    REQUIRE( !b->game_over() );
    REQUIRE( b->is_white_turn() );
}

TEST_CASE( "threefold repetition" ) {
    b->play_move({"g1","f3"});
    b->play_move({"g8","f6"});
    b->play_move({"f3","g1"});
    b->play_move({"f6","g8"});
    b->play_move({"g1","f3"});
    b->play_move({"g8","f6"});
    b->play_move({"f3","g1"});
    b->play_move({"f6","g8"});
    REQUIRE( b->game_over() );
    REQUIRE( !b->white_wins() );
    REQUIRE( !b->black_wins() );
    b->reset();
}

