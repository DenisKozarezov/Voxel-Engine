#include "Model.h"

namespace VModel
{
    class Torus : public Model
    {
    private:
        double TorusFunc(double X1, double Y1, double Z1, double R1, double r1, double x, double y, double z)
        {
            return pow(sqrt(pow(x - X1, 2) + pow(y - Y1, 2)) - R1, 2) + pow(z - Z1, 2) - pow(r1, 2);
        }
    public:
        Torus(double R, double r, Point center, Point areaSize)
        {
            AreaSize = areaSize;

            for (int x = 0; x < AreaSize.x; x++)
                for (int y = 0; y < AreaSize.y; y++)
                    for (int z = 0; z < AreaSize.z; z++)
                        FuncResult.push_back(TorusFunc(center.x, center.y, center.z, R, r, x, y, z));
        }
    };
}