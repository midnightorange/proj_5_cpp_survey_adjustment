#pragma once
#include <istream>
#include <string>
#include <vector>

#include "matrix.h"

namespace geo {

// 已知点 / 待定点
struct Point {
    std::string name;
    double height = 0.0;  // 已知点高程；待定点为 0
    bool known = false;
};

// 高差观测值（h_to - h_from）
struct LevelingObs {
    int from = -1;          // 起点在 points 中的索引
    int to = -1;            // 终点索引
    double dh = 0.0;        // 观测高差 (m)
    double lengthKm = 1.0;  // 路线长度 (km)，定权 p = 1/S
};

struct LevelingNetwork {
    std::vector<Point> points;
    std::vector<LevelingObs> obs;
};

// 平差结果
struct AdjustmentResult {
    std::vector<double> heights;    // 待定点平差高程（按待定点出现顺序）
    std::vector<double> residuals;  // 各观测值改正数 (m)
    double sigma0 = 0.0;            // 单位权中误差（1 km 路线）
    std::vector<double> sigmaH;     // 待定点高程中误差 (m)
    int redundancy = 0;             // 多余观测数 m - u
};

// 解析标准水准网数据（方案 §9）
LevelingNetwork parseLeveling(std::istream& in);

// 间接平差（参数平差）
AdjustmentResult adjustLeveling(const LevelingNetwork& net);

}  // namespace geo
