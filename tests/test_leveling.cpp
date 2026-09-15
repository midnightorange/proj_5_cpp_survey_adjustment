#include <cassert>
#include <cmath>
#include <fstream>
#include <iostream>

#include "leveling.h"

static bool near(double a, double b, double eps = 1e-4) {
    return std::fabs(a - b) < eps;
}

int main() {
    std::ifstream fin("data/leveling_example.txt");
    assert(fin && "样例数据文件未找到");

    geo::LevelingNetwork net = geo::parseLeveling(fin);
    assert(net.points.size() == 3);
    assert(net.obs.size() == 3);

    geo::AdjustmentResult r = geo::adjustLeveling(net);

    // 已知：A=10, B=20；观测 A->P +5.000(1km), P->B +5.000(1km), A->P +5.010(2km)
    // 期望：H_P=15.0020, sigma0=0.004472, sigmaH=0.002828
    assert(near(r.heights[0], 15.0020, 1e-4));
    assert(near(r.sigma0, 0.004472, 1e-5));
    assert(near(r.sigmaH[0], 0.002828, 1e-5));

    // 改正数 [0.0020, -0.0020, -0.0080]
    assert(near(r.residuals[0], 0.0020, 1e-4));
    assert(near(r.residuals[1], -0.0020, 1e-4));
    assert(near(r.residuals[2], -0.0080, 1e-4));

    assert(r.redundancy == 2);

    std::cout << "test_leveling OK\n";
    return 0;
}
