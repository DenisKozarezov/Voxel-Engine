#include "./Headers/Operations.h"

namespace vmodel
{
    Model Operations::sum(Model& m1, Model& m2, double alfa)
    {
        if (m1.size() != m2.size())
            throw std::invalid_argument("Результирующие функций имеют разные области");

        Model result = Model(m1.size());

        for (int i = 0; i < m1.value()->size(); ++i)
            if(alfa == 1)
                result.value()->push_back(.5 * (m1[i] + m2[i] + abs(m1[i] - m2[i])));
            else
                result.value()->push_back((m1[i] + m2[i] + sqrt(m1[i] * m1[i] + m2[i] * m2[i] - 2 * alfa * m1[i] * m2[i])) / (1 + alfa));
        
        return result;
    }

    Model Operations::comp(Model& m1, Model& m2, double alfa)
    {
        if (m1.size() != m2.size())
            throw std::invalid_argument("Результирующие функций имеют разные области");

        Model result = Model(m1.size());

        for (int i = 0; i < m1.value()->size(); i++)
            if (alfa == 1)
                result.value()->push_back(.5 * (m1[i] + m2[i] - abs(m1[i] - m2[i])));
            else
                result.value()->push_back((m1[i] + m2[i] - sqrt(m1[i] * m1[i] + m2[i] * m2[i] - 2 * alfa * m1[i] * m2[i])) / (1 + alfa));

        return result;
    }

    Model Operations::homotopy(Model& m1, Model& m2, double t) {
        if (m1.size() != m2.size())
            throw std::invalid_argument("Результирующие функций имеют разные области");

        Model result = Model(m1.size());

        for (int i = 0; i < m1.value()->size(); i++)
            result.value()->push_back(m1[i] * (1 - t) + m2[i] * t);

        return result;
    }

    void Operations::invert(Model& m)
    {
        for (int i = 0; i < m.value()->size(); i++)
            m[i] = -m[i];
    }

    Model Operations::new_invert(Model& m)
    {
        Model result = Model(m.size());

        for (double var : *m.value()) {
            result.value()->push_back(-var);
        }
        return result;
    }
}