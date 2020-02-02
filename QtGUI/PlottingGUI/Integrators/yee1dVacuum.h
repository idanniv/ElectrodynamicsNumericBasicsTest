#ifndef YEE1DVACCUM_H
#define YEE1DVACCUM_H

#include <Integrators/constants.h>
#include <array>
#include <vector>
#include <memory>
#include <algorithm>
#include <numeric>
#include <cmath>

#define ELECTRIC 0
#define MAGNETIC 1

namespace Vacuum {

/*********** Data types ***********/
using EMField = std::array<double, 2>;
using Mesh = std::vector<EMField>;
using Data = std::vector<Mesh>;

/*********** Functions ***********/
inline Mesh IntegrateStep(Mesh& mesh,size_t K,double a) {
    Mesh output(K);
    output[0][ELECTRIC] = mesh[0][ELECTRIC] - c * a * (mesh[0][MAGNETIC] - mesh[K-1][MAGNETIC]);
    for (size_t i = 1; i < K; i++) {
        output[i][ELECTRIC] = mesh[i][ELECTRIC] - c * a * (mesh[i][MAGNETIC] - mesh[i - 1][MAGNETIC]);
    }
    output[K - 1][MAGNETIC] = mesh[K - 1][MAGNETIC] - a / c * (output[0][ELECTRIC] - output[K - 1][ELECTRIC]);
    for (size_t i = 0; i < K-1; i++) {
        output[i][MAGNETIC] = mesh[i][MAGNETIC] - a / c * (output[i + 1][ELECTRIC] - output[i][ELECTRIC]);
    }
    return(output);
}

inline Mesh InitialValues(size_t K,double dz, double dt,double k) {
    Mesh mesh(K);
    for (size_t i = 0; i < K; ++i) {
        mesh[i][ELECTRIC] = std::sin(k * (dz * i - c * dt / 2.0 ));
        mesh[i][MAGNETIC] = -std::sin(k * dz * (i + 0.5)) / c;
    }
    return(mesh);
}


Data Compute(size_t,size_t,double,double,double);

}
#endif // YEE1DVACCUM_H
