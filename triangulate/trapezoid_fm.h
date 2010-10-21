#ifndef TRIANGULATE_TRAPEZOID_FM_H
#define TRIANGULATE_TRAPEZOID_FM_H

#include <vector>
#include "types.h"

std::vector<TVertex*> trapezoid_decompose(Polygon const &p);
Triangulation triangulate_trapezoids(std::vector<TVertex*> &t);

/*
Local Variables:
mode:c++
End:
*/
#endif /* TRIANGULATE_TRAPEZOID_FM_H */
