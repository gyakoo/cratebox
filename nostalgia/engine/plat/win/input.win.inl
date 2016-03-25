#include <engine/app/app.h>
#include <engine/common/timer.h>

#ifndef XINPUT_CAPS_FFB_SUPPORTED
#define XINPUT_CAPS_FFB_SUPPORTED 0x0001
#endif
int gyGetVirtualKeycodeFromIndex(gyInputIndex index);

////////////////////////////////////////////////////////////////////////////////////////////////////
// gyInputKeyboard
////////////////////////////////////////////////////////////////////////////////////////////////////
class gyInputKeyboard : public gyInputTrigger
{
public:
  gyInputKeyboard(gyInputIndex index);
  gy_override float Get();
  int vk;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
// gyInputMouse
////////////////////////////////////////////////////////////////////////////////////////////////////
class gyInputMouse : public gyInputTrigger
{
public:
  gyInputMouse(gyInputIndex index);
  gy_override float Get();
  short mb;
  short st;
};

enum eMouseSt{ GYMS_UP=0, GYMS_DOWN=1, GYMS_DBLCLICK=2, GYMS_WHEELUP=3, GYMS_WHEELDOWN=4 };
enum eMouseBut{ GYMB_LEFT=0, GYMB_RIGHT, GYMB_MIDDLE, GYMB_WHEEL, GYMB_EXTRA0, GYMB_EXTRA1, GYMB_EXTRA2, GYMB_EXTRA3, GYMB_MAX };

////////////////////////////////////////////////////////////////////////////////////////////////////
// gyInputPad
////////////////////////////////////////////////////////////////////////////////////////////////////
class gyInputPadButtonState : public gyInputTrigger
{
public:
  gyInputPadButtonState(gyInputIndex btn, int port);
  gy_override float Get();

  int xibutton;
  int port;
};

class gyInputPadAnalogState : public gyInputTrigger
{
public:
  gyInputPadAnalogState(gyInputIndex iindex, int port);
  gy_override float Get();

  gyInputIndex iindex;
  int port;
};

class gyInputPadPlugState : public gyInputTrigger
{
public:
  gyInputPadPlugState(gyInputIndex plugState, int port);
  gy_override float Get();

  gyInputIndex plugState;
  int port;
};

struct gyInputPadState
{
  XINPUT_STATE state;
  gyInputPadVibrationFade fade;
  bool connected;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
// gyInput
////////////////////////////////////////////////////////////////////////////////////////////////////
class gyInput::Impl: public gyCallbackHandler
{
public:  
  Impl();
  virtual ~Impl();
  void Init();
  void Destroy();
  void Recycle();
  gyInputTrigger* TriggerCreate( gyInputDevice devType, gyInputIndex iindex, const gyInputTriggerOpts& opts );
  gy_override void OnCallback( gyCallbackData* cbData );  
  bool IsDeviceConnected(gyInputDevice dev);
  bool GetPadVibrationSupport(gyInputDevice pad);
  int SetPadVibration(gyInputDevice pad, float left, float right, const gyInputPadVibrationFade* fade);

  // gamepad specific  
  void  UpdateGamepadState();

  void SetPadDeadZone(float deadZone){ padDeadZone=deadZone; }
  void SetPadDeadZoneEnabled(bool status){ padUseDeadZone = status; }
  void SetPadNormalizeAnalogEnabled(bool status){ normalizeAnalog = status; }
  bool IsPadDeadZoneEnabled(){ return padUseDeadZone; }
  bool IsPadNormalizeAnalogEnabled(){ return normalizeAnalog; }
  float GetPadDeadZone(){ return padDeadZone; }

