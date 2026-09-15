#include "matrix.h"

#include <algorithm>
#include <cmath>
#include <utility>

namespace geo {

Matrix::Matrix(std::size_t rows, std::size_t cols, double value)
    : rows_(rows), cols_(cols), data_(rows * cols, value) {}

Matrix::Matrix(std::initializer_list<std::initializer_list<double>> rows) {
    rows_ = rows.size();
    cols_ = rows.size() == 0 ? 0 : rows.begin()->size();
    data_.reserve(rows_ * cols_);
    for (const auto& r : rows) {
        if (r.size() != cols_) {
            throw std::invalid_argument("Matrix 各行列数不一致");
        }
        data_.insert(data_.end(), r.begin(), r.end());
    }
}

double& Matrix::operator()(std::size_t r, std::size_t c) {
    return data_[r * cols_ + c];
}

const double& Matrix::operator()(std::size_t r, std::size_t c) const {
    return data_[r * cols_ + c];
}

Matrix Matrix::transpose() const {
    Matrix t(cols_, rows_);
    for (std::size_t i = 0; i < rows_; ++i) {
        for (std::size_t j = 0; j < cols_; ++j) {
            t(j, i) = (*this)(i, j);
        }
    }
    return t;
}

Matrix Matrix::operator+(const Matrix& other) const {
    if (rows_ != other.rows_ || cols_ != other.cols_) {
        throw std::invalid_argument("矩阵维度不匹配：加法");
    }
    Matrix m(rows_, cols_);
    for (std::size_t i = 0; i < data_.size(); ++i) {
        m.data_[i] = data_[i] + other.data_[i];
    }
    return m;
}

Matrix Matrix::operator-(const Matrix& other) const {
    if (rows_ != other.rows_ || cols_ != other.cols_) {
        throw std::invalid_argument("矩阵维度不匹配：减法");
    }
    Matrix m(rows_, cols_);
    for (std::size_t i = 0; i < data_.size(); ++i) {
        m.data_[i] = data_[i] - other.data_[i];
    }
    return m;
}

Matrix Matrix::operator*(const Matrix& other) const {
    if (cols_ != other.rows_) {
        throw std::invalid_argument("矩阵维度不匹配：乘法");
    }
    Matrix m(rows_, other.cols_);
    for (std::size_t i = 0; i < rows_; ++i) {
        for (std::size_t k = 0; k < cols_; ++k) {
            double a = (*this)(i, k);
            if (a == 0.0) {
                continue;
            }
            for (std::size_t j = 0; j < other.cols_; ++j) {
                m(i, j) += a * other(k, j);
            }
        }
    }
    return m;
}

Matrix Matrix::operator*(double scalar) const {
    Matrix m(rows_, cols_);
    for (std::size_t i = 0; i < data_.size(); ++i) {
        m.data_[i] = data_[i] * scalar;
    }
    return m;
}

Matrix Matrix::inverse() const {
    if (rows_ != cols_ || rows_ == 0) {
        throw std::invalid_argument("仅方阵可求逆");
    }
    const std::size_t n = rows_;

    // 增广矩阵 [A | I]
    Matrix aug(n, 2 * n);
    for (std::size_t i = 0; i < n; ++i) {
        for (std::size_t j = 0; j < n; ++j) {
            aug(i, j) = (*this)(i, j);
        }
        aug(i, n + i) = 1.0;
    }

    const double eps = 1e-12;
    for (std::size_t c = 0; c < n; ++c) {
        // 列主元选取
        std::size_t pivot = c;
        double best = std::fabs(aug(c, c));
        for (std::size_t r = c + 1; r < n; ++r) {
            double v = std::fabs(aug(r, c));
            if (v > best) {
                best = v;
                pivot = r;
            }
        }
        if (best < eps) {
            throw std::runtime_error("矩阵奇异，无法求逆");
        }
        if (pivot != c) {
            for (std::size_t j = 0; j < 2 * n; ++j) {
                std::swap(aug(c, j), aug(pivot, j));
            }
        }

        double d = aug(c, c);
        for (std::size_t j = 0; j < 2 * n; ++j) {
            aug(c, j) /= d;
        }

        for (std::size_t r = 0; r < n; ++r) {
            if (r == c) {
                continue;
            }
            double f = aug(r, c);
            if (f == 0.0) {
                continue;
            }
            for (std::size_t j = 0; j < 2 * n; ++j) {
                aug(r, j) -= f * aug(c, j);
            }
        }
    }

    Matrix inv(n, n);
    for (std::size_t i = 0; i < n; ++i) {
        for (std::size_t j = 0; j < n; ++j) {
            inv(i, j) = aug(i, n + j);
        }
    }
    return inv;
}

Matrix Matrix::solve(const Matrix& b) const {
    return inverse() * b;
}

Matrix columnVector(const std::vector<double>& v) {
    Matrix m(v.size(), 1);
    for (std::size_t i = 0; i < v.size(); ++i) {
        m(i, 0) = v[i];
    }
    return m;
}

}  // namespace geo
