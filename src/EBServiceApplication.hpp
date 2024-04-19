/*
* EBCpp Project
* https://github.com/Tropby/EBCpp
* 
* ---
* 
* MIT License
* 
* Copyright (c) 2024 Carsten (Tropby)
* 
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
* 
* The above copyright notice and this permission notice shall be included in all
* copies or substantial portions of the Software.
* 
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*
 *  Created on: 2022-06-17
 *      Author: Carsten (Tropby)
 */

#pragma once

#include "EBObject.hpp"
#include "EBList.hpp"
#include "EBString.hpp"
#include "EBEventLoop.hpp"
#include "EBApplication.hpp"
#include "profile/EBLogger.hpp"
#include "profile/EBLogFile.hpp"

SERVICE_STATUS ServiceStatus;
SERVICE_STATUS_HANDLE ServiceStatusHandle;
HANDLE ServiceStopEvent = INVALID_HANDLE_VALUE;

void WINAPI EBServiceControlHandler(DWORD control)
{
    switch (control) {
        case SERVICE_CONTROL_PAUSE:
            ServiceStatus.dwCurrentState = SERVICE_PAUSED;
            break;
        case SERVICE_CONTROL_CONTINUE:
            ServiceStatus.dwCurrentState = SERVICE_RUNNING;
            break;
        case SERVICE_CONTROL_STOP:
        case SERVICE_CONTROL_SHUTDOWN:
            ServiceStatus.dwCurrentState = SERVICE_STOPPED;
        break;
    }
    SetServiceStatus(ServiceStatusHandle, &ServiceStatus);
}

void EBServiceThread()
{
    EBUtils::setThreadName(EBString("EBServiceThread"));

    EBPtr<MAIN_CLASS> mainClass = EBCreate<MAIN_CLASS>(); 
    EBCpp::EBEventLoop::getInstance()->exec();
}

VOID WINAPI EBServiceMain (DWORD argc, LPTSTR *argv)
{
    EBCpp::EBApplication::setArguments(argc, argv);

    ServiceStatusHandle = RegisterServiceCtrlHandler(EB_SERVICE_NAME, EBServiceControlHandler);
    if (ServiceStatusHandle == 0)
        return;

    // Setup service
    ZeroMemory (&ServiceStatus, sizeof (ServiceStatus));
    ServiceStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
    ServiceStatus.dwCurrentState = SERVICE_RUNNING;
    ServiceStatus.dwControlsAccepted = SERVICE_ACCEPT_PAUSE_CONTINUE | SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_SHUTDOWN;
    ServiceStatus.dwWin32ExitCode = NO_ERROR;
    ServiceStatus.dwServiceSpecificExitCode = 0;
    ServiceStatus.dwCheckPoint = 0;
    ServiceStatus.dwWaitHint = 0;

    EB_LOG_DEBUG("SERVICE SET RUNNING!");
    if (SetServiceStatus (ServiceStatusHandle , &ServiceStatus) == FALSE)
    {
        return;
    }

    EB_LOG_DEBUG("START SERVICE THREAD!");
    std::thread thread(EBServiceThread);
    thread.join();
    EB_LOG_DEBUG("END SERVICE THREAD!");

    // Tell the service controller we are stopped
    ServiceStatus.dwControlsAccepted = 0;
    ServiceStatus.dwCurrentState = SERVICE_STOPPED;
    ServiceStatus.dwWin32ExitCode = 0;
    ServiceStatus.dwCheckPoint = 3;
 
    SetServiceStatus (ServiceStatusHandle, &ServiceStatus);

    EB_LOG_DEBUG("END SERVICE!");
}

int main(int argc, char** argv)
{
    OutputDebugString("Main");

    EBCpp::EBLogger::addLogger(EBCreate<EBCpp::EBLogFile>("C:\\test.log").cast<EBCpp::EBLog>());
    EB_LOG_DEBUG("SERVICE MAIN CALLED!");

    const char sn[] = EB_SERVICE_NAME;
    SERVICE_TABLE_ENTRY StartTable[] = {{(LPSTR)sn, EBServiceMain}, {NULL, NULL}};
    if( StartServiceCtrlDispatcher(StartTable) == FALSE )
    {
        OutputDebugString("My Sample Service: Main: StartServiceCtrlDispatcher returned error");
        return GetLastError ();
    }
    
    return 0;
}