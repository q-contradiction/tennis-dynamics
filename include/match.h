#pragma once

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <array>

using std::string;
using std::array;
using std::vector;
using std::ostream;

using std::cout;

std::ofstream write_file(const string& filename, int type);

// 1 stats struct & 3 classes, a player Class, a match class & a collection of simulated matches. 

struct stats {
    int sets, setsW;
    int games, gamesW, gamesSvr, gamesSvrW, breaks, tiebreaks, tiebreaksW;
    int points, pointsW, pointsSvr, pointsSvrW;
    double pointSvrRate, pointRate, holdRate, gameRate, setRate;

    void set_rates() {
        pointSvrRate = (double)pointsSvrW / pointsSvr;
        pointRate = (double)pointsW / points;
        holdRate = (double)gamesSvrW / gamesSvr;
        gameRate = (double)gamesW / games;
        setRate = (double)setsW / sets;
    }

    stats& operator+=(const stats& other) {
        this->points += other.points;
        this->pointsSvr += other.pointsSvr;
        this->pointsSvrW += other.pointsSvrW;
        this->pointsW += other.pointsW;

        this->games += other.games;
        this->gamesSvr += other.gamesSvr;
        this->gamesSvrW += other.gamesSvrW;
        this->gamesW += other.gamesW;
        this->breaks += other.breaks;

        this->tiebreaks += other.tiebreaks;
        this->tiebreaksW += other.tiebreaksW;

        this->sets += other.sets;
        this->setsW += other.setsW;

        return *this;
    }

    void display_stats(ostream& output = cout);
    void display_rates(ostream& output = cout);
};

class Player {

    friend class TennisMatch;
private:
    string playerName;
    double iProbServer, iProbReceiver;
    bool winner;
    double tProbServer, tProbReceiver;

    stats matchStats{};
    vector<stats> setStats;
    stats currentSet{};

    void initialize_prop();
    //void set_prop(double prop);
    void update_points(bool server, bool winner);
    void update_games(bool server, bool winner, bool tiebreak);
    void update_sets(bool winner);
    void set_match_stats(bool winner);

public:
    Player() = default;
    Player(string name, double probS, double probR);

    string get_name() { return playerName; }
    bool is_winner() { return winner; }
    double get_init_server_prop() { return iProbServer; }
    double get_server_prop() { return tProbServer; }
    stats get_match_stats() { return matchStats; }
    vector<stats> get_set_stats() { return setStats; }

};

class TennisMatch : public Player {

private:
    int simID;

    Player players[2];

    int bestOf, maxSets;
    int server, startingServer;                   // Server in current point. Server = {0, 1}, 0 for A | Starting server (0 or 1, 2 for coin flip)

    // Running Score
    array<int, 2> iPoints, gPoints;               // Initial points of the game, points of the current game 
    array<int, 2> iGames, sGames;                 // Initial games of the set, games of the current set
    array<int, 2> iSets, sets;                    // Initial sets of the match, sets of the match

    bool gameON, setON, matchON;                  // Flag is set to 0, if game/set/match ended     
    bool tiebreakON;

    string model;
    vector<double> modelParams;                   // Not used atm

    // {simID, Sets[A, B], Games[A, B], Points[A, B], propServerA, propServerB, Server, PointWinner}
    vector<array<double, 11>> playByPlay;

    //void set_model_params();
    void set_starting_server();

    bool is_tiebreak();
    bool is_game_on();
    bool is_set_on();
    bool is_match_on();

    void initialize_game();
    void initialize_set();
    void initialize_match();

    void simulate_rally();
    void simulate_game();
    void simulate_set();

    void update_momentum(int pointwinner);
    void update_points(int pointwinner);
    void update_games(int gamewinner);
    void update_sets(int setwinner);
    void set_match_stats(int matchwinner);
public:
    TennisMatch(double propA, double propB, int best_of, string model = "static",
        int starting_server = 2, int PA = 0, int PB = 0, int GA = 0, int GB = 0, int SA = 0, int SB = 0,
        string nameA = "PlayerA", string nameB = "PlayerB", int id = 0);
    TennisMatch() = default;

    void simulate_match();
    Player get_player(int i) { return players[i]; }
    void display_params(ostream& output = cout);
    void display_playbyplay(ostream& output = cout);
    void display_player_stats(ostream& output = cout, int player = 0);
};


// TennisMC aggregates results, by instantiating NSIMS TennisMatches under the same params
class TennisMC : public TennisMatch {

private:
    int NSIMS;
    vector<TennisMatch> tennisMatches;

    array<double, 2>  winProp{};
    array<double, 2>  expPoints{}, expGames{}, expSets{};   //expected values
    double tiebreakProp, finalSetProp;

    stats simStats[2]{};                //simulation stats for Player A & B
    void set_player_sim_stats(int pl);

public:
    TennisMC(double propA, double propB, int best_of, int sims = 10000, string model = "static",
        int starting_server = 2, int PA = 0, int PB = 0, int GA = 0, int GB = 0, int SA = 0, int SB = 0,
        string nameA = "PlayerA", string nameB = "PlayerB");
    double get_win_prop() { return winProp[0]; }
    void display_playbyplay(ostream& output = cout, int nmatches = 100);
    void display_player_stats(ostream& output = cout, int player = 0);
    void display_sim_rates(ostream& output = cout);
};

