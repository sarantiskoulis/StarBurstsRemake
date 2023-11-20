#include <iostream>
#include <random>
#include<vector>
#include<chrono>
#include <algorithm>
#include <numeric>
#include <execution>
#include <future>
#include <thread>
#include <set>
#include <fstream>
#include <string>
#include <yaml-cpp/yaml.h>
#include <cmath>
#include <utility>
#include "config.h"

using std::accumulate;
using std::cout;
using std::random_device;
using std::mt19937;
using std::uniform_int_distribution;

using namespace std;

void game_runs(int start,int end
               ,int pay_lines[10][5]
               ,const std::vector<std::vector<int>> base_vec
               ,PayTable payTable
               ,long long int symbol_points[7][5],
               vector<int>& total_wins,
               long long int symbol_rates[7][5],
               vector<int>& free_spins_wins,
               long long int symbol_points_free_spins[7][5],
               long long int symbol_rates_free_spins[7][5],
               vector<int>& base_wins,
               long long int symbol_points_base[7][5],
               long long int symbol_rates_base[7][5],
               long long int wild_hits[3],
               long long int wild_points[3]){


    for (int iter =start; iter <end; ++iter) {
        bool Spin = true;
        std::set<std::pair<int, int>> uniquePairs;
        int line_point = 0;

        vector<int> WildCols;
        std::pair<bool, std::vector<int>> respin;
        int game_view[5][3] = {0};

        while (Spin) {

            GetGameView(base_vec, game_view);  //  GETTING GAME VIEW
            for (int const col: WildCols) {
                for (int v = 0; v< 3;++v) {
                    game_view[col][v] = 7;
                }
            }


            respin = ExpandWilds(game_view);
            Spin = respin.first; //  Is there a single 7?

            for (int const col: respin.second) {
                WildCols.push_back(col);
            }



            int game_lines[10][5] = {0};
            GetGameLines(game_view, game_lines, pay_lines);
            for (int i = 0; i < 10; ++i) {
                vector<pair<int, int>> result = analyseArray(game_lines[i]);

                if (!result.empty()) {
                    for (const auto& pair: result) {
                        int occurance = pair.first;
                        int symbol = pair.second;
                        int normal_points = payTable.table[symbol][occurance - 1];
                        symbol_points[symbol][occurance - 1] += normal_points;

                        if (!WildCols.empty()) {
                            symbol_points_free_spins[symbol][occurance - 1] += normal_points;
                        }
                        if (WildCols.empty()) {
                            symbol_points_base[symbol][occurance - 1] += normal_points;
                        }

                        line_point += normal_points;

                        auto symbol_hit = std::make_pair(symbol, occurance - 1);
                        if (uniquePairs.find(symbol_hit) == uniquePairs.end()) {
                            symbol_rates[symbol][occurance - 1] += 1;

                            if (!WildCols.empty()) {
                                symbol_rates_free_spins[symbol][occurance - 1] += 1;
                            }
                            if (WildCols.empty()) {
                                symbol_rates_base[symbol][occurance - 1] += 1;
                            }


                            uniquePairs.insert(symbol_hit);
                        }
                    }
                }
            }
        }

        if (!WildCols.empty() && line_point > 0) {
            free_spins_wins.push_back(line_point);
        }
        if (!WildCols.empty() ) {
            wild_points[WildCols.size() - 1] += static_cast<long long int>(line_point);
            wild_hits[WildCols.size() - 1] += 1;
        }

        if (WildCols.empty() && line_point > 0) {

            base_wins.push_back(line_point);
        }

        if (line_point >0) {
            total_wins.push_back(line_point);

        }

    }
}

