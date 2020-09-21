#include <cstdio>
#include <windows.h>

int main() {
    // 系统缩放配置信息
    const UCHAR MINE_END = 0x10; // 一行雷结束了的结束符
    const UCHAR MINE = -113; // 0x8F代表雷
    const UCHAR RATIO = 100; // 系统缩放比例
    // 查找窗口
    HWND hWnd = FindWindow(NULL, "扫雷");
//    printf("FindWindow hWnd = %d\n", hWnd);
    if (!hWnd) {
        printf("获取窗口失败\n");
        return -1;
    }
    // 通过窗口句柄获取游戏进程ID
    DWORD processId;
    GetWindowThreadProcessId(hWnd, &processId);
    // 通过进程ID拿到进程句柄
    HANDLE handle = OpenProcess(PROCESS_ALL_ACCESS, false, processId);
    // 读取进程的内存信息
    // 读取雷区宽高信息。0x01005334宽    0x01005338高
    UINT widthHeight[2];
    ReadProcessMemory(handle, (LPCVOID)0x01005334, widthHeight, 8, NULL);
    // 宽度可以不需要，因为一行读取到0x10就算结束了。这样也就是知道宽度了
    UINT height = widthHeight[1];
    if (height <= 0 || height > 24) {
        printf("读取宽高信息出错，读取的高度为%d\n", height);
        return -1;
    }
//    printf("height = %d\n", height);
    // 读取雷区内存信息
    byte mines[height * 32]; // 知道高度后，就不要读取24 * 32这么多了
    ReadProcessMemory(handle, (LPCVOID)0x01005361, mines, height * 32, NULL);
    // 开始解析信息，自动扫雷
    // 计算坐标位置
    const short baseX = 20;
    const short baseY = 60;
    const short step = 16;
    USHORT xyPosition[2];
    for (int i = 0; i < height; i++) {
        // y坐标要在x坐标后边。由于这里使用的是字节的高位对应数字的高位
        xyPosition[1] = (USHORT) ((baseY + step * i) * RATIO / 100);
        for (int j = 0; j < 32; j++) {
            byte b = mines[i * 32 + j];
            if (b == MINE_END) {
                break;
            } else if (b != MINE){
                // 不是雷才点击
                xyPosition[0] = (USHORT) ((baseX + step * j) * RATIO / 100);
                /*xyPosition[0] = (USHORT)20;
                xyPosition[1] = (USHORT)60;
                printf("val = %d\n", *(UINT *)xyPosition);*/
                // 这里用PostMessage和SendMessage都可以.只是PostMessage是非阻塞的，而SendMessage是阻塞的
                PostMessage(hWnd, WM_LBUTTONDOWN, MK_LBUTTON, *(UINT *)xyPosition);
                PostMessage(hWnd, WM_LBUTTONUP, MK_LBUTTON, *(UINT *)xyPosition);
//                printf("发送消息 x = %d : %d, y = %d : %d \n", j, xyPosition[0], i, xyPosition[1]);
            }
        }
    }
    printf("自动扫雷完毕\n");
    return 0;
}