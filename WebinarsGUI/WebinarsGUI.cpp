// WebinarsGUI.cpp : Defines the entry point for the application.
//

#include "WebinarsGUI.h"
#include "Button.h"
#include <string>
#include <fstream>
#include <algorithm>
#include <iostream>
#include <thread>

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name
HWND hWnd;

int width = 200, height = 300;

std::vector<Button*> classes;

std::vector<std::wstring> split(const std::wstring& str, const std::wstring& delim)
{
    std::vector<std::wstring> tokens;
    size_t prev = 0, pos = 0;
    do
    {
        pos = str.find(delim, prev);
        if (pos == std::wstring::npos) pos = str.length();
        std::wstring token = str.substr(prev, pos - prev);
        if (!token.empty()) tokens.push_back(token);
        prev = pos + delim.length();
    } while (pos < str.length() && prev < str.length());
    return tokens;
}

int runCmd(std::wstring b) {
    return _wsystem(b.c_str());
}

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    Add(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    Remove(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Place code here.

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_WEBINARSGUI, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_WEBINARSGUI));

    MSG msg;

    // Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}

//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_WEBINARSGUI));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_WEBINARSGUI);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_WEBINARSGUI));

    return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Store instance handle in our global variable

   hWnd = CreateWindowW(szWindowClass, szTitle, (WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_BORDER),
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   int h, v;
   GetDesktopResolution(h, v);
   MoveWindow(hWnd, (h-width)/2, (v-height)/2, width + 15, height, true);

   if (!hWnd)
   {
      return FALSE;
   }

   std::wstring name, code, pass;
   std::wifstream fin("classes.txt");
   if (fin.is_open()) {
       do {
           std::wstring line;
           getline(fin, line);

           std::vector<std::wstring> res = split(line, L"|");
           if (res.size() >= 2) {
               name = res[0];
               code = res[1];
               if (res.size() == 3) pass = res[2];

               classes.push_back(new Button);
               classes[classes.size() - 1]->cmd = LR"(%APPDATA%\Zoom\bin\Zoom.exe "--url=zoommtg://zoom.us/join?action=join&confno=)" + code;
               if (res.size() == 3) classes[classes.size() - 1]->cmd += LR"(&pwd=)" + pass;
               classes[classes.size() - 1]->name = name;
               classes[classes.size() - 1]->wnd = CreateWindow(
                   L"BUTTON",  // Predefined class; Unicode assumed 
                   (LPCWSTR)name.c_str(),      // Button text 
                   WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,  // Styles 
                   (width - 100) / 2,         // x position 
                   75 + 35 * (classes.size() - 1),         // y position 
                   100,        // Button width
                   25,        // Button height
                   hWnd,     // Parent window
                   NULL,       // No menu.
                   (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE),
                   NULL);      // Pointer not needed.
           
               if (175 + 35 * (classes.size() - 1) > height) {
                   height += 35;
                   MoveWindow(hWnd, (h - width) / 2, (v - 300) / 2, width + 15, height, true);
               }
           }
       } while (!fin.eof());
   }
    fin.close();

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

   for (Button* b : classes) {
       ShowWindow(b->wnd, nCmdShow);
       UpdateWindow(b->wnd);
   }

   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Parse the menu selections:
            switch (wmId)
            {
            case BN_CLICKED:
                for (Button* b : classes) {
                    if ((HWND)lParam == b->wnd) {
                        std::thread thread(runCmd, b->cmd);
                        thread.join();
                        PostQuitMessage(0);
                    }
                }

                break;
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case ID_FILE_ADDCLASS:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_DIALOG1), hWnd, Add);
                break;
            case ID_FILE_REMOVECLASS:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_DIALOG2), hWnd, Remove);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            RECT rect;
            HBRUSH hBrush;
            HFONT hFont;

            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            
            //Logical units are device dependent pixels, so this will create a handle to a logical font that is 48 pixels in height.
            //The width, when set to 0, will cause the font mapper to choose the closest matching value.
            //The font face name will be Impact.
            hFont = CreateFont(36, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_OUTLINE_PRECIS,
                CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, VARIABLE_PITCH, TEXT("Impact"));
            SelectObject(hdc, hFont);

            //Sets the coordinates for the rectangle in which the text is to be formatted.
            SetRect(&rect, 0, 15, 200, 290);
            SetTextColor(hdc, RGB(0, 0, 0));
            DrawText(hdc, TEXT("Webinar Links"), -1, &rect, DT_CENTER);

            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}

