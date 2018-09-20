// Win32Project3.cpp : 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"
#include "Win32Project3.h"
#include "egl/egl.h"
#include "vg/openvg.h"
#include "vgLoadImage.h"
#include "DXUTsound.h"
#include <time.h>


#define MAX_LOADSTRING 100

// 전역 변수:
HINSTANCE hInst;                                // 현재 인스턴스입니다.
WCHAR szTitle[MAX_LOADSTRING];                  // 제목 표시줄 텍스트입니다.
WCHAR szWindowClass[MAX_LOADSTRING];            // 기본 창 클래스 이름입니다.

EGLDisplay display;
EGLSurface surface;
EGLContext context;

CSoundManager* sm = NULL;

CSound* backgroundSound = NULL;

CSound* missileSound = NULL;

//내 진지
struct base {
	int x, y;
	int count;
}base;

//내 헬기helo
struct flight {
	int x, y;
	int count;
}flight;

//내 적기용 미사일 bullet
struct missiles {
	int shot, x, y;
}missiles[20];

//탱크용 폭탄agm
struct bombs {
	int shot, x, y;
	float vx, vy;
}bombs[20];

//내 헬기용 미사일aim
struct missiles_h {
	int x, y, shot;
}missiles_h[20];

//F22 적기
struct enemy_f{
	int x, y, shot;
}enemy_f[20];

//적 탱크
struct enemy_t {
	int x, y, shot;
}enemy_t[20];

//적 헬기
struct enemy_h {
	int x, y, shot;
}enemy_h[20];
/*
//폭탄터짐
struct explosion {
	int shot, x, y;
}explosion[20];
*/

void timerProc();
void draw();

VGImage background;
VGImage basecamp;
VGImage missile;
VGImage missile_h;
VGImage bomb;
VGImage bomb_explosion;
VGImage plane;
VGImage enemy_plane;
VGImage enemy_tank;
VGImage enemy_helicopter;

// 이 코드 모듈에 들어 있는 함수의 정방향 선언입니다.
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: 여기에 코드를 입력합니다.

    // 전역 문자열을 초기화합니다.
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_WIN32PROJECT3, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // 응용 프로그램 초기화를 수행합니다.
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_WIN32PROJECT3));

    MSG msg;

    // 기본 메시지 루프입니다.
	DWORD lastTime = GetTickCount();
	while (1)
	{
		if (PeekMessage(&msg, NULL, 0, 0, 1))
		{
			if (!TranslateAccelerator(msg.hwnd, NULL, &msg))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			if (msg.message == WM_QUIT) break;
		}
		DWORD curTime = GetTickCount();
		if (curTime - lastTime>32) // 30 frame per second
		{
			lastTime = lastTime + 33;
			timerProc();
		}
	}
	return (int)msg.wParam;
}



