#include <windows.h>
#include <iostream>
#include <string>
#include <algorithm>
//读取三轴加速度，并计算偏差平均值

int main() {
    // 1. 初始化配置
    std::string portName = "\\\\.\\COM3";
    HANDLE hSerial = CreateFileA(portName.c_str(), GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

    if (hSerial == INVALID_HANDLE_VALUE) {
        std::cerr << "Open error!" << std::endl;
        return 1;
    }

    DCB dcb = { 0 };
    dcb.DCBlength = sizeof(dcb);
    GetCommState(hSerial, &dcb);
    dcb.BaudRate = CBR_115200;
    dcb.ByteSize = 8;
    dcb.StopBits = ONESTOPBIT;
    dcb.Parity = NOPARITY;
    SetCommState(hSerial, &dcb);

    COMMTIMEOUTS timeouts = { 0 };
    timeouts.ReadIntervalTimeout = 20; // 50Hz 间隔约 20ms
    SetCommTimeouts(hSerial, &timeouts);

    // 2. 数据处理变量
    int AXS = 0, AYS = 0, AZS = 0, GXS = 0, GYS = 0, GZS = 0;
    int count = 0;
    const int LIMIT = 800;
    std::string buffer = ""; // 字节缓冲区

    std::cout << "Listening at 50Hz... Target: " << LIMIT << " frames" << std::endl;

    // 3. 主循环
    while (count < LIMIT) {
        char tmp[128];
        DWORD bytesRead = 0;

        if (ReadFile(hSerial, tmp, sizeof(tmp), &bytesRead, NULL) && bytesRead > 0) {
            buffer.append(tmp, bytesRead); // 将新读到的内容追加到缓冲区

            size_t bPos, aPos;
            // 循环检查缓冲区中是否包含完整的帧 B...A
            while ((bPos = buffer.find('B')) != std::string::npos && 
                   (aPos = buffer.find('A', bPos)) != std::string::npos) 
            {
                // 提取帧内容 (B之后到A之前的部分)
                std::string frame = buffer.substr(bPos + 1, aPos - bPos - 1);
                
                // 逆序处理（匹配你原有的逻辑）
                std::reverse(frame.begin(), frame.end());

                // 解析数值：利用 sscanf 直接从字符串中提取 6 个带符号整数
                int v[6];
                if (sscanf_s(frame.c_str(), "%d%d%d%d%d%d", &v[0], &v[1], &v[2], &v[3], &v[4], &v[5]) == 6) {
                    AXS += v[5]; AYS += v[4]; AZS += v[3];
                    GXS += v[2]; GYS += v[1]; GZS += v[0];
                    
                    count++;
                    if (count % 10 == 0) std::cout << "Progress: " << count << "/" << LIMIT << "\r";
                }

                // 丢弃已处理的数据段
                buffer.erase(0, aPos + 1);
            }

            // 防止异常数据导致缓冲区无限增长
            if (buffer.length() > 512) buffer.clear();
        }
    }

    // 4. 输出结果
    std::cout << "\n\nFinal Mean Data:" << std::endl;
    std::cout << "A: " << AXS / LIMIT << ", " << AYS / LIMIT << ", " << AZS / LIMIT << std::endl;
    std::cout << "G: " << GXS / LIMIT << ", " << GYS / LIMIT << ", " << GZS / LIMIT << std::endl;

    CloseHandle(hSerial);
    return 0;
}