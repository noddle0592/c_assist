#include <cstdio>
#include <windows.h>

int main() {
    // ϵͳ����������Ϣ
    const UCHAR MINE_END = 0x10; // һ���׽����˵Ľ�����
    const UCHAR MINE = -113; // 0x8F������
    const UCHAR RATIO = 100; // ϵͳ���ű���
    // ���Ҵ���
    HWND hWnd = FindWindow(NULL, "ɨ��");
//    printf("FindWindow hWnd = %d\n", hWnd);
    if (!hWnd) {
        printf("��ȡ����ʧ��\n");
        return -1;
    }
    // ͨ�����ھ����ȡ��Ϸ����ID
    DWORD processId;
    GetWindowThreadProcessId(hWnd, &processId);
    // ͨ������ID�õ����̾��
    HANDLE handle = OpenProcess(PROCESS_ALL_ACCESS, false, processId);
    // ��ȡ���̵��ڴ���Ϣ
    // ��ȡ���������Ϣ��0x01005334��    0x01005338��
    UINT widthHeight[2];
    ReadProcessMemory(handle, (LPCVOID)0x01005334, widthHeight, 8, NULL);
    // ��ȿ��Բ���Ҫ����Ϊһ�ж�ȡ��0x10��������ˡ�����Ҳ����֪�������
    UINT height = widthHeight[1];
    if (height <= 0 || height > 24) {
        printf("��ȡ�����Ϣ������ȡ�ĸ߶�Ϊ%d\n", height);
        return -1;
    }
//    printf("height = %d\n", height);
    // ��ȡ�����ڴ���Ϣ
    byte mines[height * 32]; // ֪���߶Ⱥ󣬾Ͳ�Ҫ��ȡ24 * 32��ô����
    ReadProcessMemory(handle, (LPCVOID)0x01005361, mines, height * 32, NULL);
    // ��ʼ������Ϣ���Զ�ɨ��
    // ��������λ��
    const short baseX = 20;
    const short baseY = 60;
    const short step = 16;
    USHORT xyPosition[2];
    for (int i = 0; i < height; i++) {
        // y����Ҫ��x�����ߡ���������ʹ�õ����ֽڵĸ�λ��Ӧ���ֵĸ�λ
        xyPosition[1] = (USHORT) ((baseY + step * i) * RATIO / 100);
        for (int j = 0; j < 32; j++) {
            byte b = mines[i * 32 + j];
            if (b == MINE_END) {
                break;
            } else if (b != MINE){
                // �����ײŵ��
                xyPosition[0] = (USHORT) ((baseX + step * j) * RATIO / 100);
                /*xyPosition[0] = (USHORT)20;
                xyPosition[1] = (USHORT)60;
                printf("val = %d\n", *(UINT *)xyPosition);*/
                // ������PostMessage��SendMessage������.ֻ��PostMessage�Ƿ������ģ���SendMessage��������
                PostMessage(hWnd, WM_LBUTTONDOWN, MK_LBUTTON, *(UINT *)xyPosition);
                PostMessage(hWnd, WM_LBUTTONUP, MK_LBUTTON, *(UINT *)xyPosition);
//                printf("������Ϣ x = %d : %d, y = %d : %d \n", j, xyPosition[0], i, xyPosition[1]);
            }
        }
    }
    printf("�Զ�ɨ�����\n");
    return 0;
}