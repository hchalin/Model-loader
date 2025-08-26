// Update shaders.metal
#include <metal_stdlib>
using namespace metal;

struct VertexIn {
    float4 position [[attribute(0)]];
    float4 color [[attribute(1)]];
};

struct VertexOut {
    float4 position [[position]];
    float4 color;
};

struct Uniforms {
    float4x4 viewMatrix;
    float4x4 projectionMatrix;
    float4x4 modelMatrix;
};

vertex VertexOut vertex_main(
        VertexIn in [[stage_in]],
        constant Uniforms &uniforms [[buffer(1)]]
        ) {
    VertexOut out;
    out.position = uniforms.projectionMatrix * uniforms.viewMatrix * uniforms.modelMatrix * in.position;
    out.color = in.color;
    return out;
}

fragment float4 fragment_main(VertexOut in [[stage_in]]) {
    return in.color;
}
