// Update shaders.metal
#include <metal_stdlib>
using namespace metal;

struct VertexIn {
    float4 position [[attribute(0)]];
    float4 color [[attribute(1)]];
    float3 normal [[attribute(2)]];
    float2 texCoord [[attribute(3)]];
    uint materialIndex [[attribute(4)]];
};

struct VertexOut {
    float4 position [[position]];
    float4 color;
    uint materialIndex;
};

struct Uniforms {
    float4x4 viewMatrix;
    float4x4 projectionMatrix;
    float4x4 modelMatrix;
};

struct Material {
    packed_float3 ambient; float _pad0;
    packed_float3 diffuse; float _pad1;
    packed_float3 specular;
    float shininess;
};

vertex VertexOut vertex_main(
        VertexIn in [[stage_in]],
        constant Material *material [[buffer(1)]],
        constant Uniforms &uniforms [[buffer(11)]]
        ) {
    VertexOut out;
    out.position = uniforms.projectionMatrix * uniforms.viewMatrix * uniforms.modelMatrix * in.position;
    out.color = in.color;
    out.materialIndex = in.materialIndex;
    return out;
}

fragment float4 fragment_main(
                VertexOut in [[stage_in]],
                constant Material* materials [[buffer(0)]]) {

     Material mat = materials[in.materialIndex];


    // Apply material properties to lighting calculation
    float3 finalColor = mat.ambient + mat.diffuse * 1.0;
    return float4(mat.diffuse, 1.0);
}
