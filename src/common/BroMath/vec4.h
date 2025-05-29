#pragma once

#include <cmath>
#include <iostream>


class vec4
{
public:
  vec4(float x, float y, float z, float w) : v{x, y, z, w}{};
  //vec4(vec3 v3, float w);   // TODO: Turn vec3 into vec4
  ~vec4() = default; // No specialized destructor

  // Getters
  float x() const {return v[0];};
  float y() const{return v[1];};
  float z() const{return v[2];};
  float w() const{return v[3];};

  float length() const {return sqrt(v[0]*v[0] + v[1]*v[1] + v[2]*v[2]);}

  /*
   * Overloads
   */
  friend std::ostream& operator << (std::ostream& os, const vec4& v) {
    os << v.x() << " " << v.y() << " " << v.z() << " " << v.w();
    return os;
  }

private:
  float v[4];
};

using float4 = vec4;
