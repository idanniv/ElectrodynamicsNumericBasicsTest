#ifndef BORISPUSH_H
#define BORISPUSH_H

#include <array>
#include <vector>
#include <memory>
#include <algorithm>
#include <numeric>
#include <cmath>
#include "constants.h"

#define ELECTRIC 0
#define MAGNETIC 1

#define POSITION 0
#define VELOCITY 1


namespace Boris {

/*********** Data types ***********/
using Vector = std::array<double, 3>;
using StateVector = std::array<Vector, 2>;

using Array = std::vector<StateVector>;

/*********** Enums ***********/
enum class Field
{
    Electric = 0,
    Magnetic = 1
};

enum class Phase
{
    Position = 0,
    Velocity = 1
};

/*********** Functions ***********/
/*********** Vector manipulations ***********/

auto norm2 = [](Vector& v) -> double {
    return(std::inner_product(v.begin(), v.end(), v.begin(), 0.0));
};

auto gamma = [](Vector& v) -> double {
    return(1.0 / std::sqrt(1.0 - norm2(v) / c2));
};

auto inv_gamma = [](Vector& v) -> double {
    return(1.0 / std::sqrt(1.0 + norm2(v) / c2));
};

auto cross = [](Vector& v, Vector& u) -> Vector {
    return(Vector{ v[1] * u[2] - v[2] * u[1], v[2] * u[0] - v[0] * u[2] , v[0] * u[1] - v[1] * u[0] });
};

/*auto normalize = [](Vector& v0) -> Vector {
    Vector v1;
    std::transform(v0.begin(), v0.end(), v1.begin(), [n = std::sqrt(norm2(v0))](double q) {return(q/n); });
    return(v1);
};*/
/*********** Boris Pusher ***********/

inline Vector VelocityPush(Vector& v0,double delta,double a,StateVector& EMfield) {
    Vector u0;
    std::transform(v0.begin(), v0.end(), u0.begin(), [l = gamma(v0)](double q) {return(l * q); });

    Vector u_minus;
    for (size_t i = 0; i < u_minus.size(); i++) {
        u_minus[i] = u0[i] + a * delta / 2.0 * EMfield[ELECTRIC][i];
    }

    Vector A= EMfield[MAGNETIC];
    std::transform(A.begin(), A.end(), A.begin(), [l= a / 2.0 * delta * inv_gamma(u_minus)](double q) {return(l * q); });

    Vector temp1 = cross(u_minus, A);
    Vector temp2 = cross(temp1, A);
    Vector u_plus;
    for (size_t i = 0; i < u_plus.size(); i++) {
        u_plus[i] = u_minus[i] + 2.0 * (temp1[i] + temp2[i]) / (1.0 + norm2(A));
    }

    Vector u1;
    for (size_t i = 0; i < u1.size(); i++) {
        u1[i] = u_plus[i] + a * delta / 2.0 * EMfield[ELECTRIC][i];
    }

    Vector v1;
    std::transform(u1.begin(), u1.end(), v1.begin(), [l = inv_gamma(u1)](double q) {return(l * q); });

    return(v1);
}

inline Vector PositionPush(Vector& x0, Vector& v1,double delta) {
    Vector x1;
    for (std::size_t i = 0; i < x1.size(); i++) {
        x1[i] = x0[i] + delta * v1[i];
    }
    return(x1);
}

inline StateVector BorisPush(StateVector& state,double delta,double a,StateVector EMfield) {
    Vector v_new = VelocityPush(state[VELOCITY],delta,a,EMfield);
    Vector x_new = PositionPush(state[POSITION], v_new,delta);
    return(StateVector{ x_new,v_new });
}

inline StateVector InitialState(StateVector& InitialValues,double delta) {
    StateVector state=InitialValues;
    Vector& x= state[(size_t)Phase::Position];
    for (size_t i = 0; i < x.size(); i++) {
        x[i] = x[i] + delta * state[VELOCITY][i] / 2.0;
    }
    return(state);
}

Array Compute(size_t,double,double,StateVector,StateVector);
}
#endif // BORISPUSH_H
