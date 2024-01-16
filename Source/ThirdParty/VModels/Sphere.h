#include "Model.h"

namespace VModel
{
    class Sphere : public Model
    {
    private:
        double SphereFunc(double X1, double Y1, double Z1, double R1, double x, double y, double z)
        {
            return pow(x - X1, 2) + pow(y - Y1, 2) + pow(z - Z1, 2) - pow(R1, 2);
        }
    public:
        Sphere(double radius, Point center, Point areaSize)
        {
            AreaSize = areaSize;

            for (int x = 0; x < AreaSize.x; x++)
                for (int y = 0; y < AreaSize.y; y++)
                    for (int z = 0; z < AreaSize.z; z++)
                        FuncResult.push_back(SphereFunc(center.x, center.y, center.z, radius, x, y, z));
        }
    };
}