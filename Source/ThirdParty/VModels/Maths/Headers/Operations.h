#pragma once
#include "Model.h"
#include <vector>
#include <cmath>
#include <iostream>

namespace vmodel
{
    static class Operations {
    public:
        static Model sum(Model& m1, Model& m2, double alfa);
        static Model comp(Model& m1, Model& m2, double alfa);
        static Model homotopy(Model& m1, Model& m2, double t);
        static void invert(Model& m);
        static Model new_invert(Model& m);
    };
}