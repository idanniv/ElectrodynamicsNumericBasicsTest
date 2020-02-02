#include "borisPush.h"

namespace Boris {
Array Compute(size_t N,double delta,double a,StateVector InitialValues,StateVector EMfield)
{
    Array states{};
    states.reserve(N);
    states.push_back(InitialState(InitialValues,delta));

    for (size_t i = 0; i < N - 1; i++) {
        states.push_back(BorisPush(states[i],delta,a,EMfield));
    }
    return(states);
}
}
