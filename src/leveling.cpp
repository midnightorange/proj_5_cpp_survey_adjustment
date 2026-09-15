#include "leveling.h"

#include <cmath>
#include <stdexcept>
#include <unordered_map>

namespace geo {

LevelingNetwork parseLeveling(std::istream& in) {
    LevelingNetwork net;
    int nKnown = 0, nUnknown = 0, mObs = 0;
    if (!(in >> nKnown >> nUnknown >> mObs)) {
        throw std::runtime_error("无法读取数据头（已知点数 待定点数 观测高差数）");
    }

    std::unordered_map<std::string, int> index;

    // 已知点
    for (int i = 0; i < nKnown; ++i) {
        Point p;
        if (!(in >> p.name >> p.height)) {
            throw std::runtime_error("已知点数据不足");
        }
        p.known = true;
        index[p.name] = static_cast<int>(net.points.size());
        net.points.push_back(p);
    }

    // 待定点名称在观测值中首次出现时创建
    auto pointId = [&](const std::string& name) -> int {
        auto it = index.find(name);
        if (it != index.end()) {
            return it->second;
        }
        Point p;
        p.name = name;
        p.known = false;
        int id = static_cast<int>(net.points.size());
        index[name] = id;
        net.points.push_back(p);
        return id;
    };

    // 观测值（起点 终点 高差 路线长度）
    for (int i = 0; i < mObs; ++i) {
        std::string from, to;
        double dh = 0.0, s = 1.0;
        if (!(in >> from >> to >> dh >> s)) {
            throw std::runtime_error("观测值数据不足");
        }
        LevelingObs o;
        o.from = pointId(from);
        o.to = pointId(to);
        o.dh = dh;
        o.lengthKm = s;
        net.obs.push_back(o);
    }

    int unknownSeen = 0;
    for (const auto& p : net.points) {
        if (!p.known) {
            ++unknownSeen;
        }
    }
    if (unknownSeen != nUnknown) {
        throw std::runtime_error("待定点数与观测值中出现的待定点不一致");
    }

    return net;
}

AdjustmentResult adjustLeveling(const LevelingNetwork& net) {
    const std::size_t m = net.obs.size();

    // 待定点编号 -> 参数下标
    std::vector<int> param(net.points.size(), -1);
    std::vector<std::size_t> unknownIdx;
    for (std::size_t i = 0; i < net.points.size(); ++i) {
        if (!net.points[i].known) {
            param[i] = static_cast<int>(unknownIdx.size());
            unknownIdx.push_back(i);
        }
    }
    const std::size_t u = unknownIdx.size();

    AdjustmentResult res;
    res.redundancy = static_cast<int>(m) - static_cast<int>(u);

    if (u == 0) {
        return res;  // 无待定点
    }

    Matrix B(m, u, 0.0);
    std::vector<double> l(m, 0.0);
    std::vector<double> w(m, 0.0);  // sqrt(p)

    for (std::size_t k = 0; k < m; ++k) {
        const LevelingObs& o = net.obs[k];
        double s = o.lengthKm > 0.0 ? o.lengthKm : 1.0;
        w[k] = std::sqrt(1.0 / s);

        double predictedKnown = 0.0;
        // 终点（高差 = 终点高程 - 起点高程，系数 +1）
        if (net.points[o.to].known) {
            predictedKnown += net.points[o.to].height;
        } else {
            B(k, param[o.to]) += 1.0;
        }
        // 起点（系数 -1）
        if (net.points[o.from].known) {
            predictedKnown -= net.points[o.from].height;
        } else {
            B(k, param[o.from]) -= 1.0;
        }

        l[k] = o.dh - predictedKnown;
    }

    // 按 sqrt(p) 缩放，等价于加权最小二乘
    for (std::size_t k = 0; k < m; ++k) {
        for (std::size_t j = 0; j < u; ++j) {
            B(k, j) *= w[k];
        }
        l[k] *= w[k];
    }

    Matrix Bt = B.transpose();
    Matrix N = Bt * B;
    Matrix t = Bt * columnVector(l);
    Matrix x = N.solve(t);

    Matrix vScaled = B * x - columnVector(l);

    double vtpv = 0.0;
    for (std::size_t k = 0; k < m; ++k) {
        vtpv += vScaled(k, 0) * vScaled(k, 0);
    }

    res.heights.resize(u);
    for (std::size_t j = 0; j < u; ++j) {
        res.heights[j] = x(j, 0);
    }

    res.residuals.resize(m);
    for (std::size_t k = 0; k < m; ++k) {
        res.residuals[k] = vScaled(k, 0) / w[k];
    }

    if (res.redundancy > 0) {
        res.sigma0 = std::sqrt(vtpv / res.redundancy);
    }

    Matrix Q = N.inverse();
    res.sigmaH.resize(u);
    for (std::size_t j = 0; j < u; ++j) {
        res.sigmaH[j] = res.sigma0 * std::sqrt(Q(j, j));
    }

    return res;
}

}  // namespace geo
