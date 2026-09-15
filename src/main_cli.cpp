#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>

#ifdef _WIN32
#include <windows.h>
#endif

#include "leveling.h"

int main(int argc, char **argv)
{
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8); // 控制台以 UTF-8 输出中文报告
#endif

    std::string path = "data/leveling_example.txt";
    if (argc > 1)
    {
        path = argv[1];
    }

    std::ifstream fin(path);
    if (!fin)
    {
        std::cerr << "无法打开文件：" << path << "\n";
        return 1;
    }

    geo::LevelingNetwork net;
    try
    {
        net = geo::parseLeveling(fin);
    }
    catch (const std::exception &e)
    {
        std::cerr << "解析失败：" << e.what() << "\n";
        return 1;
    }

    geo::AdjustmentResult r;
    try
    {
        r = geo::adjustLeveling(net);
    }
    catch (const std::exception &e)
    {
        std::cerr << "平差失败：" << e.what() << "\n";
        return 1;
    }

    std::cout << std::fixed << std::setprecision(4);
    std::cout << "==== 水准网间接平差结果 ====\n\n";

    std::cout << "待定点高程：\n";
    std::size_t ui = 0;
    for (const auto &p : net.points)
    {
        if (p.known)
        {
            continue;
        }
        std::cout << "  " << p.name << "  " << r.heights[ui] << " m";
        if (!r.sigmaH.empty())
        {
            std::cout << "  ±" << r.sigmaH[ui] << " m";
        }
        std::cout << "\n";
        ++ui;
    }

    std::cout << "\n观测值改正数：\n";
    for (std::size_t k = 0; k < net.obs.size(); ++k)
    {
        const auto &o = net.obs[k];
        std::cout << "  #" << (k + 1) << "  "
                  << net.points[o.from].name << " -> " << net.points[o.to].name
                  << "  " << std::showpos << r.residuals[k] << std::noshowpos
                  << " m\n";
    }

    std::cout << "\n单位权中误差（1 km 路线）：" << r.sigma0 << " m\n";
    std::cout << "多余观测数：" << r.redundancy << "\n";
    return 0;
}
