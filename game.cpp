//
// Created by sarantis on 17/11/2023.
//

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

void GetGameView(const std::vector<std::vector<int>>& base_vec, int game_view[5][3]) {
    std::cout << "AHHH" << std::endl;

    static mt19937 gen(std::chrono::high_resolution_clock::now().time_since_epoch().count());

    for (size_t i = 0; i < 5; ++i) {
        if (i >= base_vec.size() || base_vec[i].size() < 3) {
            continue;
        }
        auto row = base_vec[i];
        size_t length_of_wheel = row.size();
        row.push_back(row[0]);
        row.push_back(row[1]);

        uniform_int_distribution<> distrib(0, length_of_wheel - 1);
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

bool ExpandWilds(int game_view[5][3]) {
    bool Respin = false;
    for (int i = 0; i < 5; ++i) {
        for (int l = 0; l < 3; ++l) {
            if (game_view[i][l] == 7) {
                Respin = true;
                for (int m = 0; m < 3; ++m) {
                    game_view[i][m] = 7;
                }
                break;
            }
        }
    }
    return Respin;
}