//
//  함수: MyRegisterClass()
//
//  목적: 창 클래스를 등록합니다.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_WIN32PROJECT3));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_WIN32PROJECT3);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   함수: InitInstance(HINSTANCE, int)
//
//   목적: 인스턴스 핸들을 저장하고 주 창을 만듭니다.
//
//   설명:
//
//        이 함수를 통해 인스턴스 핸들을 전역 변수에 저장하고
//        주 프로그램 창을 만든 다음 표시합니다.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // 인스턴스 핸들을 전역 변수에 저장합니다.

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, 1000, 560, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  함수: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  목적:  주 창의 메시지를 처리합니다.
//
//  WM_COMMAND  - 응용 프로그램 메뉴를 처리합니다.
//  WM_PAINT    - 주 창을 그립니다.
//  WM_DESTROY  - 종료 메시지를 게시하고 반환합니다.
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // 메뉴 선택을 구문 분석합니다.
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: 여기에 hdc를 사용하는 그리기 코드를 추가합니다.
            EndPaint(hWnd, &ps);
        }
        break;

	case WM_KEYDOWN:
		//적헬기용 미사일
		if (wParam == 'Z') {
			for (int i = 0; i<20; i++) {
				if (missiles_h[i]. shot == false) {
					missiles_h[i].shot = true;
					missiles_h[i].x = flight.x + 90;
					missiles_h[i].y = flight.y;
					break;
				}
			}
		}
		//탱크용 폭탄
		if (wParam == 'X') {
			for (int i = 0; i<20; i++) {
				if (bombs[i].shot == false) {
					bombs[i].shot = true;
					bombs[i].x = flight.x + 50;
					bombs[i].y = flight.y;
					bombs[i].vx = 3;
					bombs[i].vy = 0;
					break; 
				}
			}
		}

		break;

	case WM_CREATE:
	{
		display = eglGetDisplay(GetDC(hWnd));
		eglInitialize(display, NULL, NULL);
		eglBindAPI(EGL_OPENVG_API);

		EGLint conf_list[] = { EGL_RED_SIZE, 8,
			EGL_GREEN_SIZE, 8,
			EGL_BLUE_SIZE, 8,
			EGL_ALPHA_SIZE, 8,
			EGL_NONE };
		EGLConfig config;
		EGLint    num_config;
		eglChooseConfig(display, conf_list, &config, 1, &num_config);

		surface = eglCreateWindowSurface(display, config, hWnd, NULL);
		context = eglCreateContext(display, 0, NULL, NULL);
		eglMakeCurrent(display, surface, surface, context);
		
		//배경
		background = vgLoadImage(TEXT("bk.jpg"));
		
		//우리 편 진지
		basecamp = vgLoadImage(TEXT("base.png"));
		base.x = 0;
		base.y = 10;

		//비행기 초기위치
		plane = vgLoadImage(TEXT("helo.png"));
		flight.x = 300;
		flight.y = 150;
		flight.count = 0;
		
		//랜덤함수
		srand((unsigned)time(NULL));

		//적기용 미사일
		missile = vgLoadImage(TEXT("bullet.png"));
		for (int i = 0; i < 20; i++) {
			missiles[i].shot = false;
		}
		//탱크용 폭탄
		bomb = vgLoadImage(TEXT("agm.png"));
		for (int i = 0; i < 20; i++) {
			bombs[i].shot = false;
		}
		bomb_explosion = vgLoadImage(TEXT("explosion.png"));
		/*
		//폭탄터짐
		for (int i = 0; i < 20; i++) {
			explosion[i].shot = false;
		}
		*/
		//헬기용 미사일
		missile_h = vgLoadImage(TEXT("aim.png"));
		for (int i = 0; i < 20; i++) {
			missiles_h[i].shot = false;
		}
		//적기
		enemy_plane = vgLoadImage(TEXT("F22.png"));
		for (int i = 0; i < 20; i++) {
			enemy_f[i].shot = false;
		}
		//적탱크
		enemy_tank = vgLoadImage(TEXT("tank.png"));
		for (int i = 0; i < 20; i++) {
			enemy_t[i].shot = false;
		}
		//적헬기
		enemy_helicopter = vgLoadImage(TEXT("ah66.png"));
		for (int i = 0; i < 20; i++) {
			enemy_h[i].shot = false;
		}


		//사운드만들기
		sm = new CSoundManager();
		sm->Initialize(hWnd, DSSCL_NORMAL);
		
		//백그라운드사운드
		sm->Create(&backgroundSound, TEXT("background_sound.wav"), 0, GUID_NULL);
		backgroundSound->Play(0, DSBPLAY_LOOPING);

		//미사일사운드
		sm->Create(&missileSound, TEXT("missile_sound.wav"), 0, GUID_NULL);


	}
	break;

    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// 정보 대화 상자의 메시지 처리기입니다.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
/*
//배경
int cx=0, cy=0;
int k = cx / 1000;
*/

void draw()
{
	eglMakeCurrent(display, surface, surface, context);	
	vgSeti(VG_MATRIX_MODE, VG_MATRIX_IMAGE_USER_TO_SURFACE);
	/*
	// let’s draw k’th and k+1’th image 배경 자동 스크롤
	vgLoadIdentity(); vgTranslate(k*800 - cx, 0);
	vgDrawImage(background);
	vgLoadIdentity(); vgTranslate((k + 1)*800 - cx, 0);
	vgDrawImage(background);
	*/

	//배경 그리기
	vgLoadIdentity();
	vgDrawImage(background);
	
	//내 베이스캠프 그리기
	vgLoadIdentity();
	vgTranslate(base.x, base.y);
	vgDrawImage(basecamp);

	//내 헬기 그리기
	vgLoadIdentity();
	vgTranslate(flight.x, flight.y);
	vgDrawImage(plane);


	//적기용미사일 그리기
	for (int i = 0; i < 20; i++) {
		if (missiles[i].shot) {
			vgLoadIdentity();
			vgTranslate(missiles[i].x, missiles[i].y);
			vgDrawImage(missile);
		}
	}
	//탱크용폭탄 그리기
	for (int i = 0; i < 20; i++) {
		if (bombs[i].shot) {
			vgLoadIdentity();
			vgTranslate(bombs[i].x, bombs[i].y);
			vgDrawImage(bomb);
		}
	}
	/*//폭탄터짐 그리기
	for (int i = 0; i < 20; i++) {
		if (!bombs[i].shot) {
			vgLoadIdentity();
			vgTranslate(bombs[i].x, bombs[i].y);
			vgDrawImage(bomb_explosion);
		}
	}*/
	//헬기용미사일 그리기
	for (int i = 0; i < 20; i++) {
		if (missiles_h[i].shot) {
			vgLoadIdentity();
			vgTranslate(missiles_h[i].x, missiles_h[i].y);
			vgDrawImage(missile_h);
		}
	}
	//적기그리기
	for (int i = 0; i < 20; i++) {
		if (enemy_f[i].shot) {
			vgLoadIdentity();
			vgTranslate(enemy_f[i].x, enemy_f[i].y);
			vgDrawImage(enemy_plane);
		}
	}
	//적탱크그리기
	for (int i = 0; i < 20; i++) {
		if (enemy_t[i].shot) {
			vgLoadIdentity();
			vgTranslate(enemy_t[i].x, enemy_t[i].y);
			vgDrawImage(enemy_tank);
		}
	}
	//적헬기그리기
	for (int i = 0; i < 20; i++) {
		if (enemy_h[i].shot) {
			vgLoadIdentity();
			vgTranslate(enemy_h[i].x, enemy_h[i].y);
			vgDrawImage(enemy_helicopter);
		}
	}

	
	eglSwapBuffers(display, surface);

}
#define KEY_DOWN(code) ((GetAsyncKeyState(code)&0x8000)?1:0)
RECT rect;

