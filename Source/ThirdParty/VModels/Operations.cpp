#include "Operations.h"

namespace VModel
{
    Model Operations::Sum(Model& m1, Model& m2)
    {
        if (m1.Size() != m2.Size())
            throw std::invalid_argument("Результирующие функций имеют разные области");

        Model result = Model(m1.Size());

        for (int i = 0; i < m1.Value()->size(); ++i) {
            result.Value()->push_back(m1[i] + m2[i] + sqrt(m1[i] * m1[i] + m2[i] * m2[i]));
        }
        return result;
    }

    Model Operations::Comp(Model& m1, Model& m2)
    {
        if (m1.Size() != m2.Size())
            throw std::invalid_argument("Результирующие функций имеют разные области");

        Model result = Model(m1.Size());

        for (int i = 0; i < m1.Value()->size(); i++)
            result.Value()->push_back(m1[i] + m2[i] - sqrt(m1[i] * m1[i] + m2[i] * m2[i]));

        return result;
    }

    void Operations::Invert(Model& m)
    {
        for (int i = 0; i < m.Value()->size(); i++)
            m[i] = -m[i];
    }

    Model Operations::NewInvert(Model& m)
    {
        Model result = Model(m.Size());

        for (double var : *m.Value()) {
            result.Value()->push_back(-var);
        }
        return result;
    }
}