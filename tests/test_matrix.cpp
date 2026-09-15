#include <cassert>
#include <cmath>
#include <iostream>

#include "matrix.h"

static bool near(double a, double b, double eps = 1e-9) {
    return std::fabs(a - b) < eps;
}

int main() {
    // 矩阵乘法
    geo::Matrix A = {{1, 2}, {3, 4}};
    geo::Matrix B = {{5, 6}, {7, 8}};
    geo::Matrix C = A * B;
    assert(near(C(0, 0), 19));
    assert(near(C(0, 1), 22));
    assert(near(C(1, 0), 43));
    assert(near(C(1, 1), 50));

    // 转置
    geo::Matrix At = A.transpose();
    assert(near(At(0, 1), 3));
    assert(near(At(1, 0), 2));

    // 求逆
    geo::Matrix inv = A.inverse();
    geo::Matrix I = A * inv;
    assert(near(I(0, 0), 1, 1e-6));
    assert(near(I(0, 1), 0, 1e-6));
    assert(near(I(1, 0), 0, 1e-6));
    assert(near(I(1, 1), 1, 1e-6));

    // 解方程组 [2 1; 1 3] x = [5; 10] -> x = [1; 3]
    geo::Matrix M = {{2, 1}, {1, 3}};
    geo::Matrix b = geo::columnVector({5, 10});
    geo::Matrix x = M.solve(b);
    assert(near(x(0, 0), 1, 1e-6));
    assert(near(x(1, 0), 3, 1e-6));

    std::cout << "test_matrix OK\n";
    return 0;
}
