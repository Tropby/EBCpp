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
 *  Created on: 2021-01-23
 *      Author: Carsten (Tropby)
 */

#pragma once

#include "EBList.hpp"
#include "EBObject.hpp"
#include "EBString.hpp"
#include "profile/EBLogger.hpp"

#ifdef __WIN32__

#include <Winspool.h>
#include <codecvt>
#include <gdiplus.h>
#include <locale>

namespace EBCpp
{
class EBPrinter : public EBObject<EBPrinter>
{
public:
    static EBList<EBString> getPrinterList()
    {
        PRINTER_INFO_4 printerInfo[20];
        DWORD bytesNeeded = 0;
        DWORD count = 0;
        BOOL returnValue = EnumPrinters(PRINTER_ENUM_LOCAL, NULL, 4, (PBYTE)printerInfo, sizeof(PRINTER_INFO_4) * 20,
                                        &bytesNeeded, &count);

        if (returnValue == 0)
        {
            return EBList<EBString>();
        }

        EBList<EBString> result;
        for (int i = 0; i < count; i++)
        {
            result.append(printerInfo[i].pPrinterName);
        }

        return result;
    }

    static bool printImage(EBString filename, EBString printerName)
    {
        DEVMODE* pDM;
        LONG dmLen;
        HANDLE prnHandle;
        HDC hDC;
        UINT dimCount, frameCount;
        GUID* pDimIDs;
        GUID pageGuid;
        float width, height;
        DOCINFO di = {sizeof(DOCINFO), "Printing My Image"};
        ////

        EB_LOG_DEBUG("OpenPrinter " << printerName);
        if (!OpenPrinter((LPSTR)printerName.dataPtr(), &prnHandle, 0))
        {
            EB_LOG_DEBUG("Can not open printer " << printerName);
            return 0;
        }

        dmLen = DocumentProperties(0, prnHandle, (LPSTR)printerName.dataPtr(), 0, 0, 0);
        pDM = (DEVMODE*)new char[dmLen];
        DocumentProperties(0, prnHandle, (LPSTR)printerName.dataPtr(), pDM, 0, DM_OUT_BUFFER);
        ////

        EB_LOG_DEBUG("Create Printer DC for " << printerName);
        hDC = CreateDC((LPSTR)printerName.dataPtr(), (LPSTR)printerName.dataPtr(), 0, pDM);
        delete pDM;

        if (!hDC)
        {
            EB_LOG_DEBUG("Create Printer DC for " << printerName << " failed!");
            ClosePrinter(prnHandle);
            return 0;
        }

        EB_LOG_DEBUG("Start Document on " << printerName);
        if (StartDoc(hDC, &di) == SP_ERROR)
        {
            EB_LOG_DEBUG("Start document for " << printerName << " failed!");
            ClosePrinter(prnHandle);
            return 0;
        }
        
        Gdiplus::Graphics graphics(hDC);

        // Load image from file
        std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
        std::wstring wstr = converter.from_bytes(filename.toStdString());
        Gdiplus::Image image(wstr.c_str());

        int errorCode = image.GetLastStatus();
        if( errorCode != Gdiplus::GpStatus::Ok )
        {
            EB_LOG_ERROR("Can not open file " << filename << " Errorcode: " << errorCode);
            return false;
        }

        dimCount = image.GetFrameDimensionsCount();
        pDimIDs = new GUID[dimCount];

        image.GetFrameDimensionsList(pDimIDs, dimCount);
        frameCount = image.GetFrameCount(&pDimIDs[0]);
        delete pDimIDs;

        pageGuid = Gdiplus::FrameDimensionPage;

        /// TODO: get the read width / height of the page
        float maxWidth = 210;
        float maxHeight = 297;

        for (UINT i = 0; i < frameCount; i++)
        {
            if( StartPage(hDC) == SP_ERROR )
                EB_LOG_DEBUG("Start page failed on " << printerName);

            image.SelectActiveFrame(&pageGuid, i);
            graphics.SetPageUnit(Gdiplus::UnitMillimeter);

            width = maxWidth;
            height =  image.GetHeight() * ( maxWidth / image.GetWidth() );

            EB_LOG_DEBUG("Draw image for page " << i << " on " << printerName);
            graphics.DrawImage(&image, 10.f, 10.f, ( width - 20 ), ( height - 20 ) );

            if( EndPage(hDC) == SP_ERROR )
                EB_LOG_DEBUG("End page failed on " << printerName);
        }

        if( EndDoc(hDC) == SP_ERROR )
            EB_LOG_DEBUG("End document failed on " << printerName);

        if( !ClosePrinter(prnHandle) )
            EB_LOG_DEBUG("Close printer failed on " << printerName);

        if( frameCount == 0 )
            EB_LOG_DEBUG("Can not draw image with 0 frame count!");
            

        return (frameCount > 0);
    }

private:
};
} // namespace EBCpp

#else
#error EBPrinter is not designed to work under linux!
#endif