#include "yee1dVacuum.h"

namespace Vacuum {
Data Compute(size_t N,size_t K,double dt,double dz,double k)
{
    Data array{};
    array.push_back(InitialValues(K,dz,dt,k));
    for (size_t i = 0; i < N - 1; ++i) {
        array.push_back(IntegrateStep(array[i],K,c * dt/dz));
    }

    return(Data{array});
}
}
