#include "gui_interface.h"
#include <tuple>
#include <fonts/fonts.h>
#include "../../MLP/Settings/settings.h"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(
    HWND hWnd,
    UINT msg,
    WPARAM wParam,
    LPARAM lParam
);

LRESULT CALLBACK window_procedure(HWND window, UINT message, WPARAM w_param, LPARAM l_param) {
    if (ImGui_ImplWin32_WndProcHandler(window, message, w_param, l_param)) {
        return 0L;
    }

    switch (message) {
    case WM_CLOSE:
        DestroyWindow(window);
        return 0L;
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0L;
    default:
        return DefWindowProc(window, message, w_param, l_param);
    }
}

template <typename T>
inline VOID c_overlay::safe_release(T*& p) {
    if (NULL != p) {
        p->Release();
        p = NULL;
    }
}

HWND c_overlay::get_window_handle() {
    return window_handle;
}

BOOL c_overlay::init_device() {
    DXGI_SWAP_CHAIN_DESC vSwapChainDesc;
    ZeroMemory(&vSwapChainDesc, sizeof(vSwapChainDesc));
    vSwapChainDesc.BufferCount = 2;
    vSwapChainDesc.BufferDesc.Width = 0;
    vSwapChainDesc.BufferDesc.Height = 0;
    vSwapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    vSwapChainDesc.BufferDesc.RefreshRate.Numerator = 60U;
    vSwapChainDesc.BufferDesc.RefreshRate.Denominator = 1U;
    vSwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    vSwapChainDesc.OutputWindow = window_handle;
    vSwapChainDesc.SampleDesc.Count = 2U;
    vSwapChainDesc.SampleDesc.Quality = 0;
    vSwapChainDesc.Windowed = TRUE;
    vSwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
    vSwapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

    D3D_FEATURE_LEVEL vFeatureLevel;
    D3D_FEATURE_LEVEL vFeatureLevelArray[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0, };

    if (D3D11CreateDeviceAndSwapChain(
        NULL,
        D3D_DRIVER_TYPE_HARDWARE,
        NULL,
        0,
        vFeatureLevelArray,
        2,
        D3D11_SDK_VERSION,
        &vSwapChainDesc,
        &swap_chain,
        &d3d_device,
        &vFeatureLevel,
        &device_context
    ) != S_OK) return false;

    init_render_target();

    ShowWindow(window_handle, SW_SHOWNORMAL);
    UpdateWindow(window_handle);
    return true;
}

VOID c_overlay::dest_device() {
    dest_render_target();
    safe_release(swap_chain);
    safe_release(device_context);
    safe_release(d3d_device);
}

VOID c_overlay::init_imgui() {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();

    io.IniFilename = nullptr;

    auto& style = ImGui::GetStyle();
    style.FrameRounding = 2.5f;
    style.FramePadding = ImVec2(10, 2.5);
    style.WindowRounding = 10;
    style.WindowPadding = ImVec2(0, 0);
    style.ScrollbarRounding = 2.5f;
    style.ScrollbarSize = 2.5f;
    style.ChildRounding = 2.5f;
    style.PopupRounding = 2.5f;
    style.ItemSpacing = ImVec2(10, 5);

    auto colors = style.Colors;
    colors[ImGuiCol_WindowBg] = ImColor(0, 0, 0, 0);
    colors[ImGuiCol_FrameBg] = ImColor(25, 25, 25, 255);
    colors[ImGuiCol_FrameBgHovered] = ImColor(25, 25, 25, 255);
    colors[ImGuiCol_FrameBgActive] = ImColor(77, 125, 255);
    colors[ImGuiCol_SliderGrab] = ImColor(77, 125, 253, 200);
    colors[ImGuiCol_SliderGrabActive] = ImColor(250, 180, 20, 150);
    colors[ImGuiCol_PopupBg] = ImColor(25, 25, 25, 255);
    colors[ImGuiCol_ScrollbarBg] = ImColor(4, 21, 39);
    colors[ImGuiCol_ScrollbarGrab] = Settings::Menu::color;
    colors[ImGuiCol_ScrollbarGrabActive] = Settings::Menu::color;
    colors[ImGuiCol_ScrollbarGrabHovered] = Settings::Menu::color;
    colors[ImGuiCol_Separator] = Settings::Menu::color;
    colors[ImGuiCol_Header] = ImColor(25, 25, 25, 225);
    colors[ImGuiCol_HeaderHovered] = Settings::Menu::color;
    colors[ImGuiCol_HeaderActive] = Settings::Menu::color;
    colors[ImGuiCol_Button] = ImColor(25, 25, 25, 225);
    colors[ImGuiCol_ButtonHovered] = Settings::Menu::color;
    colors[ImGuiCol_ButtonActive] = Settings::Menu::color;
    colors[ImGuiCol_Text] = ImColor(255, 255, 255, 235);
    colors[ImGuiCol_TextDisabled] = ImColor(120, 120, 120, 255);
    colors[ImGuiCol_Border] = ImColor(0, 0, 0);
    colors[ImGuiCol_CheckMark] = ImColor(16, 16, 18, 255);

    Fonts::InitFonts();

    ImGui::GetIO().WantSaveIniSettings = false;

    ImGui_ImplWin32_Init(window_handle);
    ImGui_ImplDX11_Init(d3d_device, device_context);
}

