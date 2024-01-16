#pragma once
#include "Model.h"
#include <vector>
#include <cmath>
#include <iostream>

namespace VModel
{
    static class Operations {
    public:
        //R-�������������� ���������� ��������
        static Model Sum(Model& m1, Model& m2);
        //R-�������������� ���������� ������������
        static Model Comp(Model& m1, Model& m2);
        //�������������� �������
        static void Invert(Model& m);
        //�������� ����� ��������������� �������
        static Model NewInvert(Model& m);
    };
}