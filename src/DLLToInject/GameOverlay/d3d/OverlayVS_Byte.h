#if 0
//
// Generated by Microsoft (R) HLSL Shader Compiler 10.1
//
//
//
// Input signature:
//
// Name                 Index   Mask Register SysValue  Format   Used
// -------------------- ----- ------ -------- -------- ------- ------
// SV_VERTEXID              0   x           0   VERTID    uint   x   
//
//
// Output signature:
//
// Name                 Index   Mask Register SysValue  Format   Used
// -------------------- ----- ------ -------- -------- ------- ------
// SV_POSITION              0   xyzw        0      POS   float   xyzw
//
vs_5_0
dcl_globalFlags refactoringAllowed
dcl_input_sgv v0.x, vertex_id
dcl_output_siv o0.xyzw, position
switch v0.x
  case l(0)
  mov o0.xyzw, l(-1.000000,-1.000000,0,1.000000)
  ret 
  case l(1)
  mov o0.xyzw, l(-1.000000,3.000000,0,1.000000)
  ret 
  case l(2)
  mov o0.xyzw, l(3.000000,-1.000000,0,1.000000)
  ret 
  default 
  break 
endswitch 
mov o0.xyzw, l(0,0,0,1.000000)
ret 
// Approximately 15 instruction slots used
#endif

const BYTE g_OverlayVS[] = {68, 88, 66, 67, 148, 166, 128, 14, 229, 217, 142, 99, 177, 9, 29, 214,
    33, 88, 108, 154, 1, 0, 0, 0, 164, 2, 0, 0, 5, 0, 0, 0, 52, 0, 0, 0, 160, 0, 0, 0, 212, 0, 0, 0,
    8, 1, 0, 0, 8, 2, 0, 0, 82, 68, 69, 70, 100, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 60, 0,
    0, 0, 0, 5, 254, 255, 0, 1, 0, 0, 60, 0, 0, 0, 82, 68, 49, 49, 60, 0, 0, 0, 24, 0, 0, 0, 32, 0,
    0, 0, 40, 0, 0, 0, 36, 0, 0, 0, 12, 0, 0, 0, 0, 0, 0, 0, 77, 105, 99, 114, 111, 115, 111, 102,
    116, 32, 40, 82, 41, 32, 72, 76, 83, 76, 32, 83, 104, 97, 100, 101, 114, 32, 67, 111, 109, 112,
    105, 108, 101, 114, 32, 49, 48, 46, 49, 0, 73, 83, 71, 78, 44, 0, 0, 0, 1, 0, 0, 0, 8, 0, 0, 0,
    32, 0, 0, 0, 0, 0, 0, 0, 6, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 83, 86, 95, 86, 69, 82,
    84, 69, 88, 73, 68, 0, 79, 83, 71, 78, 44, 0, 0, 0, 1, 0, 0, 0, 8, 0, 0, 0, 32, 0, 0, 0, 0, 0,
    0, 0, 1, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 15, 0, 0, 0, 83, 86, 95, 80, 79, 83, 73, 84, 73, 79,
    78, 0, 83, 72, 69, 88, 248, 0, 0, 0, 80, 0, 1, 0, 62, 0, 0, 0, 106, 8, 0, 1, 96, 0, 0, 4, 18,
    16, 16, 0, 0, 0, 0, 0, 6, 0, 0, 0, 103, 0, 0, 4, 242, 32, 16, 0, 0, 0, 0, 0, 1, 0, 0, 0, 76, 0,
    0, 3, 10, 16, 16, 0, 0, 0, 0, 0, 6, 0, 0, 3, 1, 64, 0, 0, 0, 0, 0, 0, 54, 0, 0, 8, 242, 32, 16,
    0, 0, 0, 0, 0, 2, 64, 0, 0, 0, 0, 128, 191, 0, 0, 128, 191, 0, 0, 0, 0, 0, 0, 128, 63, 62, 0, 0,
    1, 6, 0, 0, 3, 1, 64, 0, 0, 1, 0, 0, 0, 54, 0, 0, 8, 242, 32, 16, 0, 0, 0, 0, 0, 2, 64, 0, 0, 0,
    0, 128, 191, 0, 0, 64, 64, 0, 0, 0, 0, 0, 0, 128, 63, 62, 0, 0, 1, 6, 0, 0, 3, 1, 64, 0, 0, 2,
    0, 0, 0, 54, 0, 0, 8, 242, 32, 16, 0, 0, 0, 0, 0, 2, 64, 0, 0, 0, 0, 64, 64, 0, 0, 128, 191, 0,
    0, 0, 0, 0, 0, 128, 63, 62, 0, 0, 1, 10, 0, 0, 1, 2, 0, 0, 1, 23, 0, 0, 1, 54, 0, 0, 8, 242, 32,
    16, 0, 0, 0, 0, 0, 2, 64, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 128, 63, 62, 0, 0, 1,
    83, 84, 65, 84, 148, 0, 0, 0, 15, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 5, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
