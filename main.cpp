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
#include "misc.h"

using std::accumulate;
using std::cout;
using std::random_device;
using std::mt19937;
using std::uniform_int_distribution;

using namespace std;



using namespace std;int main() {
    YAML::Node config = YAML::LoadFile("C:\\Users\\sarantis\\CLionProjects\\game3\\config.yaml");
    const std::vector<std::vector<int>> feat_vec = LoadWheelsData(config["wheels"]["wheel_feature"]);
    const std::vector<std::vector<int>> base_vec = LoadWheelsData(config["wheels"]["wheel_base"]);

    const YAML::Node& payTable = config["pay_table"];
    const YAML::Node& payLines = config["pay_lines"];

    int payTableArray[12][5] = {0};
    fillPayTable(payTable, payTableArray);

    int pay_lines_array[25][5] = {0};
    LoadPayLines(payLines, pay_lines_array);

    return 0;
}
