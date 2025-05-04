#include <metal_stdlib>
using namespace metal;



struct VertexOut {
    float4 position [[position]]; // Position in clip space
    float4 color;                 // Color to pass to the fragment shader
};

// TODO: decouple vertex attributes into separate buffers using vertexId
    //constant packed_float4 *positions [[buffer(0)]],
vertex VertexOut vertex_main(

    ) {
    VertexOut out;

    return out;
}

fragment float4 fragment_main(VertexOut in [[stage_in]]) {
    return in.color; // Use the interpolated color
}
