/*
Author: Nathan Dunn
Module: G3D

Vertex shader for wire frame rendering.

*/
cbuffer cb : register(b0)
{
    // D3D11 rule: elements must align to 16 byte boundary!
    // float3x3 is 48 bytes because it is natively packed to be 3x4
    float3 translation; // XYZ translation from the object's frame to the camera's frame
    float ft; // tan(vertical_fov/2)
    row_major float3x3 rotation_bodyFrame; // rotation of the object relative to the global frame
    row_major float3x3 rotation_cameraFrame; // rotation of the object from the global frame to the camera frame
    float3 scale; // scale in body frame
    float ar; // aspect ratio, w/h
    float a; // near/far plane operator a, a=f/(f-n)
    float b; // near/far plane operator b, b=f*n/(f-n)
    float pack[2];
}

float4 main(float3 pos : Position) : SV_Position
{
    // goal is to return the clip space coordinates in XYZW, where W is x
    // clip space ZY is the projection of coordinates onto the near plane using the world space size of the near plane
    // clip space x is how far away from the camera position the coordinate is
    // the convention for direction is: +x forward (into screen), -z up, +y right
    
    // scale first
    float3 v1;
    v1[0] = pos[0] * scale[0];
    v1[1] = pos[1] * scale[1];
    v1[2] = pos[2] * scale[2];
    
    // rotate around the body
    v1 = mul(rotation_bodyFrame, v1);
    
    // translate to camera space
    v1[0] = v1[0] + translation[0];
    v1[1] = v1[1] + translation[1];
    v1[2] = v1[2] + translation[2];
    
    // rotate within camera space
    v1 = mul(rotation_cameraFrame, v1);
    
    // cache x to use as W in return
    float w = v1[0];
    
    // clip space depths are computed non-linearly
    // when these are divided by W in the next pipeline stage, the resolution for up-close x values will be exponentially better than far away
    v1[0] = v1[0] * a - b;
    
    // clip space near plane zy dimensions represent height and width
    v1[1] = v1[1] / (ft * ar);
    v1[2] = v1[2] / ft;
    
    /*
    direct x expects the output to be in XYZW, and uses different conventions for what XYZ means
    X = horizontal screen space
    Y = vertical in screen space
    Z = depth in screen space
    W = homogenous divisor (depth)
    So it needs to be rearranged from my coordinate system
    */
    return float4(v1[1], v1[2], v1[0], w);
    
}
