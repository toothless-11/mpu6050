#include <windows.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <iostream>

// --- 全局变量：控制旋转的角度 ---
float rotX = 20.0f;
float rotY = 30.0f;


void DrawGrid(float size, float step) {
    glLineWidth(1.0f);
    glColor3f(0.3f, 0.3f, 0.3f); // 深灰色，不抢坐标轴的风头
    glBegin(GL_LINES);
    for (float i = -size; i <= size; i += step) {
        // 画横线 (平行于 X 轴)
        glVertex3f(-size, 0, i);
        glVertex3f(size, 0, i);
        // 画纵线 (平行于 Z 轴)
        glVertex3f(i, 0, -size);
        glVertex3f(i, 0, size);
    }
    glEnd();
}


void DrawScene(HWND hwnd) {
    RECT rect;
    GetClientRect(hwnd, &rect);
    int w = rect.right, h = rect.bottom;

    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, (double)w / (double)h, 0.1, 100.0);

    glMatrixMode(GL_MODELVIEW);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    // 相机稍微抬高一点，俯视网格
    gluLookAt(8.0, 8.0, 12.0,  0.0, 0.0, 0.0,  0.0, 1.0, 0.0);

    // 应用旋转（键盘控制的角度）
    glRotatef(rotX, 1.0f, 0.0f, 0.0f);
    glRotatef(rotY, 0.0f, 1.0f, 0.0f);

    // 1. 先画网格（地面）
    DrawGrid(10.0f, 1.0f); 

    // 2. 再画坐标轴
    glLineWidth(4.0f);
    glBegin(GL_LINES);
        glColor3f(1, 0, 0); glVertex3f(0, 0, 0); glVertex3f(5, 0, 0); // X
        glColor3f(0, 1, 0); glVertex3f(0, 0, 0); glVertex3f(0, 5, 0); // Y
        glColor3f(0, 0, 1); glVertex3f(0, 0, 0); glVertex3f(0, 0, 5); // Z
    glEnd();
}
// --- 窗口消息处理：监听按键 ---
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_CLOSE: 
            PostQuitMessage(0); 
            return 0;
        case WM_KEYDOWN: // 按键按下事件
            switch (wParam) {
                case VK_UP:    rotX -= 5.0f; break; // 上键：绕X轴负向转
                case VK_DOWN:  rotX += 5.0f; break; // 下键：绕X轴正向转
                case VK_LEFT:  rotY -= 5.0f; break; // 左键：绕Y轴负向转
                case VK_RIGHT: rotY += 5.0f; break; // 右键：绕Y轴正向转
                case VK_ESCAPE: PostQuitMessage(0); break;
            }
            return 0;
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

int main() {
    WNDCLASS wc = {0};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = GetModuleHandle(NULL);
    wc.lpszClassName = "OpenGLWin";
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    RegisterClass(&wc);

    HWND hwnd = CreateWindow("OpenGLWin", "Keyboard Control 3D", WS_OVERLAPPEDWINDOW | WS_VISIBLE, 100, 100, 800, 600, NULL, NULL, wc.hInstance, NULL);
    
    HDC hdc = GetDC(hwnd);
    PIXELFORMATDESCRIPTOR pfd = { sizeof(pfd), 1, PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER, PFD_TYPE_RGBA, 32 };
    SetPixelFormat(hdc, ChoosePixelFormat(hdc, &pfd), &pfd);
    wglMakeCurrent(hdc, wglCreateContext(hdc));
    glEnable(GL_DEPTH_TEST);

    std::cout << "Control:↑↓←→" << std::endl;

    MSG msg;
    // ... 前面的代码不变 ...
    while (true) {
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT) break;
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        // --- 核心改动：直接检测按键状态，0延迟 ---
        // GetAsyncKeyState 如果最高位是 1，说明按键正被按下
        if (GetAsyncKeyState(VK_UP) & 0x8000)    rotX -= 2.0f; 
        if (GetAsyncKeyState(VK_DOWN) & 0x8000)  rotX += 2.0f;
        if (GetAsyncKeyState(VK_LEFT) & 0x8000)  rotY -= 2.0f;
        if (GetAsyncKeyState(VK_RIGHT) & 0x8000) rotY += 2.0f;

        DrawScene(hwnd);
        SwapBuffers(hdc);
        
        // 这里的 Sleep 决定了旋转的速度感
        // Sleep(16) 约等于 60帧，手感会非常丝滑
        Sleep(16); 
    }
    return 0;
}