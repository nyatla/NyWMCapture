//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
//
// Use of this source code is subject to the terms of the Microsoft end-user
// license agreement (EULA) under which you licensed this SOFTWARE PRODUCT.
// If you did not accept the terms of the EULA, you are not authorized to use
// this source code. For a copy of the EULA, please see the LICENSE.RTF on your
// install media.
//
//==========================================================================;
//
//  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
//  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
//  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
//  PURPOSE.
//
//
//--------------------------------------------------------------------------;
//  File:       ddmm.cpp
//  Content:    Routines for using DirectDraw on a multimonitor system
//--------------------------------------------------------------------------;

#include <streams.h>
#include <ddraw.h>
#include "ddmm.h"

/*
 * FindDeviceCallback
 */
typedef struct {
	LPSTR   szDevice;
	GUID*   lpGUID;
	GUID    GUID;
	BOOL    fFound;
}   FindDeviceData;

BOOL CALLBACK FindDeviceCallback(GUID* lpGUID, LPSTR szName, LPSTR szDevice, LPVOID lParam)
{
	FindDeviceData *p = (FindDeviceData*)lParam;

	if (lstrcmpiA(p->szDevice, szDevice) == 0) {
	    if (lpGUID) {
		p->GUID = *lpGUID;
		p->lpGUID = &p->GUID;
	    } else {
		p->lpGUID = NULL;
	    }
	    p->fFound = TRUE;
	    return FALSE;
	}
	return TRUE;
}


BOOL CALLBACK FindDeviceCallbackEx(GUID* lpGUID, LPSTR szName, LPSTR szDevice, LPVOID lParam, HMONITOR hMonitor)
{
	FindDeviceData *p = (FindDeviceData*)lParam;

	if (lstrcmpiA(p->szDevice, szDevice) == 0) {
	    if (lpGUID) {
		p->GUID = *lpGUID;
		p->lpGUID = &p->GUID;
	    } else {
		p->lpGUID = NULL;
	    }
	    p->fFound = TRUE;
	    return FALSE;
	}
	return TRUE;
}


/*
 * DirectDrawCreateFromDevice
 *
 * create a DirectDraw object for a particular device
 */
IDirectDraw * DirectDrawCreateFromDevice(LPSTR szDevice, PDRAWCREATE DirectDrawCreateP, PDRAWENUM DirectDrawEnumerateP)
{
	IDirectDraw*    pdd = NULL;
	FindDeviceData  find;

	if (szDevice == NULL) {
		DirectDrawCreateP(NULL, &pdd, NULL);
		return pdd;
	}

	find.szDevice = szDevice;
	find.fFound   = FALSE;
	DirectDrawEnumerateP(FindDeviceCallback, (LPVOID)&find);

	if (find.fFound)
	{
		//
		// In 4bpp mode the following DDraw call causes a message box to be popped
		// up by DDraw (!?!).  It's DDraw's fault, but we don't like it.  So we
		// make sure it doesn't happen.
		//
		UINT ErrorMode = SetErrorMode(SEM_FAILCRITICALERRORS);
		DirectDrawCreateP(find.lpGUID, &pdd, NULL);
		SetErrorMode(ErrorMode);
	}

	return pdd;
}


/*
 * DirectDrawCreateFromDeviceEx
 *
 * create a DirectDraw object for a particular device
 */
IDirectDraw * DirectDrawCreateFromDeviceEx(LPSTR szDevice, PDRAWCREATE DirectDrawCreateP, LPDIRECTDRAWENUMERATEEXA DirectDrawEnumerateExP)
{
	IDirectDraw*    pdd = NULL;
	FindDeviceData  find;

	if (szDevice == NULL) {
		DirectDrawCreateP(NULL, &pdd, NULL);
		return pdd;
	}

	find.szDevice = szDevice;
	find.fFound   = FALSE;
	DirectDrawEnumerateExP(FindDeviceCallbackEx, (LPVOID)&find,
					DDENUM_ATTACHEDSECONDARYDEVICES);

	if (find.fFound)
	{
		//
		// In 4bpp mode the following DDraw call causes a message box to be popped
		// up by DDraw (!?!).  It's DDraw's fault, but we don't like it.  So we
		// make sure it doesn't happen.
		//
		UINT ErrorMode = SetErrorMode(SEM_FAILCRITICALERRORS);
		DirectDrawCreateP(find.lpGUID, &pdd, NULL);
		SetErrorMode(ErrorMode);
	}

	return pdd;
}
