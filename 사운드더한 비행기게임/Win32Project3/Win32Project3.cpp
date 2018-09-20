// Win32Project3.cpp : ���� ���α׷��� ���� �������� �����մϴ�.
//

#include "stdafx.h"
#include "Win32Project3.h"
#include "egl/egl.h"
#include "vg/openvg.h"
#include "vgLoadImage.h"
#include "DXUTsound.h"
#include <time.h>


#define MAX_LOADSTRING 100

// ���� ����:
HINSTANCE hInst;                                // ���� �ν��Ͻ��Դϴ�.
WCHAR szTitle[MAX_LOADSTRING];                  // ���� ǥ���� �ؽ�Ʈ�Դϴ�.
WCHAR szWindowClass[MAX_LOADSTRING];            // �⺻ â Ŭ���� �̸��Դϴ�.

EGLDisplay display;
EGLSurface surface;
EGLContext context;

CSoundManager* sm = NULL;

CSound* backgroundSound = NULL;

CSound* missileSound = NULL;

//�� ����
struct base {
	int x, y;
	int count;
}base;

//�� ���helo
struct flight {
	int x, y;
	int count;
}flight;

//�� ����� �̻��� bullet
struct missiles {
	int shot, x, y;
}missiles[20];

//��ũ�� ��źagm
struct bombs {
	int shot, x, y;
	float vx, vy;
}bombs[20];

//�� ���� �̻���aim
struct missiles_h {
	int x, y, shot;
}missiles_h[20];

//F22 ����
struct enemy_f{
	int x, y, shot;
}enemy_f[20];

//�� ��ũ
struct enemy_t {
	int x, y, shot;
}enemy_t[20];

//�� ���
struct enemy_h {
	int x, y, shot;
}enemy_h[20];
/*
//��ź����
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

// �� �ڵ� ��⿡ ��� �ִ� �Լ��� ������ �����Դϴ�.
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

    // TODO: ���⿡ �ڵ带 �Է��մϴ�.

    // ���� ���ڿ��� �ʱ�ȭ�մϴ�.
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_WIN32PROJECT3, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // ���� ���α׷� �ʱ�ȭ�� �����մϴ�.
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_WIN32PROJECT3));

    MSG msg;

    // �⺻ �޽��� �����Դϴ�.
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
//  �Լ�: MyRegisterClass()
//
//  ����: â Ŭ������ ����մϴ�.
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
//   �Լ�: InitInstance(HINSTANCE, int)
//
//   ����: �ν��Ͻ� �ڵ��� �����ϰ� �� â�� ����ϴ�.
//
//   ����:
//
//        �� �Լ��� ���� �ν��Ͻ� �ڵ��� ���� ������ �����ϰ�
//        �� ���α׷� â�� ���� ���� ǥ���մϴ�.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // �ν��Ͻ� �ڵ��� ���� ������ �����մϴ�.

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
//  �Լ�: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  ����:  �� â�� �޽����� ó���մϴ�.
//
//  WM_COMMAND  - ���� ���α׷� �޴��� ó���մϴ�.
//  WM_PAINT    - �� â�� �׸��ϴ�.
//  WM_DESTROY  - ���� �޽����� �Խ��ϰ� ��ȯ�մϴ�.
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // �޴� ������ ���� �м��մϴ�.
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
            // TODO: ���⿡ hdc�� ����ϴ� �׸��� �ڵ带 �߰��մϴ�.
            EndPaint(hWnd, &ps);
        }
        break;

	case WM_KEYDOWN:
		//������ �̻���
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
		//��ũ�� ��ź
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
		
		//���
		background = vgLoadImage(TEXT("bk.jpg"));
		
		//�츮 �� ����
		basecamp = vgLoadImage(TEXT("base.png"));
		base.x = 0;
		base.y = 10;

		//����� �ʱ���ġ
		plane = vgLoadImage(TEXT("helo.png"));
		flight.x = 300;
		flight.y = 150;
		flight.count = 0;
		
		//�����Լ�
		srand((unsigned)time(NULL));

		//����� �̻���
		missile = vgLoadImage(TEXT("bullet.png"));
		for (int i = 0; i < 20; i++) {
			missiles[i].shot = false;
		}
		//��ũ�� ��ź
		bomb = vgLoadImage(TEXT("agm.png"));
		for (int i = 0; i < 20; i++) {
			bombs[i].shot = false;
		}
		bomb_explosion = vgLoadImage(TEXT("explosion.png"));
		/*
		//��ź����
		for (int i = 0; i < 20; i++) {
			explosion[i].shot = false;
		}
		*/
		//���� �̻���
		missile_h = vgLoadImage(TEXT("aim.png"));
		for (int i = 0; i < 20; i++) {
			missiles_h[i].shot = false;
		}
		//����
		enemy_plane = vgLoadImage(TEXT("F22.png"));
		for (int i = 0; i < 20; i++) {
			enemy_f[i].shot = false;
		}
		//����ũ
		enemy_tank = vgLoadImage(TEXT("tank.png"));
		for (int i = 0; i < 20; i++) {
			enemy_t[i].shot = false;
		}
		//�����
		enemy_helicopter = vgLoadImage(TEXT("ah66.png"));
		for (int i = 0; i < 20; i++) {
			enemy_h[i].shot = false;
		}


		//���常���
		sm = new CSoundManager();
		sm->Initialize(hWnd, DSSCL_NORMAL);
		
		//��׶������
		sm->Create(&backgroundSound, TEXT("background_sound.wav"), 0, GUID_NULL);
		backgroundSound->Play(0, DSBPLAY_LOOPING);

		//�̻��ϻ���
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

