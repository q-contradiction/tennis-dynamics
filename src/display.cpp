#include "../include/match.h"

#define DELIM ','

using std::endl;

template <typename vector1D>
void print_1dvector(vector1D vec, ostream& output) {
    for (size_t i = 0; i < vec.size(); ++i) {
        output << vec[i];
        if (i < vec.size() - 1) output << DELIM;
    }
}

std::ofstream write_file(const string& filename, int type) {

    vector<string> pbp{ "simId", "setA", "setB", "gameA", "gameB", "pointsA", "pointsB", "propServerA", "propServerB", "server", "pointWinner" };

    vector<string> match{ "propServerA", "propServerB", "bestof", "model", "winner",
        "pointSvrRate", "pointRate", "holdRate", "gameRate", "setRate",
        "sets", "setsW", "games", "gamesW", "gamesSvr", "gamesSvrW",
        "breaks", "tiebreaks", "tiebreaksW", "points", "pointsW", "pointsSvr", "pointsSvrW" };

    vector<string> sim{ "propServerA", "propServerB", "bestof", "model",
        "pointServeRate", "pointRate", "holdRate", "gameRate", "setRate", "winRate",
        "expPoints", "expGames", "expSets", "tiebreakRate", "medianTGames" };

    array<vector<string>, 3> headers{ pbp, match, sim };

    std::ofstream output(filename);

    if (!output.is_open()) {
        throw std::runtime_error("Error opening file: " + filename);
    }

    print_1dvector(headers[type], output);
    output << endl;
    return (output);
}

void stats::display_stats(ostream& output) {

    output << sets << DELIM << setsW << DELIM << games << DELIM << gamesW << DELIM
        << gamesSvr << DELIM << gamesSvrW << DELIM << breaks << DELIM << tiebreaks << DELIM << tiebreaksW << DELIM
        << points << DELIM << pointsW << DELIM << pointsSvr << DELIM << pointsSvrW;
}

void stats::display_rates(ostream& output) {

    output.precision(5);
    output << pointSvrRate << DELIM << pointRate << DELIM << holdRate << DELIM << gameRate << DELIM << setRate;
}

void TennisMatch::display_params(ostream& output) {

    output << players[0].get_init_server_prop() << DELIM << players[1].get_init_server_prop() << DELIM << bestOf << DELIM << model;
}

void TennisMatch::display_playbyplay(ostream& output) {

    for (const auto& play : playByPlay) {
        print_1dvector(play, output);
        output << endl;
    }
}

void TennisMatch::display_player_stats(ostream& output, int pl) {

    output.precision(5);

    display_params(output);
    output << DELIM;
    output << (int)players[pl].is_winner();
    output << DELIM;
    players[pl].get_match_stats().display_rates(output);
    output << DELIM;
    players[pl].get_match_stats().display_stats(output);
    output << endl;
}


void TennisMC::display_playbyplay(ostream& output, int nmatches) {

    for (int i = 0; i < nmatches; ++i) {
        tennisMatches[i].display_playbyplay(output);
    }
}

void TennisMC::display_player_stats(ostream& output, int pl) {

    output.precision(5);

    for (int i = 0; i < NSIMS; ++i) {
        tennisMatches[i].display_player_stats(output, pl);
    }
}

void TennisMC::display_sim_rates(ostream& output) {

    output.precision(5);

    tennisMatches[1].display_params(output);
    output << DELIM;
    simStats[0].display_rates(output);
    output << DELIM;
    output << winProp[0] << DELIM << expPoints[0] << DELIM << expGames[0] << DELIM << expSets[0] << DELIM
        << tiebreakProp << DELIM << medianTGames << endl;
}