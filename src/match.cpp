#include <random>
#include "../include/match.h"

std::default_random_engine generator;
std::uniform_real_distribution<double> distribution(0.0, 1.0);

Player::Player(std::string name, double probS, double probR) {

    playerName = name;
    iProbServer = probS;
    iProbReceiver = probR;
}

void Player::initialize_prop() {
    tProbServer = iProbServer;
    tProbReceiver = iProbReceiver;
}

void Player::update_points(bool server, bool winner) {

    currentSet.points += 1;
    currentSet.pointsW += (int)winner;

    currentSet.pointsSvr += (int)server;
    currentSet.pointsSvrW += (int)(server && winner);

}

void Player::update_games(bool server, bool winner, bool tiebreak) {

    currentSet.games += 1;
    currentSet.gamesW += (int)winner;

    currentSet.gamesSvr += (int)(server && !tiebreak);
    currentSet.gamesSvrW += (int)(server && !tiebreak && winner);
    currentSet.breaks += (int)(!server && !tiebreak && winner);

    currentSet.tiebreaks += (int)tiebreak;
    currentSet.tiebreaksW += (int)(tiebreak && winner);
}

void Player::update_sets(bool winner) {

    currentSet.sets += 1;
    currentSet.setsW += (int)winner;
    currentSet.set_rates();

    // push & re-initialize
    setStats.push_back(currentSet);
    currentSet = stats();
}

void Player::set_match_stats(bool winner) {

    this->winner = winner;

    for (int i = 0; i < setStats.size(); ++i) {
        matchStats += setStats[i];
    }
    matchStats.set_rates();
}


TennisMatch::TennisMatch(double propA, double propB, int best_of, std::string model,
    int starting_server, int PA, int PB, int GA, int GB, int SA, int SB,
    std::string nameA, std::string nameB, int id) {

    players[0] = Player(nameA, propA, 1 - propB);
    players[1] = Player(nameB, propB, 1 - propA);

    bestOf = best_of;
    maxSets = (best_of == 3) ? 2 : 3;

    startingServer = starting_server;

    iPoints = { PA, PB };
    iGames = { GA, GB };
    iSets = { SA, SB };

    this->model = model;
    simID = id;
}

void TennisMatch::set_starting_server() {

    if (startingServer == 2) {      // 2 coin flip - assign random server
        double r = ((double)rand() / (RAND_MAX));
        server = (r <= 0.5) ? 0 : 1;
    }
    else {
        server = startingServer;
    }
}

void TennisMatch::initialize_game() {

    gameON = true;
    gPoints = { 0, 0 };
    server = server ^ 1;    // flip server
}

void TennisMatch::initialize_set() {

    setON = true;
    sGames = { 0, 0 };

    initialize_game();
}

void TennisMatch::initialize_match() {

    players[0].initialize_prop();
    players[1].initialize_prop();

    gPoints = iPoints;
    sGames = iGames;
    sets = iSets;

    set_starting_server();

    tiebreakON = is_tiebreak();

    gameON = is_game_on();
    setON = is_set_on();
    matchON = is_match_on();

}

bool TennisMatch::is_tiebreak() {

    return (sGames[0] == 6 && sGames[1] == 6) ? true : false;
}

bool TennisMatch::is_game_on() {

    int min_points = tiebreakON ? 7 : 4;

    int gamewinner1 = ((gPoints[0] >= min_points) && (gPoints[0] - gPoints[1] >= 2)) ? 1 : 0;
    int gamewinner2 = ((gPoints[1] >= min_points) && (gPoints[1] - gPoints[0] >= 2)) ? 1 : 0;

    bool game_ended = gamewinner1 || gamewinner2;

    if (game_ended) {
        //gamewinner has the same value of gamewinner status of B.
        int gamewinner = gamewinner2;
        update_games(gamewinner);
    }

    return !game_ended;
}

bool TennisMatch::is_set_on() {

    bool set_ended, setwinner1, setwinner2;

    setwinner1 = ((sGames[0] >= 6 && sGames[0] - sGames[1] >= 2) || (tiebreakON && sGames[0] == sGames[1] + 1)) ? 1 : 0;
    setwinner2 = ((sGames[1] >= 6 && sGames[1] - sGames[0] >= 2) || (tiebreakON && sGames[1] == sGames[0] + 1)) ? 1 : 0;
    set_ended = setwinner1 || setwinner2;

    if (set_ended) {
        // setwinner has the same value of setwinner status of B
        int setwinner = setwinner2;
        update_sets(setwinner);
    }

    return !set_ended;
}

