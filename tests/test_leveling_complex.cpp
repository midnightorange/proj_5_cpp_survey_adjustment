#include <cassert>
#include <cmath>
#include <fstream>
#include <iostream>

#include "leveling.h"

static bool near(double a, double b, double eps = 1e-4) {
    return std::fabs(a - b) < eps;
}

int main() {
    std::ifstream fin("data/leveling_complex.txt");
    assert(fin && "复杂算例数据文件未找到");

    geo::LevelingNetwork net = geo::parseLeveling(fin);
    assert(net.points.size() == 6);
    assert(net.obs.size() == 8);

    geo::AdjustmentResult r = geo::adjustLeveling(net);

    // 待定点高程（按待定点出现顺序 P1, P2, P3）
    assert(r.heights.size() == 3);
    assert(near(r.heights[0], 12.0003, 1e-4));
    assert(near(r.heights[1], 17.9984, 1e-4));
    assert(near(r.heights[2], 14.0000, 1e-4));

    // 精度评定
    assert(near(r.sigma0, 0.001451, 1e-5));
    assert(near(r.sigmaH[0], 0.0008, 1e-4));
    assert(near(r.sigmaH[1], 0.0010, 1e-4));
    assert(near(r.sigmaH[2], 0.0010, 1e-4));

    // 观测值改正数
    assert(near(r.residuals[0], -0.0007, 1e-4));
    assert(near(r.residuals[1], 0.0001, 1e-4));
    assert(near(r.residuals[2], 0.0004, 1e-4));
    assert(near(r.residuals[3], 0.0005, 1e-4));
    assert(near(r.residuals[4], 0.0010, 1e-4));
    assert(near(r.residuals[5], -0.0030, 1e-4));
    assert(near(r.residuals[6], 0.0007, 1e-4));
    assert(near(r.residuals[7], 0.0023, 1e-4));

    assert(r.redundancy == 5);

    std::cout << "test_leveling_complex OK\n";
    return 0;
}
