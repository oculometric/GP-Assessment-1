#include "point_generator.h"

#include <set>

inline float randomBetween(float min, float max)
{
	return (((float)rand() / (float)RAND_MAX) * (max - min)) + min;
}

bool operator<(const Vector3& a, const Vector3& b)
{
	if (a.x < b.x) return true;
	if (a.x > b.x) return false;
	if (a.y < b.y) return true;
	if (a.y > b.y) return false;
	if (a.z < b.z) return true;
	return false;
}

void generatePoints(unsigned int max_points, unsigned int candidates_per_point, Vector3 max, Vector3 min, Vector3* points_buffer)
{
	std::set<Vector3> points;
	Vector3 true_min = min;
	Vector3 true_max = max;
	if (max.x < min.x)
	{
		true_min.x = max.x;
		true_max.x = min.x;
	}
	if (max.y < min.y)
	{
		true_min.y = max.y;
		true_max.y = min.y;
	}
	if (max.z < min.z)
	{
		true_min.z = max.z;
		true_max.z = min.z;
	}

	for (unsigned int p = 0; p < max_points; p++)
	{
		Vector3 best_candidate_point;
		float best_candidate_distance = 0;
		for (unsigned int i = 0; i < candidates_per_point; i++)
		{
			Vector3 candidate{ randomBetween(true_min.x, true_max.x),randomBetween(true_min.y, true_max.x), randomBetween(true_min.z, true_max.z) };
			float closest = INFINITY;
			for (Vector3 point : points)
			{
				float x_dif = abs(point.x - candidate.x);
				if (x_dif > closest) continue;
				float y_dif = abs(point.y - candidate.y);
				if (y_dif > closest) continue;
				float z_dif = abs(point.z - candidate.z);
				if (z_dif > closest) continue;
				x_dif *= x_dif;
				if (x_dif > closest) continue;
				y_dif *= y_dif;
				if (y_dif > closest) continue;
				z_dif *= z_dif;
				if (z_dif > closest) continue;
				float dif = x_dif + y_dif + z_dif;
				if (dif < closest)
				{
					closest = dif;
				}
			}
			if (closest > best_candidate_distance)
			{
				best_candidate_point = candidate;
				best_candidate_distance = closest;
			}
		}
		points.insert(best_candidate_point);
	}
	
	int i = 0;
	for (Vector3 point : points)
	{
		points_buffer[i] = point;
		i++;
	}
}