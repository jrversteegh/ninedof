/* 
This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>. 
*/

/** \file
 * \author Jaap Versteegh <j.r.versteegh@gmail.com>
 * Declares some basic types that are used throughout
 */

#ifndef MRU_TYPES_H
#define MRU_TYPES_H

#define _USE_MATH_DEFINES
#include <cmath>
#include <utility>
#include <deque>
#include <set>

#include <boost/date_time/posix_time/posix_time.hpp>

#include <CGAL/Simple_cartesian.h>
#include <CGAL/Vector_3.h>

namespace mru {

typedef boost::posix_time::ptime Time;
inline Time utc_now() {
  return boost::posix_time::microsec_clock::universal_time();
}

typedef float Scalar;
typedef CGAL::Simple_cartesian<Scalar>::Vector_3 Vector;
typedef CGAL::Simple_cartesian<Scalar>::Point_3 Point;

template <int Min, int Max>
struct RotScalar {
  static constexpr Scalar min = 0.5 * M_PI * Min;
  static constexpr Scalar max = 0.5 * M_PI * Max;
};

typedef CGAL::Simple_cartesian<Scalar>::Aff_transformation_3 Transformation;

// Define quantities provided by the sensors
enum class Quantity {
  Pressure, 
  Temperature, 
  Acceleration,
  AngularVelocity,
  MagneticFlux,
  Heading,
  Pitch,
  Roll,
};


template<Quantity Q> 
struct Quantity_type {}; 
template<> 
struct Quantity_type<Quantity::Pressure> { 
  typedef Scalar type; 
};
template<> 
struct Quantity_type<Quantity::Temperature> { 
  typedef Scalar type; 
};
template<> 
struct Quantity_type<Quantity::Acceleration> { 
  typedef Vector type; 
};
template<> 
struct Quantity_type<Quantity::AngularVelocity> { 
  typedef Vector type; 
};
template<> 
struct Quantity_type<Quantity::MagneticFlux> { 
  typedef Vector type; 
};

struct Correction: public Transformation {
  using Transformation::Transformation;
  Correction(const Scalar& x_factor, const Scalar& x_offset, 
             const Scalar& y_factor, const Scalar& y_offset,
             const Scalar& z_factor, const Scalar& z_offset):
    Transformation(x_factor, 0, 0, x_offset,
                   0, y_factor, 0, y_offset,
                   0, 0, z_factor, z_offset) {}
  Correction(const Scalar& xx_factor, const Scalar& xy_factor, const Scalar& xz_factor, const Scalar& x_offset, 
             const Scalar& yx_factor, const Scalar& yy_factor, const Scalar& yz_factor, const Scalar& y_offset,
             const Scalar& zx_factor, const Scalar& zy_factor, const Scalar& zz_factor, const Scalar& z_offset):
    Transformation(xx_factor, xy_factor, xz_factor, x_offset,
                   yx_factor, yy_factor, yz_factor, y_offset,
                   zx_factor, zy_factor, zz_factor, z_offset) {}
};

struct Calibration {
  Correction correction;
  Scalar value_factor;
  Scalar value_offset;
  Calibration(): 
      correction(1.0, 0.0, 1.0, 0.0, 1.0, 0.0),
      value_factor(1.0), value_offset(0.0) {}
  Calibration(const Calibration& calibration): 
      correction(calibration.correction),
      value_factor(calibration.value_factor), value_offset(calibration.value_offset) {}
  Calibration(const Scalar& x_factor, const Scalar& x_offset,
              const Scalar& y_factor, const Scalar& y_offset,
              const Scalar& z_factor, const Scalar& z_offset,
              const Scalar& v_factor, const Scalar& v_offset):
      correction(x_factor, x_offset, y_factor, y_offset, z_factor, z_offset),
      value_factor(v_factor), value_offset(v_offset) {}
  Calibration(const Scalar& xx_factor, const Scalar& xy_factor, const Scalar& xz_factor, const Scalar& x_offset,
              const Scalar& yx_factor, const Scalar& yy_factor, const Scalar& yz_factor, const Scalar& y_offset,
              const Scalar& zx_factor, const Scalar& zy_factor, const Scalar& zz_factor, const Scalar& z_offset,
              const Scalar& v_factor, const Scalar& v_offset):
      correction(xx_factor, xy_factor, xz_factor, x_offset, 
                 yx_factor, yy_factor, yz_factor, y_offset, 
                 zx_factor, zy_factor, zz_factor, z_offset),
      value_factor(v_factor), value_offset(v_offset) {}
  Calibration& operator=(const Calibration& calibration) {
    correction = calibration.correction;
    value_factor = calibration.value_factor;
    value_offset = calibration.value_offset;
    return *this;
  }
  Scalar x_factor() const { 
    return correction.m(0, 0);
  }
  Scalar x_offset() const { 
    return correction.m(0, 3);
  }
  Scalar y_factor() const { 
    return correction.m(1, 1);
  }
  Scalar y_offset() const { 
    return correction.m(1, 3);
  }
  Scalar z_factor() const { 
    return correction.m(2, 2);
  }
  Scalar z_offset() const { 
    return correction.m(2, 3);
  }
  Scalar v_factor() const { 
    return value_factor;
  }
  Scalar v_offset() const { 
    return value_offset;
  }
};

struct Sample {
  Time time;
  Vector vector;
  Scalar value;
  Sample(): time(), vector(), value() {}
  Sample(const Sample& s): 
      time(s.time), vector(s.vector), value(s.value) {}
  Sample(const Time& t, const Vector& vr): 
      time(t), vector(vr), value() {}
  Sample(const Time& t, const Vector& vr, const Scalar& v): 
      time(t), vector(vr), value(v) {}
  Sample(const Vector& vr): 
      time(utc_now()), vector(vr), value() {}
  Sample(const Vector& vr, const Scalar& v): 
      time(utc_now()), vector(vr), value(v) {}
  Sample(Sample&& s): 
      time(std::move(s.time)), vector(std::move(s.vector)), value(std::move(s.value)) {}
  Sample(const Point& p,
         const Scalar& v, const Calibration& calibration):
      time(utc_now()) {
    vector = calibration.correction(p) - CGAL::ORIGIN;
    value = v * calibration.value_factor + calibration.value_offset;
  }
  Sample& operator=(const Sample& s) {
    time = s.time;
    vector = s.vector;
    value = s.value;
    return *this;
  }
  Sample& operator=(Sample&& s) {
    time = std::move(s.time);
    vector = std::move(s.vector);
    value = std::move(s.value);
    return *this;
  }
};
typedef std::deque<Sample> Samples;
typedef Samples::iterator Sample_i;


} //namespace mru

#endif
