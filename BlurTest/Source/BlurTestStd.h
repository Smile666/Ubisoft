// *******************************
//	Includes
// *******************************
#include <windows.h>
#include <d3d11.h>
#include <d3dx11.h>
#include <d3d10.h>

#include <vector>
#include <list>

#include <assert.h>

// *******************************
//	Defines
// *******************************
#define real float
#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480

//switch off SIMD math
#define _XM_NO_INTRINSICS_

#include <xnamath.h>

// *******************************
//	Memory
// *******************************
#if !defined(SAFE_DELETE)
	#define SAFE_DELETE(x) if(x) delete x; x=NULL;
#endif

#if !defined(SAFE_DELETE_ARRAY)
	#define SAFE_DELETE_ARRAY(x) if (x) delete [] x; x=NULL; 
#endif

#if !defined(SAFE_RELEASE)
	#define SAFE_RELEASE(x) if(x) x->Release(); x=NULL;
#endif