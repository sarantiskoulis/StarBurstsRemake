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

std::array<int, 2>  analyseArray(const int (&arr)[5]) {
    int wild = 7;
    int consecutive_counts = 0;
    int first_value = arr[0];

    for (int i=0; i < 5; ++i) {
        if (arr[i] == first_value ||  arr[i] == wild) {
            ++consecutive_counts;
        }
        else {
            break;
        }
    }

    if (consecutive_counts <= 2 ) {
        consecutive_counts = 0;
        first_value = arr[4];
        for (int i=5 ; i >= 0; --i) {
            if (arr[i] == first_value ||  arr[i] == wild) {
                ++consecutive_counts;
            }
            else {
                break;
            }
        }
    }
    if (consecutive_counts > 2 ) {
        return  {consecutive_counts, first_value};
    }

    return {-1,-1};

}

std::vector<int> ExpandWilds(int game_view[5][3]) {
    std::vector<int> indexWild;
    for (int i = 0; i < 5; ++i) {
        for (int l = 0; l < 3; ++l) {
            if (game_view[i][l] == 7) {
                indexWild.push_back(i);
                for (int m = 0; m < 3; ++m) {
                    game_view[i][m] = 7;
                }
                break;

            }
        }
    }
    return indexWild;
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

























