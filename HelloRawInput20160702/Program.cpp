#include <windows.h>
#include <stdio.h>
#include <tchar.h>
#include <vector>

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
TCHAR szClassName[] = _T("HelloRawInput20160702");

HWND hWnd;

RAWINPUTDEVICE device;
std::vector<char> rawInputMessageData;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPreInst, LPSTR lpszCmdLine, int nCmdShow)
{
	MSG msg;
	WNDCLASS myProg;
	if (!hPreInst) {
		myProg.style			= CS_HREDRAW | CS_VREDRAW;
		myProg.lpfnWndProc		= WndProc;
		myProg.cbClsExtra		= 0;
		myProg.cbWndExtra		= 0;
		myProg.hInstance		= hInstance;
		myProg.hIcon			= NULL;
		myProg.hCursor			= LoadCursor(NULL, IDC_ARROW);
		myProg.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
		myProg.lpszMenuName		= NULL;
		myProg.lpszClassName	= szClassName;
		if (!RegisterClass(&myProg)) { return FALSE; }
	}
	hWnd = CreateWindow(szClassName,	// class��
		_T("Hello RawInput !! �f�o�C�X�̓��͂����o����BVC++2010Express�̃f�o�b�O�o�͂ɕ\������B"),		// �^�C�g��
		WS_OVERLAPPEDWINDOW,		// Style
		CW_USEDEFAULT,				// X
		CW_USEDEFAULT,				// Y
		CW_USEDEFAULT,				// Width
		CW_USEDEFAULT,				// Height
		NULL,						// �e�E�B���h�E�܂��̓I�[�i�[�E�B���h�E�̃n���h��
		NULL,						// ���j���[�n���h���܂��͎q�E�B���h�E ID
		hInstance,					// �A�v���P�[�V�����C���X�^���X�̃n���h��
		NULL						// �E�B���h�E�쐬�f�[�^
	);
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);
	
	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return (msg.wParam);
}

void OnRawInput(bool inForeground, HRAWINPUT hRawInput)
{
	UINT dataSize;
	GetRawInputData(hRawInput, RID_INPUT, NULL, &dataSize, sizeof(RAWINPUTHEADER));

	if(dataSize == 0) { return; }
	if(dataSize > rawInputMessageData.size()) {
		rawInputMessageData.resize(dataSize);
	}

	void* dataBuf = &rawInputMessageData[0];
	GetRawInputData(hRawInput, RID_INPUT, dataBuf, &dataSize, sizeof(RAWINPUTHEADER));

	const RAWINPUT *raw = (const RAWINPUT*)dataBuf;
	if (raw->header.dwType == RIM_TYPEMOUSE) {
		HANDLE deviceHandle = raw->header.hDevice;

		const RAWMOUSE& mouseData = raw->data.mouse;

		TCHAR dataStr[1024];
		memset(dataStr, NULL, 1024);
		_stprintf_s(dataStr, 1024, _T(
			"Mouse: Device=0x%08X, Flags=%04x, ButtonFlags=%d, ButtonData=%d, ulRawButtons=%d, X=%d, Y=%d, ulExtraInformation=%d\n"), 
			deviceHandle, 
			mouseData.usFlags, 
			mouseData.usButtonFlags, 
			(short)mouseData.usButtonData, 
			mouseData.ulRawButtons, 
			(LONG)mouseData.lLastX, 
			(LONG)mouseData.lLastY,
			mouseData.ulExtraInformation);
		OutputDebugString(dataStr);
	}
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg) {
		case WM_INPUT:
			OnRawInput(
				GET_RAWINPUT_CODE_WPARAM(wParam) == RIM_INPUT,
				(HRAWINPUT)lParam
			);
			return(DefWindowProc(hWnd, msg, wParam, lParam));
		case WM_MOUSEWHEEL:
			OutputDebugString(_T("WM_MOUSEWHEEL\n"));
			TCHAR buffer[1024];
			memset(buffer, NULL, 1024);
			_stprintf_s(buffer, 1024, _T("  wheel delta=%d\n"), GET_WHEEL_DELTA_WPARAM(wParam));
			OutputDebugString(buffer);
			break;
		// Windows XP �܂ł� tilt �͐�p�h���C�o�ɂ����WM_HSCROLL�𔭍s���Ă���ꍇ�������炵��
		case WM_HSCROLL:
			OutputDebugString(_T("WM_HSCROLL\n"));
			break;
		// Windows Vista ����� tilt �� WM_MOUSEHWHEEL �Ō��o�ł���炵��
		case WM_MOUSEHWHEEL:
			OutputDebugString(_T("WM_MOUSEHWHEEL\n"));
			break;
		case WM_CREATE:
			device.usUsagePage = 0x01;
			device.usUsage = 0x02;
			device.dwFlags = 0;
			device.hwndTarget = 0;
			RegisterRawInputDevices(&device, 1, sizeof device);
			break;
		case WM_DESTROY:
			device.usUsagePage = 0x01;
			device.usUsage = 0x02;
			device.dwFlags = RIDEV_REMOVE;
			device.hwndTarget = 0;
			RegisterRawInputDevices(&device, 1, sizeof device);
			PostQuitMessage(0);
			break;
		default:
			return(DefWindowProc(hWnd, msg, wParam, lParam));
	}
	return (0L);
}
