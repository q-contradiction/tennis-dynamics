/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * A simulation of Tennis to study basic dynamics                                                     *
 *                                                                                                    *
 * The model is a simplification of actual matches, propabilities are constant.                       *
 *                                                                                                    *
 * Winner of the match is determined in a best of {3,5} - standard rules apply                        *
 *                                                                                                    *
 * Parameterization includes probability of winning the rally depending on who is serving             *
 * and the initial state of the match {Points|Games|Sets}                                             *
 *                                                                                                    *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "../include/match.h"

int main() 
{

    std::ofstream csvPbp = write_file(".\\files-sim\\pbp_sample1.csv", 0);
    std::ofstream csvMatch = write_file(".\\files-sim\\match_sample1.csv", 1);
    std::ofstream csvSim = write_file(".\\files-sim\\sim1.csv", 2);

    // Example
    TennisMC M1(0.51, 0.52, 5);
    M1.display_playbyplay(csvPbp);
    M1.display_player_stats(csvMatch);

    for (double pa = 0.1; pa <= 0.9; pa += 0.01) {
        for (double pb = 0.1; pb <= 0.9; pb += 0.01) {
            cout << pa << " " << pb << std::endl;
            TennisMC M(pa, pb, 3);
            M.display_sim_rates(csvSim);
        }
    }

    csvPbp.close();
    csvMatch.close();
    csvSim.close();

    return 0;
}