void timerProc()
{
	//비행기 비동기식 움직이기
	if (GetAsyncKeyState(VK_LEFT) & 0x8000 && flight.x >= 300)  flight.x -= 10;
	if (GetAsyncKeyState(VK_RIGHT) & 0x8000 && flight.x <= 1000 - 100) flight.x += 10;
	if (GetAsyncKeyState(VK_UP) & 0x8000 && flight.y <= 560 - 110)  flight.y += 10;
	if (GetAsyncKeyState(VK_DOWN) & 0x8000 && flight.y >= 150) flight.y -= 10;
	 
	//적기용 미사일 비동기적 발사
	if (GetAsyncKeyState(VK_SPACE) & 0x8000)
	{
		for (int i = 0; i<20; i++)  {
			if (missiles[i].shot==false) {
				missileSound->Play(0);
				missileSound->Reset();
				missiles[i].shot = true;
				missiles[i].x = flight.x + 90;
				missiles[i].y = flight.y;
			} 
		}
	}
	/*
	//화면 움직이기
	cx  += 5;
	*/
	/////////////////////////////내 미사일 발사/////////////////////////
	//적기용 미사일 앞으로 나감
	for (int i = 0; i<20; i++) {
		if (missiles[i].shot) {
			missiles[i].x += 15;
			if (missiles[i].x>1000) missiles[i].shot = false;
		}
	}
	//적 헬기용 미사일 앞으로 나감
	for (int i = 0; i<20; i++) {
		if (missiles_h[i].shot) {
			missiles_h[i].x += 10;
			if (missiles_h[i].x>1000) missiles_h[i].shot = false;
		}
	}
	//적 탱크용 폭탄 밑으로 떨어짐
	for (int i = 0; i<20; i++) {
		if (bombs[i].shot) {
			bombs[i].vy -= 1;
			bombs[i].y += bombs[i].vy;
			if (bombs[i].y < 45) {
				bombs[i].shot = false;
			}
		}
	}
	/////////////////////적기 랜덤출현 및 움직임///////////////////////
	//적기 랜덤출현 
	if (rand() % 90 == 0) {
		for (int i = 0; i < 20; i++) {
			if (enemy_f[i].shot == false) {
				enemy_f[i].shot = true;
				enemy_f[i].x = 975;
				enemy_f[i].y = rand() % 150 + 300 ;
				break;
			}
		}
	}
	//적기 왼쪽으로 돌진
	for (int i = 0; i < 20; i++) {
		if (enemy_f[i].shot) {
			enemy_f[i].x -= 15;
			if (enemy_f[i].x < 0)
				enemy_f[i].shot = false;
		}
	}
	
	//적 탱크 랜덤출현
	if (rand() % 150  == 0) {
		for (int i = 0; i < 20; i++) {
			if (enemy_t[i].shot == false) {
				enemy_t[i].shot = true;
				enemy_t[i].x = 975;
				enemy_t[i].y = 45;
				break;
			}
		}
	}
	//적 탱크 왼쪽 돌진
	for (int i = 0; i < 20; i++) {
		if (enemy_t[i].shot) {
			enemy_t[i].x -= 5;
			if (enemy_t[i].x < 0)
				enemy_t[i].shot = false;
		}
	}
	
	//적헬기 랜덤출현
	if (rand() % 60 == 0) {
		for (int i = 0; i < 20; i++) {
			if (enemy_h[i].shot == false) {
				enemy_h[i].shot = true;
				enemy_h[i].x = 975;
				enemy_h[i].y = rand() % 150 + 150;
				break;
			}
		}
	}
	//적 헬기 왼쪽 돌진
	for (int i = 0; i < 20; i++) {
		if (enemy_h[i].shot) {
			enemy_h[i].x -= 2;
			if (enemy_h[i].x < 0)
				enemy_h[i].shot = false;
		}
	}

	/////////////////////////////////////적이랑 미사일/폭탄이랑 충돌검사////////////////////////////
	//적기가 총알 혹은 미사일에 부딪혔을때
	for (int i = 0; i < 20; i++) {
		if (enemy_f[i].shot) {
			//적기용 미사일 충돌
			for (int j = 0; j < 20; j++) {
				if (missiles[j].shot && missiles[j].x < enemy_f[i].x + 106 && enemy_f[i].x < missiles[j].x + 7
					&& missiles[j].y < enemy_f[i].y + 19 && enemy_f[i].y < missiles[j].y + 4) {
					missiles[j].shot = false;
					enemy_f[i].shot = false;
				}
			}
			//적헬기용 미사일 충돌
			for (int j = 0; j < 20; j++) {
				if (missiles_h[j].shot && missiles_h[j].x < enemy_f[i].x + 106 && enemy_f[i].x < missiles_h[j].x + 60
					&& missiles_h[j].y < enemy_f[i].y + 19 && enemy_f[i].y < missiles_h[j].y + 11) {
					missiles_h[j].shot = false;
					enemy_f[i].shot = false;
				}
			}
		}
	}
	//탱크가 폭탄 부딪혔을때
	for (int i = 0; i < 20; i++) {
		if (enemy_t[i].shot) {
			for (int j = 0; j < 20; j++) {
				if (bombs[j].shot && bombs[j].x < enemy_t[i].x + 51 && enemy_t[i].x < bombs[j].x + 32
					&& bombs[j].y < enemy_t[i].y + 36  && enemy_t[i].y < bombs[j].y  + 11) {
					bombs[j].shot = false;
					enemy_t[i].shot = false;
				}
			}
		}
	}
	//적헬기가 미사일 부딪혔을때
	for (int i = 0; i < 20; i++) {
		if (enemy_h[i].shot) {
			for(int j = 0; j < 20; j++) {
				if (missiles_h[j].shot && missiles_h[j].x < enemy_h[i].x + 99 && enemy_h[i].x < missiles_h[j].x + 60
					&& missiles_h[j].y < enemy_h[i].y + 32 && enemy_h[i].y < missiles_h[j].y + 11) {
					missiles_h[j].shot = false;
					enemy_h[i].shot = false;
				}
			}
		}
	}
	
	///////////////////////////////////내 헬기 적이랑 충돌검사/////////////////////
	//적기 내꺼랑 부딪혔을때
	for (int i = 0; i < 20; i++) {
		if (enemy_f[i].shot) {
			if (flight.x < enemy_f[i].x + 106  && enemy_f[i].x < flight.x + 81
				&& flight.y < enemy_f[i].y + 32 && enemy_f[i].y < flight.y + 36) {
				PostQuitMessage(0);
			}
		}
	}
	/*
	//적탱크 내꺼랑 부딪혔을때
	for (int i = 0; i < 20; i++) {
		if (enemy_t[i].shot) {
			if (flight.x < enemy_t[i].x + 51 && enemy_t[i].x < flight.x + 81
				&& flight.y < enemy_t[i].y + 36 && enemy_t[i].y < flight.y + 36) {
				PostQuitMessage(0);
			}
		}
	}
	*/
	//적헬기 내꺼랑 부딪혔을때
	for (int i = 0; i < 20; i++) {
		if (enemy_h[i].shot) {
			if (flight.x < enemy_h[i].x + 99 && enemy_h[i].x < flight.x + 81
				&& flight.y < enemy_h[i].y + 22 && enemy_h[i].y < flight.y + 36) {
				PostQuitMessage(0);
			}
		}
	}
	////////////////////////////내 진지랑 적이랑 충돌검사//////////////////////////
	/*for (int i = 0; i < 20; i++) {
		//적기 베이스랑 충돌
		if (enemy_f[i].shot) {
			if (base.x < enemy_f[i].x + 106 && enemy_f[i].x < base.x + 240) {
				PostQuitMessage(0);
			}
		}
		//적 탱크 베이스랑 충돌
		if (enemy_t[i].shot) {
			if (base.x < enemy_t[i].x + 106 && enemy_t[i].x < base.x + 293
				&& base.y < enemy_t[i].y + 32 && enemy_t[i].y < base.y + 388) {
				PostQuitMessage(0);
			}
		}
		//적 헬기 베이스랑 충돌
		if (enemy_h[i].shot) {
			if (base.x < enemy_h[i].x + 99 && enemy_h[i].x < base.x + 240
				&& base.y < enemy_h[i].y + 32 && enemy_h[i].y < base.y + 388) {
				PostQuitMessage(0);
			}
		}
	}*/
	draw();
}