  char curKeyState[256];
  char lastKeyState[256];
  char curMouseState[GYMB_MAX];
  char lastMouseState[GYMB_MAX];
  gyInputPadState curPadState[GY_INPUTDEV_MAXGAMEPADS];
  gyInputPadState lastPadState[GY_INPUTDEV_MAXGAMEPADS];
  float padDeadZone;
  bool padUseDeadZone;
  bool padInUse;
  bool normalizeAnalog;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
gyInput::Impl::Impl()
{
  ::ZeroMemory(curKeyState,sizeof(curKeyState));
  ::ZeroMemory(lastKeyState,sizeof(lastKeyState));
  ::ZeroMemory(curMouseState, sizeof(curMouseState));
  ::ZeroMemory(lastMouseState, sizeof(lastMouseState));
  ::ZeroMemory(curPadState, sizeof(curPadState));
  ::ZeroMemory(lastPadState, sizeof(lastPadState));
  // Default to 24% of the +/- 32767 range.   This is a reasonable default value but can be altered if needed.
  padDeadZone = .24f * float(0x7fff);
  padUseDeadZone = true;  
  padInUse = false;
  normalizeAnalog = false;
}

void gyInput::Impl::Init()
{
  gyApp::OnWindowMessage.Subscribe(this);
  AddRef();
}

void gyInput::Impl::Destroy()
{
  gyApp::OnWindowMessage.Unsubscribe(this);
  DecRef();
}

gyInput::Impl::~Impl()
{
  if ( padInUse )
    XInputEnable(false);  
}

void gyInput::Impl::Recycle()
{
  // so we always have current and last frame states
  char lastWheel = lastMouseState[GYMB_WHEEL];
  char& curWheel = curMouseState[GYMB_WHEEL];
  if ( lastWheel == GYMS_UP && lastWheel != curWheel )
    curWheel = GYMS_UP;

  memcpy(lastKeyState, curKeyState, sizeof(lastKeyState) );
  memcpy(lastMouseState, curMouseState, sizeof(lastMouseState));

  UpdateGamepadState();
}

void gyInput::Impl::OnCallback( gyCallbackData* cbData )
{
  gyAppWindowMessageData* wdata = static_cast<gyAppWindowMessageData*>(cbData);
  const UINT msg = (UINT)wdata->params[1];
  const WPARAM wParam = (WPARAM)wdata->params[2];

  switch ( msg  )
  {
  case WM_LBUTTONDOWN   : curMouseState[GYMB_LEFT] = GYMS_DOWN; break;
  case WM_LBUTTONUP     : curMouseState[GYMB_LEFT] = GYMS_UP; break;
  case WM_LBUTTONDBLCLK : curMouseState[GYMB_LEFT] = GYMS_DBLCLICK; break;
  case WM_RBUTTONDOWN   : curMouseState[GYMB_RIGHT] = GYMS_DOWN; break;
  case WM_RBUTTONUP     : curMouseState[GYMB_RIGHT] = GYMS_UP; break;
  case WM_RBUTTONDBLCLK : curMouseState[GYMB_RIGHT] = GYMS_DBLCLICK; break;
  case WM_MBUTTONDOWN   : curMouseState[GYMB_MIDDLE] = GYMS_DOWN; break;
  case WM_MBUTTONUP     : curMouseState[GYMB_MIDDLE] = GYMS_UP; break;
  case WM_MBUTTONDBLCLK : curMouseState[GYMB_MIDDLE] = GYMS_DBLCLICK; break;
  case WM_MOUSEWHEEL    :
    {
      int zDelta = GET_WHEEL_DELTA_WPARAM(wParam);
      if ( zDelta > 0 )
        curMouseState[GYMB_WHEEL] = GYMS_WHEELUP;
      else if ( zDelta < 0 )
        curMouseState[GYMB_WHEEL] = GYMS_WHEELDOWN;
      else
        curMouseState[GYMB_WHEEL] = GYMS_UP;
    }break;

  case WM_KEYDOWN: 
  case WM_KEYUP:
    {
      int vk = wParam & 0xff;
      curKeyState[vk] = WM_KEYUP-msg;
    }break;

  case WM_ACTIVATEAPP:
    XInputEnable((BOOL)wParam);
    break;
  case WM_DEVICECHANGE:
//     switch ( wParam )
//     {
//       case DBT_DEVICEARRIVAL:
//       case DBT_DEVICEREMOVECOMPLETE:
//       {
//         DEV_BROADCAST_HDR* header = reinterpret_cast<DEV_BROADCAST_HDR*>(wdata->params[3]);
//         if( header->dbch_devicetype == DBT_DEVTYP_DEVICEINTERFACE )
//           UpdateGamepadState();
//       }break;
//    }
  break;
  }
}

gyInputTrigger* gyInput::Impl::TriggerCreate( gyInputDevice devType, gyInputIndex iindex, const gyInputTriggerOpts& opts )
{
  gyInputTrigger* trigger=NULL;
  if ( !gyGetInputSystem().CheckCompatibilityDeviceIndex(devType, iindex) )
  {
    FAIL( "Invalid combination device type and input index" );
    return NULL;
  }
  switch ( devType )
  {
    case GY_INPUTDEV_KEYBOARD : trigger = new gyInputKeyboard(iindex); break;  
    case GY_INPUTDEV_MOUSE    : trigger = new gyInputMouse(iindex); break;
    case GY_INPUTDEV_GAMEPAD_0: 
    case GY_INPUTDEV_GAMEPAD_1:
    case GY_INPUTDEV_GAMEPAD_2:
    case GY_INPUTDEV_GAMEPAD_3:
      {
        int padPort = devType-GY_INPUTDEV_GAMEPAD_0;
        switch(iindex)
        {
          case GYII_PAD_UNPLUGGED:
          case GYII_PAD_PLUGGED:
            trigger = new gyInputPadPlugState(iindex,padPort);
            break;
          case GYII_PAD_LTRIGGER:
          case GYII_PAD_RTRIGGER:
          case GYII_PAD_LSTICK_X:
          case GYII_PAD_LSTICK_Y:
          case GYII_PAD_RSTICK_X:
          case GYII_PAD_RSTICK_Y:
            trigger = new gyInputPadAnalogState(iindex,padPort);
            break;
          default:
            trigger = new gyInputPadButtonState(iindex,padPort); 
        }        
        if ( !padInUse )
        {
          padInUse = true; 
          XInputEnable(true);
        }
      }break;
  }

  if ( trigger )
  {
    trigger->opts = opts;
    uint32_t data[]={ devType, iindex, *reinterpret_cast<const uint32_t*>(&opts.deadZone), (uint32_t)opts.once, (uint32_t)opts.oncePerFrame };
    trigger->hash = gyComputeHash(data, sizeof(data));
  }
  return trigger;
}

void gyInput::Impl::UpdateGamepadState()
{
  memcpy(lastPadState, curPadState, sizeof(lastPadState) );
  DWORD dwResult;
  for( DWORD i = 0; i < GY_INPUTDEV_MAXGAMEPADS; i++ )
  {
    // Simply get the state of the controller from XInput.
    dwResult = XInputGetState( i, &curPadState[i].state );

    if( dwResult == ERROR_SUCCESS )
      curPadState[i].connected = true;
    else
    {
      curPadState[i].connected = false;
      curPadState[i].fade.timeInSec = -1.0f;
      continue;
    }

    // update fade    
    gyInputPadVibrationFade& fade = curPadState[i].fade;
    if ( fade.curTime > 0.0f )
    {
      gyv2f outFade;
      fade.Update(gyGetTimer().GetFrameTime(), outFade);
      XINPUT_VIBRATION vib={ WORD(outFade.x*0xffff), WORD(outFade.y*0xffff) };
      XInputSetState(i,&vib);
    }

    // If using dead zone, put 0 these values if they're in the dead-zone threshold
    XINPUT_GAMEPAD& gp = curPadState[i].state.Gamepad;
    if( padUseDeadZone )
    {
      // Zero value if thumbsticks are within the dead zone 
      if( ( gp.sThumbLX < padDeadZone && gp.sThumbLX > -padDeadZone ) &&
        ( gp.sThumbLY < padDeadZone && gp.sThumbLY > -padDeadZone ) )
      {
        gp.sThumbLX = 0;
        gp.sThumbLY = 0;
      }

      if( ( gp.sThumbRX < padDeadZone && gp.sThumbRX > -padDeadZone ) &&
        ( gp.sThumbRY < padDeadZone && gp.sThumbRY > -padDeadZone ) )
      {
        gp.sThumbRX = 0;
        gp.sThumbRY = 0;
      }
    }
  }
}

bool gyInput::Impl::IsDeviceConnected(gyInputDevice dev)
{
  // let's assume keyb and mouse are present under win // todo: detect this correctly
  switch ( dev )
  {
    case GY_INPUTDEV_KEYBOARD:
    case GY_INPUTDEV_MOUSE: return true;
    case GY_INPUTDEV_GAMEPAD_0:
    case GY_INPUTDEV_GAMEPAD_1:
    case GY_INPUTDEV_GAMEPAD_2:
    case GY_INPUTDEV_GAMEPAD_3: return curPadState[dev-GY_INPUTDEV_GAMEPAD_0].connected;
  }
  return false;
}

bool gyInput::Impl::GetPadVibrationSupport(gyInputDevice pad)
{
  switch ( pad )
  {
  case GY_INPUTDEV_GAMEPAD_0:
  case GY_INPUTDEV_GAMEPAD_1:
  case GY_INPUTDEV_GAMEPAD_2:
  case GY_INPUTDEV_GAMEPAD_3:
    {
      int port = pad-GY_INPUTDEV_GAMEPAD_0;
      if ( !curPadState[port].connected ) 
        return false;
      XINPUT_VIBRATION vib={ 0, 0 };
      return XInputSetState(port,&vib)==ERROR_SUCCESS;
    }break;
  }
  return false;
}

int gyInput::Impl::SetPadVibration(gyInputDevice pad, float left, float right, const gyInputPadVibrationFade* fade)
{
  switch ( pad )
  {
  case GY_INPUTDEV_GAMEPAD_0:
  case GY_INPUTDEV_GAMEPAD_1:
  case GY_INPUTDEV_GAMEPAD_2:
  case GY_INPUTDEV_GAMEPAD_3:
    {
      int port = pad-GY_INPUTDEV_GAMEPAD_0;
      if ( !curPadState[port].connected ) return -1;
      left = gyMath::Saturate(left);
      right= gyMath::Saturate(right);
      gyInputPadVibrationFade& padFade = curPadState[port].fade;
      if ( fade && GetPadVibrationSupport(pad) )
      {
        padFade.src.Set(left,right);
        padFade.tgt.Set(gyMath::Saturate(fade->tgt.x), gyMath::Saturate(fade->tgt.y));
        padFade.timeInSec = padFade.curTime = fade->timeInSec;
      }
      else
      {
        padFade.Invalidate();
      }
      XINPUT_VIBRATION vib={ WORD(left*0xffff), WORD(right*0xffff) };
      return XInputSetState(port,&vib)==ERROR_SUCCESS ? R_OK : -1;
    }break;
  }
  return -1;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// GAMEPAD
////////////////////////////////////////////////////////////////////////////////////////////////////
gyInputPadButtonState::gyInputPadButtonState(gyInputIndex btn, int port)
  : port(port)
{
  switch ( btn )
  {
  case GYII_PAD_START         : xibutton = XINPUT_GAMEPAD_START; break;
  case GYII_PAD_BACK_PAUSE    : xibutton = XINPUT_GAMEPAD_BACK; break;
  case GYII_PAD_LPAD_DOWN     : xibutton = XINPUT_GAMEPAD_DPAD_DOWN; break;
  case GYII_PAD_LPAD_UP       : xibutton = XINPUT_GAMEPAD_DPAD_UP; break;
  case GYII_PAD_LPAD_LEFT     : xibutton = XINPUT_GAMEPAD_DPAD_LEFT; break;
  case GYII_PAD_LPAD_RIGHT    : xibutton = XINPUT_GAMEPAD_DPAD_RIGHT; break;
  case GYII_PAD_RPAD_DOWN     : xibutton = XINPUT_GAMEPAD_A; break;
  case GYII_PAD_RPAD_UP       : xibutton = XINPUT_GAMEPAD_Y; break;
  case GYII_PAD_RPAD_LEFT     : xibutton = XINPUT_GAMEPAD_X; break;
  case GYII_PAD_RPAD_RIGHT    : xibutton = XINPUT_GAMEPAD_B; break;
  case GYII_PAD_LSTICK_BUTTON : xibutton = XINPUT_GAMEPAD_LEFT_THUMB; break;
  case GYII_PAD_RSTICK_BUTTON : xibutton = XINPUT_GAMEPAD_RIGHT_THUMB; break;
  case GYII_PAD_LSHOULDER     : xibutton = XINPUT_GAMEPAD_LEFT_SHOULDER; break;
  case GYII_PAD_RSHOULDER     : xibutton = XINPUT_GAMEPAD_RIGHT_SHOULDER; break;
  default: FAIL_ALWAYS( "Invalid index for an input PAD" ); 
  }
}

float gyInputPadButtonState::Get()
{
  gyInput::Impl* impl = gyGetInputSystem().GetImpl();
  const gyInputPadState& state = impl->curPadState[port];
  int curState = state.state.Gamepad.wButtons & xibutton;
  int lastState= impl->lastPadState[port].state.Gamepad.wButtons & xibutton;
  if ( !state.connected )
    return 0.0f;
  if ( opts.once )
    return (curState!=0 && lastState==0) ? 1.0f : 0.0f;
  else
    return curState!=0 ? 1.0f : 0.0f;
}

gyInputPadAnalogState::gyInputPadAnalogState(gyInputIndex iindex, int port)
  : iindex(iindex), port(port)
{
}

float gyInputPadAnalogState::Get()
{
  gyInput::Impl* impl = gyGetInputSystem().GetImpl();
  const gyInputPadState& state = impl->curPadState[port];
  if ( !state.connected ) return 0.0f;
  const XINPUT_GAMEPAD& gp = state.state.Gamepad;
  float retValue = 0.0f;
  float maxValue = 1.0f;
  switch ( iindex )
  {
    case GYII_PAD_LTRIGGER: retValue = float(gp.bLeftTrigger); maxValue = 255.0f; break;
    case GYII_PAD_RTRIGGER: retValue = float(gp.bRightTrigger); maxValue= 255.0f; break;
    case GYII_PAD_LSTICK_X: retValue = float(gp.sThumbLX); maxValue = float(0x7fff); break;
    case GYII_PAD_LSTICK_Y: retValue = float(gp.sThumbLY); maxValue = float(0x7fff); break;
    case GYII_PAD_RSTICK_X: retValue = float(gp.sThumbRX); maxValue = float(0x7fff); break;
    case GYII_PAD_RSTICK_Y: retValue = float(gp.sThumbRY); maxValue = float(0x7fff); break;
  }
  if ( impl->normalizeAnalog )
    retValue /= maxValue;
  return retValue;
}

gyInputPadPlugState::gyInputPadPlugState(gyInputIndex plugState, int port)
  : plugState(plugState), port(port)
{
  FAIL_IF( plugState != GYII_PAD_PLUGGED && plugState != GYII_PAD_UNPLUGGED, "invalid plug state" );
}

float gyInputPadPlugState::Get()
{
  gyInput::Impl* impl = gyGetInputSystem().GetImpl();

  const gyInputPadState& curState = impl->curPadState[port];
  const gyInputPadState& lastState = impl->lastPadState[port];
  if ( plugState == GYII_PAD_PLUGGED )
    return !lastState.connected && curState.connected;
  else //unplugged
    return lastState.connected && !curState.connected;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// MOUSE
////////////////////////////////////////////////////////////////////////////////////////////////////
gyInputMouse::gyInputMouse(gyInputIndex index)
{
  switch ( index )
  {
  case GYII_MO_LEFT_BUTTON_DOWN : mb = GYMB_LEFT; st = GYMS_DOWN; break;
  case GYII_MO_LEFT_BUTTON_UP   : mb = GYMB_LEFT; st = GYMS_UP; break;
  case GYII_MO_LEFT_DBLCLICK    : mb = GYMB_LEFT; st = GYMS_DBLCLICK; break;
  case GYII_MO_RIGHT_BUTTON_DOWN: mb = GYMB_RIGHT; st = GYMS_DOWN; break;
  case GYII_MO_RIGHT_BUTTON_UP  : mb = GYMB_RIGHT; st = GYMS_UP; break;
  case GYII_MO_RIGHT_DBLCLICK   : mb = GYMB_RIGHT; st = GYMS_DBLCLICK; break;
  case GYII_MO_MIDDLE_BUTTON_DOWN : mb = GYMB_MIDDLE; st = GYMS_DOWN; break;
  case GYII_MO_MIDDLE_BUTTON_UP   : mb = GYMB_MIDDLE; st = GYMS_UP; break;
  case GYII_MO_MIDDLE_DBLCLICK    : mb = GYMB_MIDDLE; st = GYMS_DBLCLICK; break;
  case GYII_MO_WHEEL_UP           : mb = GYMB_WHEEL; st = GYMS_WHEELUP; break;
  case GYII_MO_WHEEL_DOWN         : mb = GYMB_WHEEL; st = GYMS_WHEELDOWN; break;
  default: FAIL( "index not for mouse");
  }
}

float gyInputMouse::Get()
{
  gyInput::Impl* impl = gyGetInputSystem().GetImpl();
  return (impl->curMouseState[mb] == st) ? 1.0f : 0.0f;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// KEYBOARD
////////////////////////////////////////////////////////////////////////////////////////////////////
gyInputKeyboard::gyInputKeyboard(gyInputIndex index)
{
  this->vk = gyGetVirtualKeycodeFromIndex(index);
}

float gyInputKeyboard::Get() 
{
  gyInput::Impl* impl = gyGetInputSystem().GetImpl();
  char lastSt = impl->lastKeyState[vk];
  char curSt = impl->curKeyState[vk];
  if ( opts.once )
    return ( lastSt == GYMS_UP && curSt == GYMS_DOWN ) ? 1.0f : 0.0f;
  return (curSt!=GYMS_UP) ? 1.0f : 0.0f;
}

int gyGetVirtualKeycodeFromIndex(gyInputIndex index)
{
  if ( index < GYII_KB_FIRST_ELEMENT || index > GYII_KB_LAST_ELEMENT )
    return 0;
  switch(index)
  {
    case GYII_KB_LSHIFT : return VK_LSHIFT;
    case GYII_KB_RSHIFT : return VK_RSHIFT;
    case GYII_KB_LCTRL  : return VK_LCONTROL;
    case GYII_KB_RCTRL  : return VK_RCONTROL;
    case GYII_KB_LALT   : return VK_MENU;
    case GYII_KB_RALT   : return VK_RMENU;
    case GYII_KB_BACKSP : return VK_BACK;
    case GYII_KB_TAB    : return VK_TAB;
    case GYII_KB_LWIN   : return VK_LWIN;
    case GYII_KB_RWIN   : return VK_RWIN;
    case GYII_KB_APPS   : return VK_APPS;
    case GYII_KB_ENTER  : return VK_RETURN;
    case GYII_KB_LCOM   :
    case GYII_KB_RCOM   : 
    case GYII_KB_MAC    :
    case GYII_KB_LMETA  : 
    case GYII_KB_RMETA  : return 0;
    case GYII_KB_INS    : return VK_INSERT;
    case GYII_KB_DEL    : return VK_DELETE;
    case GYII_KB_HOME   : return VK_HOME;
    case GYII_KB_END    : return VK_END;
    case GYII_KB_PGUP   : return VK_PRIOR;
    case GYII_KB_PGDN   : return VK_NEXT;
    case GYII_KB_ESC    : return VK_ESCAPE;
    case GYII_KB_UP     : return VK_UP;
    case GYII_KB_DOWN   : return VK_DOWN;
    case GYII_KB_LEFT   : return VK_LEFT;
    case GYII_KB_RIGHT  : return VK_RIGHT;
    case GYII_KB_SPACE  : return VK_SPACE;
    case GYII_KB_COMMA  : return VK_OEM_COMMA;
    case GYII_KB_PERIOD : return VK_OEM_PERIOD;
    case GYII_KB_MINUS  : return VK_OEM_MINUS;
    case GYII_KB_GRAVE  : return 0; // TODO
    case GYII_KB_0      : 
    case GYII_KB_1      : 
    case GYII_KB_2      : 
    case GYII_KB_3      : 
    case GYII_KB_4      : 
    case GYII_KB_5      : 
    case GYII_KB_6      :
    case GYII_KB_7      : 
    case GYII_KB_8      :
    case GYII_KB_9      : return "0123456789"[index-GYII_KB_0];
    case GYII_KB_CAPS   : return VK_CAPITAL;
    case GYII_KB_SCROLL : return VK_SCROLL;
    case GYII_KB_NUM    : return VK_NUMLOCK;
    case GYII_KB_PRSCR  : return VK_SNAPSHOT;
    case GYII_KB_PAUSE  : return VK_PAUSE;
    case GYII_KB_A      : 
    case GYII_KB_B      :
    case GYII_KB_C      :
    case GYII_KB_D      :
    case GYII_KB_E      :
    case GYII_KB_F      :
    case GYII_KB_G      :
    case GYII_KB_H      :
    case GYII_KB_I      :
    case GYII_KB_J      :
    case GYII_KB_K      :
    case GYII_KB_L      :
    case GYII_KB_M      :
    case GYII_KB_N      :
    case GYII_KB_O      :
    case GYII_KB_P      :
    case GYII_KB_Q      :
    case GYII_KB_R      :
    case GYII_KB_S      :
    case GYII_KB_T      :
    case GYII_KB_U      :
    case GYII_KB_V      :
    case GYII_KB_W      :
    case GYII_KB_X      :
    case GYII_KB_Y      :
    case GYII_KB_Z      : return "ABCDEFGHIJKLMNOPQRSTUVWXYZ"[index-GYII_KB_A];
    case GYII_KB_KP_SLASH : return VK_DIVIDE;
    case GYII_KB_KP_MUL   : return VK_MULTIPLY;
    case GYII_KB_KP_MINUS : return VK_OEM_MINUS; // TODO
    case GYII_KB_KP_PLUS  : return VK_ADD;
    case GYII_KB_KP_ENTER : return VK_RETURN; //TODO
    case GYII_KB_KP_PERIOD: return VK_DECIMAL;
    
    case GYII_KB_KP_0 : return VK_NUMPAD0;
    case GYII_KB_KP_1 : return VK_NUMPAD1;
    case GYII_KB_KP_2 : return VK_NUMPAD2;
    case GYII_KB_KP_3 : return VK_NUMPAD3;
    case GYII_KB_KP_4 : return VK_NUMPAD4;
    case GYII_KB_KP_5 : return VK_NUMPAD5;
    case GYII_KB_KP_6 : return VK_NUMPAD6;
    case GYII_KB_KP_7 : return VK_NUMPAD7;
    case GYII_KB_KP_8 : return VK_NUMPAD8;
    case GYII_KB_KP_9 : return VK_NUMPAD9;

    case GYII_KB_F1   : return VK_F1;
    case GYII_KB_F2   : return VK_F2;
    case GYII_KB_F3   : return VK_F3;
    case GYII_KB_F4   : return VK_F4;
    case GYII_KB_F5   : return VK_F5;
    case GYII_KB_F6   : return VK_F6;
    case GYII_KB_F7   : return VK_F7;
    case GYII_KB_F8   : return VK_F8;
    case GYII_KB_F9   : return VK_F9;
    case GYII_KB_F10  : return VK_F10;
    case GYII_KB_F11  : return VK_F11;
    case GYII_KB_F12  : return VK_F12;

    case GYII_KB_EQUAL        : return VK_OEM_PLUS;
    case GYII_KB_EQUAL_LSQBRK : return VK_OEM_4;
    case GYII_KB_EQUAL_RSQBRK : return VK_OEM_6;
    case GYII_KB_EQUAL_SEMICL : return VK_OEM_1;
    case GYII_KB_EQUAL_APOSTR : return VK_OEM_7;
    case GYII_KB_EQUAL_BACKSL : return VK_OEM_5;
    case GYII_KB_EQUAL_SLASH  : return VK_OEM_2;

    case GYII_KB_DE_SS : return VK_OEM_4;
    case GYII_KB_DE_ACCENT : return VK_OEM_6;
    case GYII_KB_DE_UE : return VK_OEM_1;
    case GYII_KB_DE_PLUS  : return VK_OEM_PLUS;
    case GYII_KB_DE_OE : return VK_OEM_3;
    case GYII_KB_DE_AE : return VK_OEM_7;
    case GYII_KB_DE_HASH : return VK_OEM_2;
    case GYII_KB_DE_LT : return VK_OEM_102;
    case GYII_KB_DE_CIRC : return VK_OEM_5;
  }
  return 0;
}