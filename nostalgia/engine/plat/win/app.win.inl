#include <stdio.h>
#include <engine/common/timer.h>
#include <engine/input/input.h>
#include <dbt.h>

static LRESULT CALLBACK gyWndProc( HWND , UINT , WPARAM , LPARAM );

class gyApp::Impl
{
public:
  Impl();
  int Create(const gyVarDict& createParams);
  int Run(gyApp& app);
  int InitWindow(int w, int h);
  void Destroy();
  void Terminate();
  void SetWindowTitle(const char* title);  
  void InitDevNotification();
  void DeInitDevNotification();

  char className[64];  
  HWND hWnd;
  HINSTANCE hInstance;
  HDEVNOTIFY hDevNotify;
  bool terminated;
};

// *****************************************************************************
// *****************************************************************************
gyApp::Impl::Impl(): hWnd(0), hInstance(0), terminated(false), hDevNotify(0)
{
  *className=0;
}
int gyApp::Impl::Create(const gyVarDict& createParams)
{
  int w = createParams.GetXPath("window.width",800);
  int h = createParams.GetXPath("window.height",600);
  if ( InitWindow(w,h) != R_OK )
    return -1;  
  bool fs = createParams.GetXPath("window.windowed",1).GetI() != 1;

  gyGetCommonDict().Set("win_hwnd",(int64_t)this->hWnd);
  gyGetCommonDict().Set("win_width", w);
  gyGetCommonDict().Set("win_height", h);
  gyGetCommonDict().Set("win_fs", fs);
  InitDevNotification();
  return R_OK;
}

int gyApp::Impl::InitWindow(int w, int h)
{
  sprintf_s( className, "gyAppClass%d", rand() );

  // Register class
  WNDCLASSEXA wcex;
  wcex.cbSize = sizeof( WNDCLASSEX );
  wcex.style = CS_HREDRAW | CS_VREDRAW;
  wcex.lpfnWndProc = gyWndProc;
  wcex.cbClsExtra = 0;
  wcex.cbWndExtra = 0;
  wcex.hInstance = (HINSTANCE)GetModuleHandle(NULL);
  wcex.hIcon = NULL;//LoadIcon( hInstance, ( LPCTSTR )IDI_TUTORIAL1 );
  wcex.hCursor = LoadCursor( NULL, IDC_ARROW );
  wcex.hbrBackground = ( HBRUSH )( COLOR_WINDOW + 1 );
  wcex.lpszMenuName = NULL;
  wcex.lpszClassName = className;
  wcex.hIconSm = NULL;//LoadIcon( wcex.hInstance, ( LPCTSTR )IDI_TUTORIAL1 );
  if( !RegisterClassExA( &wcex ) )
    return -1;

  // Create window
  this->hInstance = wcex.hInstance;
  RECT rc = { 0, 0, w, h };
  AdjustWindowRect( &rc, WS_OVERLAPPEDWINDOW, FALSE );  
  this->hWnd = CreateWindowA( className, "gyApp", WS_OVERLAPPEDWINDOW,
                              CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, 
                              rc.bottom - rc.top, NULL, NULL, hInstance, NULL );
  if( !hWnd )
    return -1;

  ShowWindow( hWnd, SW_SHOWDEFAULT );

//   RAWINPUTDEVICE rid[1];
//   rid[0].usUsagePage = 0x1;
//   rid[0].usUsage = 0x06;
//   rid[0].dwFlags = RIDEV_NOLEGACY;
//   rid[0].hwndTarget = hWnd;
//   if ( RegisterRawInputDevices(rid,1,sizeof(RAWINPUTDEVICE)) == FALSE )
//     return -1;

  return R_OK;
}

void gyApp::Impl::Destroy()
{
  int gwl=
#ifdef _WIN64
    GWLP_WNDPROC;
#else
    GWL_WNDPROC;
#endif
  DeInitDevNotification();
  SetWindowLong(hWnd, gwl, (LONG)gyWndProc);
  // todo: renderer release
  DestroyWindow(hWnd);
  hWnd = NULL;
  UnregisterClassA(className,hInstance);
  hInstance = NULL;  
}