bool TennisMatch::is_match_on() {

    int matchwinner1 = (sets[0] == maxSets) ? 1 : 0;
    int matchwinner2 = (sets[1] == maxSets) ? 1 : 0;
    bool match_ended = matchwinner1 || matchwinner2;

    if (match_ended) {
        int matchwinner = matchwinner2;
        set_match_stats(matchwinner);
    }

    return !match_ended;
}

// Stats for Player1 are opposite to player 2 - no need to update
// If server/winner is 0, the bool value for player A is true, this it is !(variable). for Player B is (variable) 
void TennisMatch::update_points(int pointwinner) {

    array<double, 11> play = { simID, sets[0], sets[1], sGames[0], sGames[1], gPoints[0], gPoints[1],
        players[0].get_server_prop() , players[1].get_server_prop(),  server, pointwinner };
    playByPlay.push_back(play);

    gPoints[pointwinner]++;
    players[0].update_points(!server, !pointwinner);
    //players[1].update_points(server, pointwinner);
}

void TennisMatch::update_games(int gamewinner) {

    sGames[gamewinner]++;
    players[0].update_games(!server, !gamewinner, tiebreakON);
    //players[1].update_games(server, gamewinner, tiebreakON);
}

void TennisMatch::update_sets(int setwinner) {

    sets[setwinner]++;
    players[0].update_sets(!setwinner);
    //players[1].update_sets(setwinner);

}

void TennisMatch::set_match_stats(int matchwinner) {

    players[0].set_match_stats(!matchwinner);
    //players[1].set_match_stats(matchwinner);
}

// main function to update the point probabilities based on model|server|previous pointwnner 

void TennisMatch::update_momentum(int pointwinner) {}

void TennisMatch::simulate_rally() {

    double x = distribution(generator);
    double prop = players[server].get_server_prop();
    int pointwinner = (x <= prop) ? server : server ^ 1;

    update_points(pointwinner);
    update_momentum(pointwinner);
}

// Initialization of game & set not in the beggining
// Simulation may start in the middle of a game & set based on input
void TennisMatch::simulate_game() {

    tiebreakON = is_tiebreak();

    while (gameON) {
        simulate_rally();
        gameON = is_game_on();
        //ABBA rule in Tiebreak
        if (tiebreakON && (gPoints[0] + gPoints[1]) % 2)
            server = server ^ 1;
    }

    initialize_game();
}

void TennisMatch::simulate_set() {

    while (setON) {
        simulate_game();
        setON = is_set_on();
    }
    initialize_set();
}

void TennisMatch::simulate_match() {

    initialize_match();

    while (matchON) {
        simulate_set();
        matchON = is_match_on();
    }
}

double median(vector<int> a)
{
    int n = a.size();

    // If size of the arr[] is even
    nth_element(a.begin(), a.begin() + n / 2, a.end());
    nth_element(a.begin(), a.begin() + (n - 1) / 2, a.end());

    return (double)(a[(n - 1) / 2] + a[n / 2]) / 2;
}


TennisMC::TennisMC(double propA, double propB, int best_of, int sims, string model,
    int starting_server, int PA, int PB, int GA, int GB, int SA, int SB,
    string nameA, string nameB) {

    NSIMS = sims;
    int winsA = 0;
    vector<int> totalGames; //for median calculations
    for (int i = 0; i < NSIMS; ++i) {
        tennisMatches.push_back(TennisMatch(propA, propB, best_of, model, starting_server,
            PA, PB, GA, GB, SA, SB, nameA, nameB, i));
        tennisMatches[i].simulate_match();

        simStats[0] += tennisMatches[i].get_player(0).get_match_stats();
        //simStats[1] += tennisMatches[i].get_player(1).get_match_stats();

        winsA += tennisMatches[i].get_player(0).is_winner();
        totalGames.push_back(tennisMatches[i].get_player(0).get_match_stats().games);
    }

    winProp[0] = (double)winsA / NSIMS; winProp[1] = 1 - winProp[0];

    set_player_sim_stats(0);
    //set_player_sim_stats(1);

    tiebreakProp = (double)simStats[0].tiebreaks / simStats[0].sets;
    medianTGames = median(totalGames);

}

void TennisMC::set_player_sim_stats(int pl) {

    simStats[pl].set_rates();

    expPoints[pl] = (double)simStats[pl].pointsW / NSIMS;
    expGames[pl] = (double)simStats[pl].gamesW / NSIMS;
    expSets[pl] = (double)simStats[pl].setsW / NSIMS;
}