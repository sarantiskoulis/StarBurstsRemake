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
               ,int symbol_points[7][5],
               vector<int>& total_wins) {


    for (int k = start; k < end; ++k) {
        bool Spin = true;
        std::set<std::pair<int, int>> uniquePairs;
        int line_point = 0;

        int game_view[5][3] = {0};
        GetGameView(base_vec, game_view);

        while (Spin) {
            vector<int> indexWild = ExpandWilds(game_view);
            if (indexWild.size() == 0) {
                Spin = false;
            }

            for (int i = 0; i < 5 ; ++i) {
                for (int  j = 0; j < 3 ; ++j) {
                    cout << game_view[i][j] << " ";
                }
                cout << endl;
            }

            int game_lines[10][5] = {0};
            GetGameLines(game_view, game_lines, pay_lines);


            for (int i = 0; i < 10; ++i) {
                array<int, 2> result = analyseArray(game_lines[i]);
                int occurance = result[0];
                int symbol = result[1];
                if (symbol >= 0 && symbol <= 9 && occurance > 2) {

                    int normal_points = payTable.table[symbol][occurance - 1];
                    symbol_points[symbol][occurance - 1] += normal_points;

                    line_point += normal_points;

//                    auto pair = std::make_pair(symbol, occurance - 1);
//                    if (uniquePairs.find(pair) == uniquePairs.end()) {
//                        symbol_rate[symbol][occurance - 1] += 1;
//                        uniquePairs.insert(pair);
//                    }

                }
            }
        }
        total_wins.push_back(line_point);

    }
}

int main() {
    YAML::Node config = YAML::LoadFile("C:\\Users\\sarantis\\CLionProjects\\StarburstRemake\\config.yaml");
    const std::vector<std::vector<int>> base_vec = LoadWheelsData(config["wheels"]["wheel_base"]);

    PayTable paytable = GetPayTableFromYaml(config);
//    paytable.PrintTable();

    const YAML::Node& payLines = config["pay_lines"];
    int pay_lines[10][5] = {0};
    LoadPayLines(payLines, pay_lines);




    int default_bet = 30;
    const int num_threads = 1;  //std::thread::hardware_concurrency();
    long long int num_elements = 1;
    const int elements_per_thread = num_elements / num_threads;

    vector<std::thread> threads;


    vector<vector<int>> total_wins(num_threads);
    int symbol_points[num_threads][7][5] = {0};


    for (int i = 0; i < num_threads; ++i) {
        int start = i * elements_per_thread;
        int end = (i == num_threads - 1) ? num_elements : start + elements_per_thread;
        threads.emplace_back(
                game_runs,start, end,
                std::ref(pay_lines),
                std::ref(base_vec),
                std::ref(paytable),
                std::ref(symbol_points[i]),
                std::ref(total_wins[i])
                             );

    }
    for (auto &thread: threads) {
        thread.join();
    }

    // POINTS
    vector<int> total_wins_values =  FlattenData(total_wins);
    cout << "Size of: " << total_wins_values.size() << endl;
    long long int total_winnings = GetArraySum(total_wins_values);
    cout << "Total Wins: " << total_winnings << endl;

    long long int symbol_points_t[7][5] = {};
    for (int i = 0; i< num_threads; ++i) {
        for (int j = 0; j < 7; ++ j) {
            for (int k = 0; k < 5; ++k) {
                symbol_points_t[j][k] += symbol_points[i][j][k];
            }
        }
    }

    for (int j = 0; j < 7; ++ j) {
        for (int k = 0; k < 5; ++k) {
            cout << symbol_points_t[j][k] << " ";
        }
        cout << endl;
    }



    return 0;
}
