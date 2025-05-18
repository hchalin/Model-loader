#pragma once

// TODO: Create my own matrix class to hold all transformations

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

private:
  float v[4];
};

using float4 = vec4;
