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

std::vector<std::vector<int>> LoadWheelsData(const YAML::Node &node);
void fillPayTable(const YAML::Node& payTableNode, int payTableArray[12][5]);
void LoadPayLines(const YAML::Node &node, int pay_lines_arry[25][5]);
#endif //GAME3_MISC_H
