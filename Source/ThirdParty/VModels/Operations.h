#pragma once
#include "Model.h"
#include <vector>
#include <cmath>
#include <iostream>

namespace VModel
{
    static class Operations {
    public:
        //R-функциональное логическое сложение
        static Model Sum(Model& m1, Model& m2);
        //R-функциональное логическое произведение
        static Model Comp(Model& m1, Model& m2);
        //Инвертирование функции
        static void Invert(Model& m);
        //Создание копии инвертированной функции
        static Model NewInvert(Model& m);
    };
}