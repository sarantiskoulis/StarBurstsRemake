//
// Created by sarantis on 13/11/2023.
//
#include "config.h"
#include <iostream>
#include <random>
#include<vector>
#include<chrono>
#include <algorithm>
#include <numeric>
#include <future>
#include <thread>
#include <set>
#include <fstream>
#include <yaml-cpp/yaml.h>
#include <cmath>
#include <utility>

using std::accumulate;
using std::cout;
using std::random_device;
using std::mt19937;
using std::uniform_int_distribution;

using namespace std;

std::vector<std::vector<int>> LoadWheelsData(const YAML::Node &node) {
    std::vector<std::vector<int>> wheelsData;
    for (const auto &wheel : node) {
        std::vector<int> wheelVec;
        for (const auto &value : wheel) {
            wheelVec.push_back(value.as<int>());
        }
        wheelsData.push_back(wheelVec);
    }
    return wheelsData;
}

void LoadPayLines(const YAML::Node &node, int pay_lines_arry[10][5]) {
    int i = 0;
    for (const auto &wheel : node) {
        int j = 0;
        for (const auto &value : wheel) {
            pay_lines_arry[i][j] = value.as<int>();
            ++j;
        }
        ++i;
    }
}


PayTable GetPayTableFromYaml(const YAML::Node& yaml) {
    int num_symbols = 7;
    int num_cols = 5;
    PayTable payTable;

    for (int i = 0; i < num_symbols; ++i) {
        for (int j = 0; j < num_cols; ++j) {
            if (yaml["pay_table"][i] && yaml["pay_table"][i].IsSequence()) {
                payTable.table[i][j] = yaml["pay_table"][i][j].as<int>();
            } else {
                // Handle the error if the node is not present or not a sequence
                std::cerr << "Error in YAML format: pay_table[" << i << "] is not a valid sequence." << std::endl;
                // You could throw an exception or handle it as per your error handling policy
            }
        }
    }

    return payTable;
}


void GetGameView(const std::vector<std::vector<int>>& base_vec, int game_view[5][3]) {

    static mt19937 gen(chrono::high_resolution_clock::now().time_since_epoch().count());
    for (int i = 0; i < 5; ++i) {
        if (i >= base_vec.size() || base_vec[i].size() < 3) {
            continue;
        }
        auto row = base_vec[i];
        size_t length_of_wheel = row.size();
        row.push_back(row[0]);
        row.push_back(row[1]);

        uniform_int_distribution<> distrib(0, length_of_wheel-1);
        int random_index = distrib(gen);
        int single_wheel[3] = {row[random_index], row[random_index + 1], row[random_index + 2]};

        for (int j = 0; j < 3; ++j) {
            game_view[i][j] = single_wheel[j];
        }
    }
}

void GetGameLines(int game_view[5][3], int game_lines[10][5], const int pay_lines_array[10][5]) {
    for (size_t line = 0; line < 10; ++line) {
        for (size_t i = 0; i < 5; ++i) {
            game_lines[line][i] = game_view[i][pay_lines_array[line][i]];
        }
    }
}

std::vector<std::pair<int, int>>  analyseArray(const int (&arr)[5]) {
    int wild = 7;
    int consecutive_counts = 0;
    int first_value = arr[0];
    std::vector<std::pair<int, int>> pairVector;
    for (int i=0; i < 5; ++i) {
        if (arr[i] == first_value ||  arr[i] == wild) {
            ++consecutive_counts;
        }
        else {
            break;
        }

    }

    if (consecutive_counts == 5) {
        pairVector.push_back(std::make_pair(consecutive_counts , first_value));
        return pairVector;
    }

    if (consecutive_counts >= 2) {
        pairVector.push_back(std::make_pair(consecutive_counts , first_value));

    }

    consecutive_counts = 0;
    first_value = arr[4];
    for (int i=4 ; i >= 0; --i) {
        if (arr[i] == first_value ||  arr[i] == wild) {
            ++consecutive_counts;
        }
        else {
            break;
        }
    }

    if (consecutive_counts >= 2 ) {
        pairVector.push_back(std::make_pair(consecutive_counts , first_value));
    }

    return pairVector;

}
std::pair<bool, vector<int>> ExpandWilds(int game_view[5][3]) {
    bool oneWild = false;
    vector<int> infoArray;

    for (int i = 0; i < 5; ++i) {
        int num_wilds = 0;
        for (int l = 0; l < 3; ++l) {
            if (game_view[i][l] == 7) {
                ++num_wilds;
            }
        }
//        cout << num_wilds << endl;
        if (num_wilds == 1) {
            oneWild = true;
            infoArray.push_back(i);
            // Assuming you want to modify the current row if exactly one wild is found
            for (int l = 0; l < 3; ++l) {
                game_view[i][l] = 7; // Use assignment operator
            }
        }
    }

    return {oneWild, infoArray};
}

vector<int> FlattenData(const vector<vector<int>>& array) {
    vector<int> ReturnArray;
    for (const auto& thread_wins : array) {
        ReturnArray.insert(ReturnArray.end(), thread_wins.begin(), thread_wins.end());
    }
    return ReturnArray;
}

long long int GetArraySum(vector<int> wins_array) {

    long long int winnigns = std::accumulate(wins_array.begin(), wins_array.end(), 0LL);
    return winnigns;
}

std::pair<double, double> GetStats(const std::vector<int>& wins, long long sum_wins, long long n, double default_bet) {
    if (n <= 1) {
        // Handle the case where n is 1 or less
        throw std::invalid_argument("n must be greater than 1 to calculate variance.");
    }

    double sum_x_sq = 0;
    for (const auto& win : wins) {
        sum_x_sq += pow(static_cast<double>(win) / default_bet, 2) ;
    }

    // Ensure the multiplication does not overflow
    long double sum_wins_sq = pow( sum_wins / default_bet, 2) ;


    double var = (1.0 / (n - 1)) * (sum_x_sq - (sum_wins_sq / static_cast<double>(n)));
    double std_dev = std::sqrt(var);

    return {var, std_dev};
}
std::array<long long int, 4> GetHighWins(const std::vector<int>& flat_array, int default_bet) {
    long long int h1l = static_cast<long long int>(default_bet) * 12;
    long long int h1h = static_cast<long long int>(default_bet) * 40;
    std::array<long long int, 4> win_frequency_count = {0, 0};
    for (const int val : flat_array) {
        if (val < h1h && val >= h1l) {
            ++win_frequency_count[0];
            win_frequency_count[1] += val;
        } else if (val > h1h) {
            ++win_frequency_count[2];
            win_frequency_count[3] += val;
        }
    }

    return win_frequency_count;
}
























