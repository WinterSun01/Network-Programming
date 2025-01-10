#include <Windows.h>
#include <CommCtrl.h>
#include "resource.h"
#include <strsafe.h>

BOOL CALLBACK DlgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void UpdateMaskAndPrefix(HWND hwnd, LPWSTR ipAddress);  //для вычисления маски подсети и префикса

INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInst, LPSTR lpCmdLine, INT nCmdShow)
{
    DialogBoxParam(hInstance, MAKEINTRESOURCE(IDD_MAIN_DIALOG), NULL, (DLGPROC)DlgProc, 0);
    return 0;
}

BOOL CALLBACK DlgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_INITDIALOG:
    {
        HWND hUpDown = GetDlgItem(hwnd, IDC_SPIN_PREFIX);
        SendMessage(hUpDown, UDM_SETRANGE, 0, MAKELPARAM(30, 1));
    }
    break;

    case WM_COMMAND:
    {
        //для проверки, была ли изменена строка с IP-адресом
        if (LOWORD(wParam) == IDC_IPADDRESS)
        {
            WCHAR ipBuffer[16];  //буфер для хранения IP-адреса
            GetDlgItemText(hwnd, IDC_IPADDRESS, ipBuffer, ARRAYSIZE(ipBuffer));

            UpdateMaskAndPrefix(hwnd, ipBuffer);
        }
    }
    break;

    case WM_CLOSE:
        EndDialog(hwnd, 0);
        break;
    }

    return FALSE;
}

//для вычисления маски подсети и префикса на основе IP-адреса
void UpdateMaskAndPrefix(HWND hwnd, LPWSTR ipAddress)
{
    unsigned char octets[4];
    int prefix = 0;

    int scanned = swscanf_s(ipAddress, L"%hhu.%hhu.%hhu.%hhu", &octets[0], &octets[1], &octets[2], &octets[3]);

    if (scanned == 4)
    {
        if (octets[0] >= 1 && octets[0] <= 127)
        {
            prefix = 8;
        }
        else if (octets[0] >= 128 && octets[0] <= 191)
        {
            prefix = 16;
        }
        else if (octets[0] >= 192 && octets[0] <= 223)
        {
            prefix = 24;
        }

        //для форм. строки маски подсети на основе префикса
        WCHAR mask[16];
        StringCchPrintfW    (mask, ARRAYSIZE(mask), L"%d.%d.%d.%d",
                            (prefix >= 8 ? 255 : 0),
                            (prefix >= 16 ? 255 : 0),
                            (prefix >= 24 ? 255 : 0),
                            0);

        //уст. знач. маски подсети в соответствующее текст. поле
        SetDlgItemText(hwnd, IDC_IPMASK, mask);

        //уст. значение префикса в соотв. текст. поле
        SetDlgItemInt(hwnd, IDC_EDIT_PREFIX, prefix, FALSE);
    }
}