VOID c_overlay::dest_imgui() {
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
}

VOID c_overlay::init_render_target() {
    ID3D11Texture2D* back_buffer = nullptr;
    swap_chain->GetBuffer(0, IID_PPV_ARGS(&back_buffer));

    if (back_buffer) {
        d3d_device->CreateRenderTargetView(back_buffer, NULL, &render_target_view);
    }
    back_buffer->Release();
}

VOID c_overlay::dest_render_target() {
    if (!render_target_view)
        return;

    render_target_view->Release();
    render_target_view = NULL;
}

BOOL c_overlay::init_window() {
    WNDCLASSEXW wc{};
    wc.cbSize = sizeof(WNDCLASSEXW);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = window_procedure;
    wc.hInstance = instance;
    wc.lpszClassName = L"External overlay Class";

    RECT desktop;
    const HWND hDesktop = GetDesktopWindow();
    GetWindowRect(hDesktop, &desktop);
    int screenWidth = desktop.right;
    int screenHeight = desktop.bottom;

    SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);

    if (!RegisterClassExW(&wc))
        return false;

    window_handle = CreateWindowExW(
        WS_EX_TOPMOST | WS_EX_LAYERED | WS_EX_TOOLWINDOW | WS_EX_TRANSPARENT,
        wc.lpszClassName,
        L"External overlay Class",
        WS_POPUP,
        0,
        0,
        screenWidth,
        screenHeight,
        nullptr,
        nullptr,
        wc.hInstance,
        nullptr
    );

    if (!window_handle)
        return false;
}

VOID c_overlay::dest_window() {
    DestroyWindow(window_handle);
    UnregisterClassW(L"External overlay Class", instance);
}

c_overlay::c_overlay() :
    exit_ready(false),
    window_handle(nullptr),
    window_class({}),
    window_width((FLOAT)GetSystemMetrics(SM_CXSCREEN)),
    window_height((FLOAT)GetSystemMetrics(SM_CYSCREEN)),
    font(nullptr),
    d3d_device(nullptr),
    device_context(nullptr),
    swap_chain(nullptr),
    swap_chain_occluded(false),
    render_target_view(nullptr),
    draw_list(nullptr),
    breath(NULL),
    menu_ticks(NULL) {

    init_window();

    if (window_handle) {

        auto info = GetWindowLongA(window_handle, -20);
        if (!info)
            return;

        auto attrchange = SetWindowLongPtrA(window_handle, -20, (LONG_PTR)(info | 0x20));
        if (!attrchange)
            return;

        RECT client_area{};
        GetClientRect(window_handle, &client_area);

        RECT window_area{};
        GetWindowRect(window_handle, &window_area);

        POINT diff{};
        ClientToScreen(window_handle, &diff);

        const MARGINS margins{
            window_area.left + (diff.x - window_area.left),
            window_area.top + (diff.y - window_area.top),
            client_area.right,
            client_area.bottom
        };

        if (DwmExtendFrameIntoClientArea(window_handle, &margins) != S_OK)
            return;

        if (!SetLayeredWindowAttributes(window_handle, RGB(0, 0, 0), BYTE(255), LWA_ALPHA))
            return;

        auto [x, y] = std::tuple<float, float>((float)GetSystemMetrics(SM_CXSCREEN), (float)GetSystemMetrics(SM_CYSCREEN));

        if (!SetWindowPos(window_handle, HWND_TOPMOST, 0, 0, static_cast<int>(x), static_cast<int>(y), 0))
            return;
    }
    else
        window_handle = FindWindowA(("0"), ("0"));

    font = new s_font();

    if (!init_device()) {
        return;
    }

    init_imgui();

    std::thread(&c_overlay::input_handler, this).detach();
}