int main() {
    YAML::Node config = YAML::LoadFile("C:\\Users\\sarantis\\CLionProjects\\StarburstRemake\\config.yaml");
    const std::vector<std::vector<int>> base_vec = LoadWheelsData(config["wheels"]["wheel_base"]);

    PayTable paytable = GetPayTableFromYaml(config);

    const YAML::Node& payLines = config["pay_lines"];
    int pay_lines[10][5] = {0};
    LoadPayLines(payLines, pay_lines);




    int default_bet = 30;
    const int num_threads = std::thread::hardware_concurrency();
    long long int num_elements = 10'000'000;
    const int elements_per_thread = num_elements / num_threads;

    vector<std::thread> threads;

    // OVER ALL GAME
    vector<vector<int>> total_wins(num_threads);
    long long int symbol_points[num_threads][7][5] = {0};
    long long int symbol_rates[num_threads][7][5] = {0};
    // FREE SPINS
    long long int symbol_points_free_spins[num_threads][7][5] = {0};
    long long int symbol_rates_free_spins[num_threads][7][5] = {0};
    vector<vector<int>> free_spins_wins(num_threads);
    long long int wild_hits[num_threads][3] = {0};
    long long int wild_points[num_threads][3] = {0};

    // BASE GAME
    long long int symbol_points_base[num_threads][7][5] = {0};
    long long int symbol_rates_base[num_threads][7][5] = {0};
    vector<vector<int>> base_wins(num_threads);
    auto start = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < num_threads; ++i) {
        int start = i * elements_per_thread;
        int end = (i == num_threads - 1) ? num_elements : start + elements_per_thread;
        threads.emplace_back(
                game_runs,start, end,
                std::ref(pay_lines),
                std::ref(base_vec),
                std::ref(paytable),
                std::ref(symbol_points[i]),
                std::ref(total_wins[i]),
                std::ref(symbol_rates[i]),
                std::ref(free_spins_wins[i]),
                std::ref(symbol_points_free_spins[i]),
                std::ref(symbol_rates_free_spins[i]),
                std::ref(base_wins[i]),
                std::ref(symbol_points_base[i]),
                std::ref(symbol_rates_base[i]),
                std::ref(wild_hits[i]),
                std::ref(wild_points[i]));

    }
    for (auto &thread: threads) {
        thread.join();
    }
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    auto duration_mins = std::chrono::duration_cast<std::chrono::minutes>(end - start);
    cout << "Time: " << duration << endl;


    // Game Points
    vector<int> total_wins_values =  FlattenData(total_wins);
    long long int total_winnings = GetArraySum(total_wins_values);
    std::pair<double, double> game_stats = GetStats(total_wins_values,
                                                          total_winnings,
                                                          total_wins_values.size(),
                                                          static_cast<double>(default_bet)
    );
    double game_variance = game_stats.first;
    double game_std = game_stats.second;
    // Free Spin Points
    vector<int> total_free_wins_values =  FlattenData(free_spins_wins);
    long long int total_free_spins_winnings = GetArraySum(total_free_wins_values);
    std::pair<double, double> free_spins_stats = GetStats(total_free_wins_values,
                                                      total_free_spins_winnings,
                                                      total_free_wins_values.size(),
                                                      static_cast<double>(default_bet)
                                                      );
    double free_spins_variance = free_spins_stats.first;
    double free_spins_std = free_spins_stats.second;

    double wild_points_f[3] = {0};
    for (int i=0;i<num_threads; ++i) {
        for (int k=0;k<3;++k) {
            wild_points_f[k] += wild_points[i][k] / static_cast<double>(default_bet * num_elements);
        }
    }
    long long int wild_hit_f[3] = {0};
    for (int i=0;i<num_threads; ++i) {
        for (int k=0;k<3;++k) {
            wild_hit_f[k] += wild_hits[i][k];
        }
    }
    // Base Game
    vector<int> total_base_wins_values =  FlattenData(base_wins);
    long long int total_base_winnings = GetArraySum(total_base_wins_values);
    std::pair<double, double> base_stats = GetStats(total_base_wins_values,
                                                          total_base_winnings,
                                                          total_base_wins_values.size(),
                                                          static_cast<double>(default_bet)
    );
    double base_variance = base_stats.first;
    double base_std = base_stats.second;

    // OVERALL GAME INFO
    long long int bet = num_elements * default_bet;
    double game_rtp = total_winnings / static_cast<double>(bet);
    double base_rtp = total_base_winnings/ static_cast<double>(bet);
    double free_spins_rtp = total_free_spins_winnings / static_cast<double>(bet);
    cout << "Iterations: " << num_elements << endl;
    cout << "Total Bet: " << bet << endl;
    cout << "Total Wins: " << total_winnings << endl;
    cout << "Total RTP: " << game_rtp << endl;
    cout << "Variance: " << game_variance << endl;
    cout << "Standard Dev.: " << game_std << endl;
    cout << "Hit Frequency: " << total_wins_values.size()/ static_cast<double>(num_elements) << endl;
    cout << endl << endl;


    cout << "Base Spins" << endl;
    cout << "RTP: " << base_rtp << endl;
    cout << "Variance: " << base_variance << endl;
    cout << "Standard Dev.: "<< base_std << endl;
    cout << "Hit Frequency: " << total_base_wins_values.size()/ static_cast<double>(num_elements)  << endl;
    cout << "--------" <<endl;
    cout << "Free Spins" << endl;
    cout << "RTP: " <<free_spins_rtp << endl;
    cout << "Variance: " << free_spins_variance << endl;
    cout << "Standard Dev.: " << free_spins_std << endl;
    cout << "Hit Frequency: " << total_free_wins_values.size()/ static_cast<double>(num_elements)  << endl;
    cout << "Wild Hit Frequency: 1 in " << num_elements / (wild_hit_f[0] + wild_hit_f[1]+ wild_hit_f[2]) << endl;
    for (int wildIter = 0; wildIter < 3; ++wildIter ) {
        cout << wildIter+1 << " wild : 1 in " << num_elements / wild_hit_f[wildIter] << "  RTP : " << wild_points_f[wildIter] << endl;
    }



    cout << endl << endl;





    double symbol_points_t[7][5] = {};
    double symbol_hits_t[7][5] = {};

    double symbol_free_spins_points_t[7][5] = {};
    double symbol_free_spins_hits_t[7][5] = {};

    double symbol_base_points_t[7][5] = {};
    double symbol_base_hits_t[7][5] = {};
    cout << std::fixed << std::setprecision(5);
    for (int i = 0; i< num_threads; ++i) {
        for (int j = 0; j < 7; ++ j) {
            for (int k = 0; k < 5; ++k) {
                symbol_points_t[j][k] += symbol_points[i][j][k] / static_cast<double>(bet);
            }
        }
    }
    for (int i = 0; i< num_threads; ++i) {
        for (int j = 0; j < 7; ++ j) {
            for (int k = 0; k < 5; ++k) {
                symbol_hits_t[j][k] += symbol_rates[i][j][k] / static_cast<double>(num_elements);
            }
        }
    }
    for (int i = 0; i< num_threads; ++i) {
        for (int j = 0; j < 7; ++ j) {
            for (int k = 0; k < 5; ++k) {
                symbol_free_spins_points_t[j][k] += symbol_points_free_spins[i][j][k] / static_cast<double>(bet);
            }
        }
    }
    for (int i = 0; i< num_threads; ++i) {
        for (int j = 0; j < 7; ++ j) {
            for (int k = 0; k < 5; ++k) {
                symbol_free_spins_hits_t[j][k] += symbol_rates_free_spins[i][j][k]/ static_cast<double>(num_elements);
            }
        }
    }
    for (int i = 0; i< num_threads; ++i) {
        for (int j = 0; j < 7; ++ j) {
            for (int k = 0; k < 5; ++k) {
                symbol_base_points_t[j][k] += symbol_points_base[i][j][k] / static_cast<double>(bet);
            }
        }
    }
    for (int i = 0; i< num_threads; ++i) {
        for (int j = 0; j < 7; ++ j) {
            for (int k = 0; k < 5; ++k) {
                symbol_base_hits_t[j][k] += symbol_rates_base[i][j][k]/ static_cast<double>(num_elements);
            }
        }
    }






    // SYMBOL POINTS
    cout << "Game" << endl;
    cout << "RTP" << endl;
    for (int j = 0; j < 7; ++ j) {
        for (int k = 2; k < 5; ++k) {
            cout << symbol_points_t[j][k] << " ";
        }
        cout << endl;
    } cout << endl;


    cout << "Hit Frequency" << endl;
    for (int j = 0; j < 7; ++ j) {
        for (int k = 2; k < 5; ++k) {
            cout << symbol_hits_t[j][k] << " ";
        }
        cout << endl;
    }

    cout << endl << endl;

    // FREE SPINS
    cout << "Free Spins" << endl;
    cout << "RTP" << endl;
    for (int j = 0; j < 7; ++ j) {
        for (int k =2; k < 5; ++k) {
            cout << symbol_free_spins_points_t[j][k] << " ";
        }
        cout << endl;
    } cout << endl;

    cout << "Hit Frequency" << endl;
    for (int j = 0; j < 7; ++ j) {
        for (int k = 2; k < 5; ++k) {
            cout << symbol_free_spins_hits_t[j][k] << " ";
        }
        cout << endl;
    }
    cout << endl << endl;

    // BASE GAME
    cout << "Base" << endl;
    cout << "RTP" << endl;
    for (int j = 0; j < 7; ++ j) {
        for (int k =2; k < 5; ++k) {
            cout << symbol_base_points_t[j][k] << " ";
        }
        cout << endl;
    } cout << endl;

    cout << "Hit Frequency" << endl;
    for (int j = 0; j < 7; ++ j) {
        for (int k = 2; k < 5; ++k) {
            cout << symbol_base_hits_t[j][k] << " ";
        }
        cout << endl;
    }
    return 0;
}
