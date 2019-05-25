#if 0
//
// Generated by Microsoft (R) HLSL Shader Compiler 10.1
//
//
// Buffer Definitions: 
//
// cbuffer cbOverlayOffset
// {
//
//   float2 g_offset;                   // Offset:    0 Size:     8
//
// }
//
//
// Resource Bindings:
//
// Name                                 Type  Format         Dim      HLSL Bind  Count
// ------------------------------ ---------- ------- ----------- -------------- ------
// texOverlay_                       texture  float4          2d             t0      1 
// cbOverlayOffset                   cbuffer      NA          NA            cb0      1 
//
//
//
// Input signature:
//
// Name                 Index   Mask Register SysValue  Format   Used
// -------------------- ----- ------ -------- -------- ------- ------
// SV_POSITION              0   xyzw        0      POS   float   xy  
//
//
// Output signature:
//
// Name                 Index   Mask Register SysValue  Format   Used
// -------------------- ----- ------ -------- -------- ------- ------
// SV_TARGET                0   xyzw        0   TARGET   float   xyzw
//
ps_5_0
dcl_globalFlags refactoringAllowed
dcl_constantbuffer CB0[1], immediateIndexed
dcl_resource_texture2d (float,float,float,float) t0
dcl_input_ps_siv linear noperspective v0.xy, position
dcl_output o0.xyzw
dcl_temps 2
add r0.xy, v0.xyxx, -cb0[0].xyxx
ftoi r0.xy, r0.xyxx
mov r0.zw, l(0,0,0,0)
ld_indexable(texture2d)(float,float,float,float) r0.xyzw, r0.xyzw, t0.xyzw
eq r1.x, r0.w, l(0.000000)
mov o0.xyzw, r0.xyzw
discard_nz r1.x
ret 
// Approximately 8 instruction slots used
#endif

const BYTE g_OverlayPS[] = {68, 88, 66, 67, 10, 17, 15, 144, 22, 42, 167, 108, 125, 39, 80, 171,
    119, 26, 51, 153, 1, 0, 0, 0, 136, 3, 0, 0, 5, 0, 0, 0, 52, 0, 0, 0, 112, 1, 0, 0, 164, 1, 0, 0,
    216, 1, 0, 0, 236, 2, 0, 0, 82, 68, 69, 70, 52, 1, 0, 0, 1, 0, 0, 0, 152, 0, 0, 0, 2, 0, 0, 0,
    60, 0, 0, 0, 0, 5, 255, 255, 0, 1, 0, 0, 12, 1, 0, 0, 82, 68, 49, 49, 60, 0, 0, 0, 24, 0, 0, 0,
    32, 0, 0, 0, 40, 0, 0, 0, 36, 0, 0, 0, 12, 0, 0, 0, 0, 0, 0, 0, 124, 0, 0, 0, 2, 0, 0, 0, 5, 0,
    0, 0, 4, 0, 0, 0, 255, 255, 255, 255, 0, 0, 0, 0, 1, 0, 0, 0, 13, 0, 0, 0, 136, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 116, 101, 120, 79,
    118, 101, 114, 108, 97, 121, 95, 0, 99, 98, 79, 118, 101, 114, 108, 97, 121, 79, 102, 102, 115,
    101, 116, 0, 136, 0, 0, 0, 1, 0, 0, 0, 176, 0, 0, 0, 16, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 216,
    0, 0, 0, 0, 0, 0, 0, 8, 0, 0, 0, 2, 0, 0, 0, 232, 0, 0, 0, 0, 0, 0, 0, 255, 255, 255, 255, 0, 0,
    0, 0, 255, 255, 255, 255, 0, 0, 0, 0, 103, 95, 111, 102, 102, 115, 101, 116, 0, 102, 108, 111,
    97, 116, 50, 0, 1, 0, 3, 0, 1, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 225, 0, 0, 0, 77, 105, 99, 114, 111, 115, 111, 102, 116, 32, 40, 82, 41, 32, 72,
    76, 83, 76, 32, 83, 104, 97, 100, 101, 114, 32, 67, 111, 109, 112, 105, 108, 101, 114, 32, 49,
    48, 46, 49, 0, 73, 83, 71, 78, 44, 0, 0, 0, 1, 0, 0, 0, 8, 0, 0, 0, 32, 0, 0, 0, 0, 0, 0, 0, 1,
    0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 15, 3, 0, 0, 83, 86, 95, 80, 79, 83, 73, 84, 73, 79, 78, 0, 79,
    83, 71, 78, 44, 0, 0, 0, 1, 0, 0, 0, 8, 0, 0, 0, 32, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0,
    0, 0, 0, 0, 0, 15, 0, 0, 0, 83, 86, 95, 84, 65, 82, 71, 69, 84, 0, 171, 171, 83, 72, 69, 88, 12,
    1, 0, 0, 80, 0, 0, 0, 67, 0, 0, 0, 106, 8, 0, 1, 89, 0, 0, 4, 70, 142, 32, 0, 0, 0, 0, 0, 1, 0,
    0, 0, 88, 24, 0, 4, 0, 112, 16, 0, 0, 0, 0, 0, 85, 85, 0, 0, 100, 32, 0, 4, 50, 16, 16, 0, 0, 0,
    0, 0, 1, 0, 0, 0, 101, 0, 0, 3, 242, 32, 16, 0, 0, 0, 0, 0, 104, 0, 0, 2, 2, 0, 0, 0, 0, 0, 0,
    9, 50, 0, 16, 0, 0, 0, 0, 0, 70, 16, 16, 0, 0, 0, 0, 0, 70, 128, 32, 128, 65, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 27, 0, 0, 5, 50, 0, 16, 0, 0, 0, 0, 0, 70, 0, 16, 0, 0, 0, 0, 0, 54, 0, 0, 8,
    194, 0, 16, 0, 0, 0, 0, 0, 2, 64, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 45, 0,
    0, 137, 194, 0, 0, 128, 67, 85, 21, 0, 242, 0, 16, 0, 0, 0, 0, 0, 70, 14, 16, 0, 0, 0, 0, 0, 70,
    126, 16, 0, 0, 0, 0, 0, 24, 0, 0, 7, 18, 0, 16, 0, 1, 0, 0, 0, 58, 0, 16, 0, 0, 0, 0, 0, 1, 64,
    0, 0, 0, 0, 0, 0, 54, 0, 0, 5, 242, 32, 16, 0, 0, 0, 0, 0, 70, 14, 16, 0, 0, 0, 0, 0, 13, 0, 4,
    3, 10, 0, 16, 0, 1, 0, 0, 0, 62, 0, 0, 1, 83, 84, 65, 84, 148, 0, 0, 0, 8, 0, 0, 0, 2, 0, 0, 0,
    0, 0, 0, 0, 2, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