// Message handler for about box.
INT_PTR CALLBACK Add(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    TCHAR lpszCode[MAX_LOADSTRING];
    WORD cchCode;

    TCHAR lpszPass[MAX_LOADSTRING];
    WORD cchPass;

    TCHAR lpszName[MAX_LOADSTRING];
    WORD cchName;

    HFONT hFont;
    HWND hctl;
    HDC hdc;

    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;
    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK)
        {
            cchCode = (WORD)SendDlgItemMessage(hDlg,
                IDC_EDIT1,
                EM_LINELENGTH,
                (WPARAM)0,
                (LPARAM)0);
            
            if (cchCode == 0)
            {
                MessageBox(hDlg,
                    L"No room code entered.",
                    L"Error",
                    MB_OK);

                EndDialog(hDlg, TRUE);
                return FALSE;
            }

            // Put the number of characters into first word of buffer. 
            *((LPWORD)lpszCode) = cchCode;

            // Get the characters. 
            SendDlgItemMessage(hDlg,
                IDC_EDIT1,
                EM_GETLINE,
                (WPARAM)0,       // line 0 
                (LPARAM)lpszCode);

            // Null-terminate the string. 
            lpszCode[cchCode] = 0;

            cchPass = (WORD)SendDlgItemMessage(hDlg,
                IDC_EDIT2,
                EM_LINELENGTH,
                (WPARAM)0,
                (LPARAM)0);

            if (cchPass == 0)
            {
                if (MessageBox(hDlg,
                    L"Are you sure this room has no password?",
                    L"No characters entered.",
                    MB_YESNO) == IDNO) {

                    return FALSE;
                }
            }

            // Put the number of characters into first word of buffer. 
            *((LPWORD)lpszPass) = cchPass;

            // Get the characters. 
            SendDlgItemMessage(hDlg,
                IDC_EDIT2,
                EM_GETLINE,
                (WPARAM)0,       // line 0 
                (LPARAM)lpszPass);

            // Null-terminate the string. 
            lpszPass[cchPass] = 0;

            cchName = (WORD)SendDlgItemMessage(hDlg,
                IDC_EDIT3,
                EM_LINELENGTH,
                (WPARAM)0,
                (LPARAM)0);

            if (cchName == 0)
            {
                MessageBox(hDlg,
                    L"No name entered.",
                    L"Error",
                    MB_OK);

                EndDialog(hDlg, TRUE);
                return FALSE;
            }

            // Put the number of characters into first word of buffer. 
            *((LPWORD)lpszName) = cchName;

            // Get the characters. 
            SendDlgItemMessage(hDlg,
                IDC_EDIT3,
                EM_GETLINE,
                (WPARAM)0,       // line 0 
                (LPARAM)lpszName);

            // Null-terminate the string. 
            lpszName[cchName] = 0;

            classes.push_back(new Button);
            classes[classes.size() - 1]->cmd = LR"(%APPDATA%\Zoom\bin\Zoom.exe "--url=zoommtg://zoom.us/join?action=join&confno=)" + (std::wstring)lpszCode + LR"(&pwd=)" + (std::wstring)lpszPass;
            classes[classes.size() - 1]->name = lpszName;
            classes[classes.size() - 1]->wnd = CreateWindow(
                L"BUTTON",  // Predefined class; Unicode assumed 
                lpszName,      // Button text 
                WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,  // Styles 
                (width - 100) / 2,         // x position 
                75 + 35*(classes.size() -1),         // y position 
                100,        // Button width
                25,        // Button height
                hWnd,     // Parent window
                NULL,       // No menu.
                (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE),
                NULL);      // Pointer not needed.

            if (175 + 35 * (classes.size() - 1) > height) {
                height += 35;
                int h, v;
                GetDesktopResolution(h, v);
                MoveWindow(hWnd, (h - width) / 2, (v - 300) / 2, width + 15, height, true);
            }

            std::wofstream fout("classes.txt", std::ios::app);
            fout << std::wstring(lpszName) << "|" << std::wstring(lpszCode) << "|" << std::wstring(lpszPass) << std::endl;
            fout.close();

            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        else if (LOWORD(wParam) == IDCANCEL) {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}

// Message handler for about box.
INT_PTR CALLBACK Remove(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    HWND hwndList = GetDlgItem(hDlg, IDC_COMBO2);

    switch (message)
    {
    case WM_INITDIALOG:
        // Add items to list. 
        for (int i = 0; i < classes.size(); i++)
        {
            int pos = (int)SendMessage(hwndList, CB_ADDSTRING, 0,
                (LPARAM)classes[i]->name.c_str());
            // Set the array index of the player as item data.
            // This enables us to retrieve the item from the array
            // even after the items are sorted by the list box.
            SendMessage(hwndList, CB_SETITEMDATA, pos, (LPARAM)i);
        }
        // Set input focus to the list box.
        SetFocus(hwndList);
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK)
        {
            int ItemIndex = SendMessage(hwndList, (UINT)CB_GETCURSEL,
                    (WPARAM)0, (LPARAM)0);
            TCHAR  ListItem[256];
            (TCHAR)SendMessage(hwndList, (UINT)CB_GETLBTEXT,
                (WPARAM)ItemIndex, (LPARAM)ListItem);
            
            for (int i = 0; i < classes.size(); i++) {
                if (classes[i]->name == ListItem) {
                    std::wstring line;
                    std::wifstream fin;

                    fin.open("classes.txt");
                    // contents of path must be copied to a temp file then
                    // renamed back to the path file
                    std::wofstream temp;
                    temp.open("temp.txt");

                    while (getline(fin, line)) {
                        // write all lines to temp other than the line marked for erasing
                        if (line.find(classes[i]->name) == std::wstring::npos)
                            temp << line << std::endl;
                    }

                    temp.close();
                    fin.close();

                    remove("classes.txt");
                    rename("temp.txt", "classes.txt");

                    DestroyWindow(classes[i]->wnd);
                    classes.erase(classes.begin() + i);

                    if (height - 35 > 300) {
                        height -= 35;
                        int h, v;
                        GetDesktopResolution(h, v);
                        MoveWindow(hWnd, (h - width) / 2, (v - 300) / 2, width + 15, height, true);
                    }

                    for (int j = i; j < classes.size(); j++) MoveWindow(classes[j]->wnd, (width - 100) / 2, 75 + 35 * j, 100, 25, true);
                }
            }

            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        } else if (LOWORD(wParam) == IDCANCEL) {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}