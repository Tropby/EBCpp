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
            height =  maxWidth * ( image.GetWidth() / image.GetHeight() );

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