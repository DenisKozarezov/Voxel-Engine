#pragma once
#include "Point.h"
#include <vector>
#include <cmath>

namespace VModel {
    class Model {
    protected:
        std::vector<double> FuncResult;
        Point AreaSize;

    public:
        std::vector<double>* Value() { return &FuncResult; }
        const Point& Size() const { return AreaSize; }

        Model() = default;
        Model(Point areaSize) : AreaSize(areaSize) {}

        double& operator[](int index) { return FuncResult[index]; }

		std::vector<Point> GetPoints();
    };
}