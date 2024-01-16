#pragma once
namespace VModel
{
	struct Point final
	{
	public:
		int x, y, z;

		constexpr Point() noexcept : Point(0, 0, 0) { }
		constexpr Point(int x, int y) noexcept : Point(x, y, 0) { }
		constexpr Point(int x, int y, int z) noexcept
		{
			this->x = x;
			this->y = y;
			this->z = z;
		}
		constexpr Point(const Point& p) noexcept : Point(p.x, p.y, p.z) { }


		~Point() noexcept = default;
	};
	static const bool operator==(const Point& lhs, const Point& rhs) {
		return lhs.x == rhs.x && lhs.y == rhs.y && lhs.z == rhs.z;
	}
	static const bool operator!=(const Point& lhs, const Point& rhs) {
		return !(lhs == rhs);
	}
}