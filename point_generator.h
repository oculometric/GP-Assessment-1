#pragma once

#include "vector3.h"

// randomly distribute points using Mitchell's Best Candidate algorithm
void generatePoints(unsigned int max_points, unsigned int candidates_per_point, Vector3 max, Vector3 min, Vector3* points_buffer);