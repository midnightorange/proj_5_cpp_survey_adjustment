#pragma once
#include <cstddef>
#include <initializer_list>
#include <stdexcept>
#include <string>
#include <vector>

namespace geo {

// 稠密矩阵（double，行主序存储）
class Matrix {
public:
    Matrix() = default;
    Matrix(std::size_t rows, std::size_t cols, double value = 0.0);
    Matrix(std::initializer_list<std::initializer_list<double>> rows);

    std::size_t rows() const { return rows_; }
    std::size_t cols() const { return cols_; }
    bool empty() const { return rows_ == 0 || cols_ == 0; }

    double& operator()(std::size_t r, std::size_t c);
    const double& operator()(std::size_t r, std::size_t c) const;

    Matrix transpose() const;

    Matrix operator+(const Matrix& other) const;
    Matrix operator-(const Matrix& other) const;
    Matrix operator*(const Matrix& other) const;  // 矩阵乘法
    Matrix operator*(double scalar) const;

    // 高斯-约当求逆（含列主元），奇异时抛 std::runtime_error
    Matrix inverse() const;
    // 解线性方程组 A x = b（复用 inverse）
    Matrix solve(const Matrix& b) const;

private:
    std::size_t rows_ = 0;
    std::size_t cols_ = 0;
    std::vector<double> data_;
};

// 由 std::vector 构造列向量 (n x 1)
Matrix columnVector(const std::vector<double>& v);

}  // namespace geo
