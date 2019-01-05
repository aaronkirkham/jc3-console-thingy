#include <Windows.h>

#include "hooking/hooking.h"
#include "xinput9_1_0.h"

#include "graphics.h"
#include "input.h"
#include "util.h"

#include "game/clock.h"
#include "game/device.h"
#include "game/game_object.h"
#include "game/player_manager.h"
#include "game/spawn_system.h"

#include "commands/event.h"
#include "commands/spawn.h"
#include "commands/world.h"

WNDPROC WndProc_orig = nullptr;
LRESULT WINAPI WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    auto game_state = *(uint32_t *)0x142D6F8AC;
    auto clock      = &jc::Base::CClock::instance();

    if (game_state == 15 && clock) {
        if (!clock->m_paused) {
            if (Input::Get()->WndProc(uMsg, wParam, lParam)) {
                return 0;
            }
        }
        // disable input if the clock is paused and input is still enabled somehow
        else if (Input::Get()->IsInputEnabled()) {
            Input::Get()->EnableInput(false);
        }
    }

    return CallWindowProc(WndProc_orig, hwnd, uMsg, wParam, lParam);
}

static uintptr_t D3D11CreateDevice_orig = 0;
HRESULT          D3D11CreateDevice(IDXGIAdapter *pAdapter, D3D_DRIVER_TYPE DriverType, HMODULE Software, UINT Flags,
                                   const D3D_FEATURE_LEVEL *pFeatureLevels, UINT FeatureLevels, UINT SDKVersion,
                                   ID3D11Device **ppDevice, D3D_FEATURE_LEVEL *pFeatureLevel,
                                   ID3D11DeviceContext **ppImmediateContext)
{
    static std::once_flag flag;
    std::call_once(flag, [] {
        Input::Get()->RegisterCommand(std::make_unique<EventCommand>());
        Input::Get()->RegisterCommand(std::make_unique<SpawnCommand>());
        Input::Get()->RegisterCommand(std::make_unique<WorldCommand>());
#ifdef DEBUG
        Input::Get()->RegisterCommand("exit",
                                      [](const std::string &arguments) { TerminateProcess(GetCurrentProcess(), -1); });
#endif

#ifdef DEBUG
        bool quick_start = true;
#else
        bool quick_start = (strstr(GetCommandLine(), "-quickstart") != nullptr);
#endif

        // enable quick start
        if (quick_start) {
            // quick start
            hk::put<bool>(0x142F3407A, true);

            // IsIntroSequenceComplete always returns true
            hk::put<uint32_t>(0x144883EE0, 0x90C301B0);

            // IsIntroMovieComplete always returns true
            hk::put<uint32_t>(0x144883F60, 0x90C301B0);
        }

        HWND hwnd    = *(HWND *)0x142E22A18;
        WndProc_orig = (WNDPROC)SetWindowLongPtr(hwnd, GWLP_WNDPROC, (LONG_PTR)WndProc);

        // focus lost
        static hk::inject_call<void, HWND, bool> lose_focus(0x143218620);
        lose_focus.inject([](HWND hwnd, bool a2) {
            lose_focus.call(hwnd, a2);
            Input::Get()->FocusChanged(true);
        });

        // focus gained
        static hk::inject_call<void, HWND> gain_focus(0x143218611);
        gain_focus.inject([](HWND hwnd) {
            gain_focus.call();
            Input::Get()->FocusChanged(false);
        });

        static hk::inject_call<int64_t, jc::HDevice_t *> flip(0x1432E0071);
        flip.inject([](jc::HDevice_t *device) {
            Graphics::Get()->BeginDraw(device);

            // draw input
            Input::Get()->Draw();

            Graphics::Get()->EndDraw();

            return flip.call(device);
        });
    });

    return ((decltype(D3D11CreateDevice) *)D3D11CreateDevice_orig)(pAdapter, DriverType, Software, Flags,
                                                                   pFeatureLevels, FeatureLevels, SDKVersion, ppDevice,
                                                                   pFeatureLevel, ppImmediateContext);
}

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
    // exit if not jc3
    if (!GetModuleHandle("JustCause3.exe")) {
        return FALSE;
    }

    if (fdwReason == DLL_PROCESS_ATTACH) {
        // allocate a section for hooking things
        VirtualAlloc((LPVOID)0x0000000160000000, 0x6000000, MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE);

        char buffer[MAX_PATH] = {0};
        GetSystemDirectoryA(buffer, MAX_PATH);
        strcat_s(buffer, MAX_PATH, "\\xinput9_1_0.dll");

        const auto module = LoadLibraryA(buffer);
        if (module) {
            XInputGetState_ = (XInputGetState_t)GetProcAddress(module, "XInputGetState");
            XInputSetState_ = (XInputSetState_t)GetProcAddress(module, "XInputSetState");
        }

        // if we are running on the wrong version, don't continue
        if (*(uint32_t *)0x142305658 != 0x6c617641) {
#ifdef DEBUG
            MessageBox(nullptr, "Wrong version.", nullptr, MB_ICONERROR | MB_OK);
#endif
            return TRUE;
        }

        D3D11CreateDevice_orig = (uintptr_t)GetProcAddress(GetModuleHandle("d3d11.dll"), "D3D11CreateDevice");
        hk::set_import("D3D11CreateDevice", (uintptr_t)D3D11CreateDevice);
    } else if (fdwReason == DLL_PROCESS_DETACH) {
        Graphics::Get()->Shutdown();

        HWND hwnd    = *(HWND *)0x142E22A18;
        WndProc_orig = (WNDPROC)SetWindowLongPtr(hwnd, GWLP_WNDPROC, (LONG_PTR)WndProc_orig);
    }

    return TRUE;
}
