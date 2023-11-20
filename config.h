//
// Created by sarantis on 13/11/2023.
//

#ifndef GAME3_MISC_H
#define GAME3_MISC_H
#include "vector"
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


#include <unordered_map>
#include <unordered_set>

struct PayTable {
    int table[7][5];
    void PrintTable() {
        int cols = 7;
        int rows = 5;
        for (int i = 0; i < cols; ++i) {
            for (int j = 0; j < rows; ++j) {
                std::cout << table[i][j] << " ";
            }
            std::cout << std::endl;
        }
    }
};


std::vector<std::vector<int>> LoadWheelsData(const YAML::Node &node);
PayTable GetPayTableFromYaml(const YAML::Node& yaml);
void LoadPayLines(const YAML::Node &node, int pay_lines_arry[25][5]);
void GetGameView(const std::vector<std::vector<int>>& base_vec, int game_view[5][3]);
void GetGameLines(int game_view[5][3], int game_lines[10][5], const int pay_lines_array[10][5]);
std::vector<std::pair<int, int>> analyseArray(const int (&arr)[5]);
std::pair<bool, std::vector<int>> ExpandWilds(int game_view[5][3]);
std::vector<int> FlattenData(const std::vector<std::vector<int>>& array);
long long int GetArraySum(std::vector<int> wins_array);
std::pair<double, double> GetStats(const std::vector<int>& wins, long long sum_wins, long long n, double default_bet);
std::array<long long int, 4> GetHighWins(const std::vector<int>& flat_array, int default_bet);
#endif //GAME3_MISC_H