void gyApp::Impl::SetWindowTitle(const char* title)
{
  if ( hWnd == NULL ) return;
  ::SetWindowTextA( hWnd, title );
}

// *****************************************************************************
gyAppWindowMessageData wmd;
LRESULT CALLBACK gyWndProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
  PAINTSTRUCT ps;
  HDC hdc;
  wmd.params[0] = (uint64_t)hWnd;
  wmd.params[1] = (uint64_t)message;
  wmd.params[2] = (uint64_t)wParam;
  wmd.params[3] = (uint64_t)lParam;
  gyApp::OnWindowMessage.Trigger(&wmd);
  switch( message )
  {
  case WM_PAINT:
    hdc = BeginPaint( hWnd, &ps );
    EndPaint( hWnd, &ps );
    break;

  case WM_CLOSE:
  case WM_DESTROY:
    PostQuitMessage( 0 );
    break;  
//   case WM_INPUT:
//     {
//       UINT dwSize;
//       GetRawInputData((HRAWINPUT)lParam, RID_INPUT, NULL, &dwSize, sizeof(RAWINPUTHEADER));
//       LPBYTE lpb = new BYTE[dwSize];
//       GetRawInputData((HRAWINPUT)lParam, RID_INPUT, lpb, &dwSize, sizeof(RAWINPUTHEADER));
//       RAWINPUT* raw = (RAWINPUT*)lpb;
//       if (raw->header.dwType == RIM_TYPEKEYBOARD) 
//       {
//         char szTmpOutput[256];
//         sprintf_s(szTmpOutput," Kbd: make=%04x Flags:%04x Reserved:%04x ExtraInformation:%08x, msg=%04x VK=%04x \n", 
//                                 raw->data.keyboard.MakeCode, 
//                                 raw->data.keyboard.Flags, 
//                                 raw->data.keyboard.Reserved, 
//                                 raw->data.keyboard.ExtraInformation, 
//                                 raw->data.keyboard.Message, 
//                                 raw->data.keyboard.VKey);
//         OutputDebugString(szTmpOutput);
//       }
//       delete[] lpb;
//     }break;
  default:
    return DefWindowProc( hWnd, message, wParam, lParam );
  }

  return R_OK;
}

int gyApp::Impl::Run(gyApp& app)
{
  if ( terminated )
    return -1;

  gyTimer& timer = gyGetTimer();
  gyInput& input = gyGetInputSystem();
  input.Recycle();

  // Main message loop
  MSG msg = {0};  
  if( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ) )
  {
    TranslateMessage( &msg );
    DispatchMessage( &msg );
  }

  if ( msg.message == WM_DESTROY || msg.message == WM_QUIT )
    Terminate();

  app.OnRun();

  timer.Update();

  return R_OK;
}

void gyApp::Impl::Terminate()
{
  terminated = true;  
}

void gyApp::Impl::InitDevNotification()
{
  if ( hDevNotify )
    return;

  // HID-GUID (http://msdn.microsoft.com/en-us/library/windows/hardware/ff545860%28v=vs.85%29.aspx)
  const GUID GUID_DEVINTERFACE_HID = { 0x4D1E55B2, 0xF16F, 0x11CF, { 0x88, 0xCB, 0x00, 0x11, 0x11, 0x00, 0x00, 0x30 } };

  // A notification filter will restrict the types of device plugging notifications that WndProc will receive
  DEV_BROADCAST_DEVICEINTERFACE notificationFilter;
  ZeroMemory(&notificationFilter, sizeof(notificationFilter));
  notificationFilter.dbcc_devicetype = DBT_DEVTYP_DEVICEINTERFACE; // we only want to be notified about devices
  notificationFilter.dbcc_size = sizeof(DEV_BROADCAST_DEVICEINTERFACE);  
  notificationFilter.dbcc_classguid = GUID_DEVINTERFACE_HID; // we only want to be notified about HID devices

  if(hWnd)
    hDevNotify = RegisterDeviceNotification(hWnd, &notificationFilter, DEVICE_NOTIFY_WINDOW_HANDLE);
}

void gyApp::Impl::DeInitDevNotification()
{
  if ( hDevNotify )
  {
    UnregisterDeviceNotification(hDevNotify);
    hDevNotify = NULL;
  }
}
