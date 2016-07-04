// stdafx.h : 자주 사용하지만 자주 변경되지는 않는
// 표준 시스템 포함 파일 또는 프로젝트 관련 포함 파일이
// 들어 있는 포함 파일입니다.
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // 거의 사용되지 않는 내용은 Windows 헤더에서 제외합니다.
// Windows 헤더 파일:
#include <windows.h>

// C 런타임 헤더 파일입니다.
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <mmsystem.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>


#include<random>
#include<iostream>
#include <stdlib.h>
#include<ctime>

using namespace std;

// TODO: 프로그램에 필요한 추가 헤더는 여기에서 참조합니다.
#include <D3D9Types.h>
#include <d3d11.h>
#include <d3dx11.h>
#include <mmsystem.h>
#include <d3dcompiler.h>  	//쉐이더 컴파일 함수를 사용하기 위한 헤더 파일
#include <D3DX10Math.h>	//Direct3D 수학 함수를 사용하기 위한 헤더 파일
#include <fstream>

#define computer_Value 1000
#define maze_Data 30
extern int Nowscene;
extern float p_X;
extern float p_Y;
extern float p_Z;
extern int c_Value;
extern float c_X[computer_Value];
extern float c_Y[computer_Value];
extern float c_Z[computer_Value];
extern bool c_View[computer_Value];
extern int mi_map[maze_Data][maze_Data];

#define FRAME_BUFFER_WIDTH   640
#define FRAME_BUFFER_HEIGHT   480
#define VS_SLOT_CAMERA				0x00
#define VS_SLOT_WORLD_MATRIX			0x01

//조명과 재질을 설정하기 위한 상수 버퍼의 슬롯 번호를 정의한다. 
#define PS_SLOT_LIGHT			0x00
#define PS_SLOT_MATERIAL		0x01



