#include "Model.h"

namespace VModel
{
	std::vector<Point> Model::GetPoints()
	{
        std::vector<Point> result;

        int it = 0;
        for (int x = 0; x < AreaSize.x; x++)
            for (int y = 0; y < AreaSize.y; y++)
                for (int z = 0; z < AreaSize.z; z++) {
                    if (FuncResult[it] <= 0)
                        result.push_back(Point(x, y, z));
                    it++;
                }

        return result;
	}
}