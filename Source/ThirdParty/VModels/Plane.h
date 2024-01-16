#include "Model.h"

namespace VModel
{
    class Plane : public Model
    {
    private:
        double PlaneFunc(Point p1, Point p2, Point p3, double x, double y, double z)
        {
            return (x - p1.x) * ((p2.y - p1.y) * (p3.z - p1.z) - (p2.z - p1.z) * (p3.y - p1.y))
                - (y - p1.y) * ((p2.x - p1.x) * (p3.z - p1.z) - (p2.z - p1.z) * (p3.x - p1.x))
                + (z - p1.z) * ((p2.x - p1.x) * (p3.y - p1.y) - (p2.y - p1.y) * (p3.x - p1.x));
        }
    public:
        Plane(Point p1, Point p2, Point p3, Point areaSize)
        {
            AreaSize = areaSize;

            for (int x = 0; x < AreaSize.x; x++)
                for (int y = 0; y < AreaSize.y; y++)
                    for (int z = 0; z < AreaSize.z; z++)
                        FuncResult.push_back(PlaneFunc(p1, p2, p3, x, y, z));
        }
    };
}