/*
 * EBCpp
 *
 * Copyright (C) 2020 Carsten (Tropby)
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

#include "../../EBEvent.hpp"
#include "../../EBSemaphore.hpp"

#include "../../EBOs.hpp"

#include <gdiplus.h>

#include <iostream>
#include <memory>
#include <thread>


#ifndef WINMAIN
#define WINMAIN
static inline HINSTANCE hInstance;
int main();
inline int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow)
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
    EBGuiWindow() : EBGuiWindowBase(), hwnd(0), windowThread(std::thread(&EBGuiWindow::createWindow, this))
    {
    }

    ~EBGuiWindow()
    {
        windowThread.join();
    }

    virtual void setTitle(std::string title)
    {
        // WAITING FOR WINDOW CREATED
        /// TODO: Wait condition
        while (!hwnd)
            ;
        SetWindowText(hwnd, title.c_str());
    }

    virtual void invalidate()
    {
        InvalidateRect(hwnd, NULL, FALSE);
    }

    virtual void resized(int w, int h)
    {
        EBGuiWindowBase::resized(w, h);
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

                // Prepare the widget bounderies
                for (EBObjectPointer<EBGuiWidget> w : widgets)
                {
                    w->prepare(0, 0, width, height);
                }

                // Fill the Background wird white
                Gdiplus::SolidBrush brush(Gdiplus::Color(255, 255, 255, 255));
                graphics.FillRectangle(&brush, 0, 0, width, height);

                // Create the Widget
                std::list<EBObjectPointer<EBGuiRenderer>> list;
                for (EBObjectPointer<EBGuiWidget> w : widgets)
                {
                    // Create rendering instructions for the widget
                    w->render(list);

                    for (EBObjectPointer<EBGuiRenderer> l : list)
                    {
                        l->render(graphics);
                    }
                }

                BitBlt(hdc, ps.rcPaint.left, ps.rcPaint.top, ps.rcPaint.right, ps.rcPaint.bottom, hdcMem,
                       ps.rcPaint.left, ps.rcPaint.top, SRCCOPY);

                // Free Memory
                SelectObject(hdcMem, hbmOld);
                DeleteDC(hdcMem);
                DeleteObject(MemBitmap);
            }

            EndPaint(hwnd, &ps);
            return 0;
        }

        case WM_SIZE: {
            w = lParam & 0x0000FFFF;
            h = (lParam >> 16) & 0x0000FFFF;
            resized(w, h);
            return 0;
        }

        case WM_LBUTTONDOWN: {
            WORD xPos = lParam & 0x0000FFFF;
            WORD yPos = (lParam >> 16) & 0x0000FFFF;
            if (handleMouseDown(xPos, yPos))
                return 0;
            break;
        }

        case WM_LBUTTONUP: {
            WORD xPos = lParam & 0x0000FFFF;
            WORD yPos = (lParam >> 16) & 0x0000FFFF;
            if (handleMouseUp(xPos, yPos))
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

        case WM_CHAR: {
            handleKeyPress(wParam);
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

        hwnd = CreateWindowEx(0,                                              // Optional window styles.
                              CLASS_NAME,                                     // Window class
                              TEXT("No Title Set"),                           // Window text
                              WS_TILEDWINDOW /* & ~WS_SIZEBOX & ~WS_SYSMENU */, // Window style

                              // Size and position
                              CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,

                              NULL,      // Parent window
                              NULL,      // Menu
                              hInstance, // Instance handle
                              this       // Additional application data
        );

        if (hwnd == NULL)
        {
            EB_EXCEPTION("Can not create window!");
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