//
// Created by sarantis on 13/11/2023.
//
#include "misc.h"
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
using namespace std;

std::vector<std::vector<int>> LoadWheelsData(const YAML::Node &node) {
    std::vector<std::vector<int>> wheelsData(25);
    for (const auto &wheel : node) {
        std::vector<int> wheelVec;
        for (const auto &value : wheel) {
            wheelVec.push_back(value.as<int>());

        }
        wheelsData.push_back(wheelVec);
    }
    return wheelsData;
}

void fillPayTable(const YAML::Node& payTableNode, int payTableArray[12][5]) {
    for (const auto& item : payTableNode) {
        int index = item.first.as<int>();
        std::vector<int> points = item.second[1].as<std::vector<int>>();

        if (index < 12 && points.size() == 5) {
            for (int j = 0; j < 5; ++j) {
                payTableArray[index][j] = points[j];
            }
        }
    }
}
void LoadPayLines(const YAML::Node &node, int pay_lines_arry[25][5]) {
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


