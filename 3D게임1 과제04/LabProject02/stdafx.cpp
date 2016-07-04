// stdafx.cpp : 표준 포함 파일만 들어 있는 소스 파일입니다.
// LabProject02.pch는 미리 컴파일된 헤더가 됩니다.
// stdafx.obj에는 미리 컴파일된 형식 정보가 포함됩니다.

#include "stdafx.h"

int	Nowscene = 1;
float p_X = 0;
float p_Y = -20;
float p_Z = 5;

int c_Value = 0;
float c_X[computer_Value] = { 0 };
float c_Y[computer_Value] = { 0 };
float c_Z[computer_Value] = { 0 };
bool c_View[computer_Value] = { true };
int mi_map[maze_Data][maze_Data] = { 0 };

// TODO: 필요한 추가 헤더는
// 이 파일이 아닌 STDAFX.H에서 참조합니다.