c_overlay::~c_overlay() {
    dest_imgui();
    dest_window();
    dest_device();

    delete font;
}

void c_overlay::bind_render_callback(std::function<void()> callback) {
    this->render_callback = (callback);
}


const void c_overlay::render(FLOAT width, FLOAT height) {
    SetWindowPos(window_handle, 0, 0, 0, static_cast<int>(window_width), static_cast<int>(window_height), 0);

    ImGuiStyle& style = ImGui::GetStyle();

    init_draw_list();

    if (menu_ticks == 1)
        return;

    this->render_callback();
}

void c_overlay::end_frame() {
    ImGui::Render();
    device_context->OMSetRenderTargets(1, &render_target_view, NULL);
    device_context->ClearRenderTargetView(render_target_view, (float*)&clear_clr);
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

    if (swap_chain_occluded && swap_chain->Present(0, DXGI_PRESENT_TEST) == DXGI_STATUS_OCCLUDED) {
        Sleep(10);
        return;
    }

    swap_chain_occluded = false;
    HRESULT hr = swap_chain->Present(1, 0);
    swap_chain_occluded = (hr == DXGI_STATUS_OCCLUDED);
}


MSG c_overlay::begin_frame() {
    MSG msg{ 0 };

    if (::PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE)) {
        ::TranslateMessage(&msg);
        ::DispatchMessage(&msg);
    }

    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    return msg;
}

BOOL c_overlay::msg_loop() {
    if (exit_ready)
        return false;

    if (window_handle && !clear_screen) {

        auto msg = begin_frame();

        render(window_width, window_height);

        end_frame();

        return msg.message != WM_QUIT;
    }
    else if (clear_screen) {
        auto msg = begin_frame();
        end_frame();

        std::this_thread::sleep_for(std::chrono::seconds(1));

        clear_screen = false;

        return msg.message != WM_QUIT;
    }
    return false;
}

void c_overlay::input_handler() {
    for (; !exit_ready; Sleep(1)) {
        ImGuiIO& io = ImGui::GetIO();

        POINT p{};
        GetCursorPos(&p);
        io.MousePos = ImVec2((float)p.x, (float)p.y);

        io.MouseDown[0] = GetAsyncKeyState(VK_LBUTTON) & 0x8000;
        io.MouseDown[1] = GetAsyncKeyState(VK_RBUTTON) & 0x8000;

        bool is_arrow_key_up_down = GetAsyncKeyState(VK_UP) != 0;
        bool is_arrow_key_down_down = GetAsyncKeyState(VK_DOWN) != 0;

        io.MouseWheel = is_arrow_key_up_down ? .5f : is_arrow_key_down_down ? -.5f : 0.0f;
    }
}

VOID c_overlay::init_draw_list() {
    draw_list = ImGui::GetBackgroundDrawList();
}

BOOL c_overlay::in_screen(const ImVec2& pos) {
    return !(pos.x > window_width || pos.x<0 || pos.y>window_height || pos.y < 0);
}

void c_overlay::exit() {
    begin_frame();
    end_frame();
    exit_ready = true;
}