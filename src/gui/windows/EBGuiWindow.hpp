/*
 * EBCpp
 *
 * Copyright (C) 2020 Carsten Grings
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 *
 *  Created on: 2021-01-23
 *      Author: Carsten (Tropby)
 */

#pragma once

#ifdef __WIN32__

#include <gdiplus.h>

#include <iostream>
#include <memory>
#include <thread>
#include <windows.h>

#include "../../EBEvent.hpp"

#ifndef WINMAIN
#define WINMAIN
HINSTANCE hInstance;
int main();
int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow)
{
    hInstance = hInstance;

    return main();
}
#endif

namespace EBCpp
{

class EBGuiWindow : public EBGuiWindowBase
{
public:
    EBGuiWindow(EBObject* parent) : EBGuiWindowBase(parent), windowThread(std::thread(&EBGuiWindow::createWindow, this))
    {
    }

    ~EBGuiWindow()
    {
        windowThread.join();
    }

    virtual void invalidate()
    {
        InvalidateRect(hwnd, NULL, FALSE);
    }

    EB_SIGNAL(closed);

private:
    HWND hwnd;
    std::thread windowThread;

    static LRESULT CALLBACK WindowProcBase(_In_ HWND hwnd, _In_ UINT uMsg, _In_ WPARAM wParam, _In_ LPARAM lParam)
    {
        EBGuiWindow* wnd = (EBGuiWindow*)GetWindowLong(hwnd, GWL_USERDATA);
        switch (uMsg)
        {
        case WM_CREATE:
            wnd = (EBGuiWindow*)((CREATESTRUCT*)lParam)->lpCreateParams;
            SetWindowLong(hwnd, GWL_USERDATA, long(wnd));
        }
        return wnd->WindowProc(hwnd, uMsg, wParam, lParam);
    }

    LRESULT CALLBACK WindowProc(_In_ HWND hwnd, _In_ UINT uMsg, _In_ WPARAM wParam, _In_ LPARAM lParam)
    {
        switch (uMsg)
        {
        case WM_ERASEBKGND:
            return 1;

        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);

            x = 0;
            y = 0;

            RECT rect;
            if (GetWindowRect(WindowFromDC(hdc), &rect))
            {
                w = rect.right - rect.left;
                h = rect.bottom - rect.top;

                int width = rect.right - rect.left;
                int height = rect.bottom - rect.top;

                HDC hdcMem = CreateCompatibleDC(hdc);
                HBITMAP MemBitmap = CreateCompatibleBitmap(hdc, width, height); // Breite und Höhe musst du anpassen
                HBITMAP hbmOld = (HBITMAP)SelectObject(hdcMem, MemBitmap);

                Gdiplus::Graphics graphics(hdcMem);

                for (EBGuiWidget* w : widgets)
                {
                    w->prepare(0, 0, width, height);
                }
                Gdiplus::SolidBrush brush(Gdiplus::Color(255, 255, 255, 255));
                graphics.FillRectangle(&brush, 0, 0, width, height);

                std::list<EBGuiRenderer*> list;
                for (EBGuiWidget* w : widgets)
                {
                    w->render(list);
                    for (EBGuiRenderer* l : list)
                    {
                        l->render(graphics);
                        delete l;
                    }
                }

                BitBlt(hdc, ps.rcPaint.left, ps.rcPaint.top, ps.rcPaint.right, ps.rcPaint.bottom, hdcMem, ps.rcPaint.left,
                    ps.rcPaint.top, SRCCOPY);

                // Free Memory
                SelectObject(hdcMem, hbmOld);
                DeleteDC(hdcMem);
                DeleteObject(MemBitmap);
            }

            EndPaint(hwnd, &ps);
            return 0;
        }

        case WM_SIZE: {
            InvalidateRect(hwnd, NULL, FALSE);
            break;
        }

        case WM_LBUTTONDOWN: {
            WORD xPos = lParam & 0x0000FFFF;
            WORD yPos = (lParam >> 16) & 0x0000FFFF;
            if( handleMouseDown(xPos, yPos) )
                return 0;
            break;
        }

        case WM_LBUTTONUP: {
            WORD xPos = lParam & 0x0000FFFF;
            WORD yPos = (lParam >> 16) & 0x0000FFFF;
            if( handleMouseUp(xPos, yPos) )
                return 0;
            break;
        }

        case WM_MOUSEMOVE: {
            WORD xPos = lParam & 0x0000FFFF;
            WORD yPos = (lParam >> 16) & 0x0000FFFF;

            if (setMousePos(xPos, yPos))
            {
                InvalidateRect(hwnd, NULL, FALSE);
            }

            return 0;
        }

        case WM_CLOSE: {
            DestroyWindow(hwnd);
            return 0;
        }

        case WM_DESTROY: {
            EB_EMIT(closed);
            PostQuitMessage(0);
            return 0;
        }
        }

        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }

    virtual void createWindow()
    {
        LPCTSTR CLASS_NAME = TEXT("EBGuiWindow");

        Gdiplus::GdiplusStartupInput gdiplusStartupInput;
        ULONG_PTR gdiplusToken;
        GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

        WNDCLASS wc = {};

        wc.lpfnWndProc = &EBGuiWindow::WindowProcBase;
        wc.hInstance = hInstance;
        wc.lpszClassName = CLASS_NAME;
        wc.hCursor = LoadCursor(hInstance, IDC_ARROW);
        RegisterClass(&wc);

        hwnd = CreateWindowEx(0,                    // Optional window styles.
                              CLASS_NAME,           // Window class
                              TEXT("No Title Set"), // Window text
                              WS_OVERLAPPEDWINDOW,  // Window style

                              // Size and position
                              CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,

                              NULL,      // Parent window
                              NULL,      // Menu
                              hInstance, // Instance handle
                              this       // Additional application data
        );

        if (hwnd == NULL)
        {
            /// TODO: Failed.... Throw an exeption
            exit(1);
        }

        ShowWindow(hwnd, SW_SHOW);

        MSG msg = {};
        while (GetMessage(&msg, NULL, 0, 0) > 0)
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        Gdiplus::GdiplusShutdown(gdiplusToken);
    }
};

} // namespace EBCpp

#endif