// ���� ��ȭ ������ �޽��� ó�����Դϴ�.
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
//���
int cx=0, cy=0;
int k = cx / 1000;
*/

void draw()
{
	eglMakeCurrent(display, surface, surface, context);	
	vgSeti(VG_MATRIX_MODE, VG_MATRIX_IMAGE_USER_TO_SURFACE);
	/*
	// let��s draw k��th and k+1��th image ��� �ڵ� ��ũ��
	vgLoadIdentity(); vgTranslate(k*800 - cx, 0);
	vgDrawImage(background);
	vgLoadIdentity(); vgTranslate((k + 1)*800 - cx, 0);
	vgDrawImage(background);
	*/

	//��� �׸���
	vgLoadIdentity();
	vgDrawImage(background);
	
	//�� ���̽�ķ�� �׸���
	vgLoadIdentity();
	vgTranslate(base.x, base.y);
	vgDrawImage(basecamp);

	//�� ��� �׸���
	vgLoadIdentity();
	vgTranslate(flight.x, flight.y);
	vgDrawImage(plane);


	//�����̻��� �׸���
	for (int i = 0; i < 20; i++) {
		if (missiles[i].shot) {
			vgLoadIdentity();
			vgTranslate(missiles[i].x, missiles[i].y);
			vgDrawImage(missile);
		}
	}
	//��ũ����ź �׸���
	for (int i = 0; i < 20; i++) {
		if (bombs[i].shot) {
			vgLoadIdentity();
			vgTranslate(bombs[i].x, bombs[i].y);
			vgDrawImage(bomb);
		}
	}
	/*//��ź���� �׸���
	for (int i = 0; i < 20; i++) {
		if (!bombs[i].shot) {
			vgLoadIdentity();
			vgTranslate(bombs[i].x, bombs[i].y);
			vgDrawImage(bomb_explosion);
		}
	}*/
	//����̻��� �׸���
	for (int i = 0; i < 20; i++) {
		if (missiles_h[i].shot) {
			vgLoadIdentity();
			vgTranslate(missiles_h[i].x, missiles_h[i].y);
			vgDrawImage(missile_h);
		}
	}
	//����׸���
	for (int i = 0; i < 20; i++) {
		if (enemy_f[i].shot) {
			vgLoadIdentity();
			vgTranslate(enemy_f[i].x, enemy_f[i].y);
			vgDrawImage(enemy_plane);
		}
	}
	//����ũ�׸���
	for (int i = 0; i < 20; i++) {
		if (enemy_t[i].shot) {
			vgLoadIdentity();
			vgTranslate(enemy_t[i].x, enemy_t[i].y);
			vgDrawImage(enemy_tank);
		}
	}
	//�����׸���
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
	//����� �񵿱�� �����̱�
	if (GetAsyncKeyState(VK_LEFT) & 0x8000 && flight.x >= 300)  flight.x -= 10;
	if (GetAsyncKeyState(VK_RIGHT) & 0x8000 && flight.x <= 1000 - 100) flight.x += 10;
	if (GetAsyncKeyState(VK_UP) & 0x8000 && flight.y <= 560 - 110)  flight.y += 10;
	if (GetAsyncKeyState(VK_DOWN) & 0x8000 && flight.y >= 150) flight.y -= 10;
	 
	//����� �̻��� �񵿱��� �߻�
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
	//ȭ�� �����̱�
	cx  += 5;
	*/
	/////////////////////////////�� �̻��� �߻�/////////////////////////
	//����� �̻��� ������ ����
	for (int i = 0; i<20; i++) {
		if (missiles[i].shot) {
			missiles[i].x += 15;
			if (missiles[i].x>1000) missiles[i].shot = false;
		}
	}
	//�� ���� �̻��� ������ ����
	for (int i = 0; i<20; i++) {
		if (missiles_h[i].shot) {
			missiles_h[i].x += 10;
			if (missiles_h[i].x>1000) missiles_h[i].shot = false;
		}
	}
	//�� ��ũ�� ��ź ������ ������
	for (int i = 0; i<20; i++) {
		if (bombs[i].shot) {
			bombs[i].vy -= 1;
			bombs[i].y += bombs[i].vy;
			if (bombs[i].y < 45) {
				bombs[i].shot = false;
			}
		}
	}
	/////////////////////���� �������� �� ������///////////////////////
	//���� �������� 
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
	//���� �������� ����
	for (int i = 0; i < 20; i++) {
		if (enemy_f[i].shot) {
			enemy_f[i].x -= 15;
			if (enemy_f[i].x < 0)
				enemy_f[i].shot = false;
		}
	}
	
	//�� ��ũ ��������
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
	//�� ��ũ ���� ����
	for (int i = 0; i < 20; i++) {
		if (enemy_t[i].shot) {
			enemy_t[i].x -= 5;
			if (enemy_t[i].x < 0)
				enemy_t[i].shot = false;
		}
	}
	
	//����� ��������
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
	//�� ��� ���� ����
	for (int i = 0; i < 20; i++) {
		if (enemy_h[i].shot) {
			enemy_h[i].x -= 2;
			if (enemy_h[i].x < 0)
				enemy_h[i].shot = false;
		}
	}

	/////////////////////////////////////���̶� �̻���/��ź�̶� �浹�˻�////////////////////////////
	//���Ⱑ �Ѿ� Ȥ�� �̻��Ͽ� �ε�������
	for (int i = 0; i < 20; i++) {
		if (enemy_f[i].shot) {
			//����� �̻��� �浹
			for (int j = 0; j < 20; j++) {
				if (missiles[j].shot && missiles[j].x < enemy_f[i].x + 106 && enemy_f[i].x < missiles[j].x + 7
					&& missiles[j].y < enemy_f[i].y + 19 && enemy_f[i].y < missiles[j].y + 4) {
					missiles[j].shot = false;
					enemy_f[i].shot = false;
				}
			}
			//������ �̻��� �浹
			for (int j = 0; j < 20; j++) {
				if (missiles_h[j].shot && missiles_h[j].x < enemy_f[i].x + 106 && enemy_f[i].x < missiles_h[j].x + 60
					&& missiles_h[j].y < enemy_f[i].y + 19 && enemy_f[i].y < missiles_h[j].y + 11) {
					missiles_h[j].shot = false;
					enemy_f[i].shot = false;
				}
			}
		}
	}
	//��ũ�� ��ź �ε�������
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
	//����Ⱑ �̻��� �ε�������
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
	
	///////////////////////////////////�� ��� ���̶� �浹�˻�/////////////////////
	//���� ������ �ε�������
	for (int i = 0; i < 20; i++) {
		if (enemy_f[i].shot) {
			if (flight.x < enemy_f[i].x + 106  && enemy_f[i].x < flight.x + 81
				&& flight.y < enemy_f[i].y + 32 && enemy_f[i].y < flight.y + 36) {
				PostQuitMessage(0);
			}
		}
	}
	/*
	//����ũ ������ �ε�������
	for (int i = 0; i < 20; i++) {
		if (enemy_t[i].shot) {
			if (flight.x < enemy_t[i].x + 51 && enemy_t[i].x < flight.x + 81
				&& flight.y < enemy_t[i].y + 36 && enemy_t[i].y < flight.y + 36) {
				PostQuitMessage(0);
			}
		}
	}
	*/
	//����� ������ �ε�������
	for (int i = 0; i < 20; i++) {
		if (enemy_h[i].shot) {
			if (flight.x < enemy_h[i].x + 99 && enemy_h[i].x < flight.x + 81
				&& flight.y < enemy_h[i].y + 22 && enemy_h[i].y < flight.y + 36) {
				PostQuitMessage(0);
			}
		}
	}
	////////////////////////////�� ������ ���̶� �浹�˻�//////////////////////////
	/*for (int i = 0; i < 20; i++) {
		//���� ���̽��� �浹
		if (enemy_f[i].shot) {
			if (base.x < enemy_f[i].x + 106 && enemy_f[i].x < base.x + 240) {
				PostQuitMessage(0);
			}
		}
		//�� ��ũ ���̽��� �浹
		if (enemy_t[i].shot) {
			if (base.x < enemy_t[i].x + 106 && enemy_t[i].x < base.x + 293
				&& base.y < enemy_t[i].y + 32 && enemy_t[i].y < base.y + 388) {
				PostQuitMessage(0);
			}
		}
		//�� ��� ���̽��� �浹
		if (enemy_h[i].shot) {
			if (base.x < enemy_h[i].x + 99 && enemy_h[i].x < base.x + 240
				&& base.y < enemy_h[i].y + 32 && enemy_h[i].y < base.y + 388) {
				PostQuitMessage(0);
			}
		}
	}*/
	draw();
}