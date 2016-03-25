#ifndef _GY_PLATSELECTOR_H_
#define _GY_PLATSELECTOR_H_

#ifdef BX_PLATFORM_WINDOWS
#define GY_PLATINC_APP        "engine/plat/win/app.win.inl"
#define GY_PLATINC_ATOMIC     "engine/plat/win/atomic.win.inl"
#define GY_PLATINC_CLOCK      "engine/plat/win/clock.win.inl"
#define GY_PLATINC_DYNLIB     "engine/plat/win/dynlib.win.inl"
#define GY_PLATINC_ENGINE     "engine/plat/win/engine.win.inl"
#define GY_PLATINC_FILE       "engine/plat/win/file.win.inl"
#define GY_PLATINC_INPUT      "engine/plat/win/input.win.inl"
#define GY_PLATINC_LOGMGR     "engine/plat/win/logmgr.win.inl"
#define GY_PLATINC_NETMGR     "engine/plat/win/netmgr.win.inl"
#define GY_PLATINC_PHYSICS2D  "engine/plat/win/physics2d.box2d.inl"
#define GY_PLATINC_SOUNDMGR   "engine/plat/win/soundmgr.fmod.inl"
#define GY_PLATINC_TIMER      "engine/plat/win/timer.win.inl"
#else
#error Not platform specific files defines for this platform!
#endif

#ifdef GY_RENDER_D3D11
#define GY_PLATINC_RENDERER   "engine/plat/win/renderer.d3d11.inl"
#endif



#endif