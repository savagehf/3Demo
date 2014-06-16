//-----------------------------------------------------------------------------
// File: MouseNon.cpp
//
// Desc: Demonstrates an application which receives relative mouse data
//       in non-exclusive mode via a dialog timer.
//
// Copyright (c) 1998-1999 Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include "stdafx.h"
#include "Std.h"
#include "HDef.h"
#define INITGUID

#define DIRECTINPUT_VERSION 0x0700 //时间：2014-5-20,DirectInputCreate 没定义的bug修改。
#include <dinput.h>
#include "MouseMan.h"

 #pragma comment(lib,"dinput.lib")

//#include "MouseMan.h"
//#include "resource.h"

//-----------------------------------------------------------------------------
// Global variables for the DirectMusic sample 
//-----------------------------------------------------------------------------
IDirectInput*           g_pDI       = NULL;
IDirectInputDevice*     g_pMouse    = NULL;
//HINSTANCE               g_hInst     = NULL;
//BOOL                    g_bActive   = TRUE;




//-----------------------------------------------------------------------------
// Function: InitDirectInput
//
// Description: 
//      Initialize the DirectInput variables.
//
//-----------------------------------------------------------------------------
r_val MouseManInit(HINSTANCE hInst,HWND hwnd)
{
    HRESULT hr;

		////static bool com_init=false;
		////if (!com_init)
		//{
		//	// Initialize COM.
		//	if( FAILED(hr=CoInitialize(NULL)) )
		//		return r_err;
		//	//com_init=true;
		//}

    // Register with the DirectInput subsystem and get a pointer
    // to a IDirectInput interface we can use.
    hr = DirectInputCreate( hInst, DIRECTINPUT_VERSION, &g_pDI, NULL );
    if ( FAILED(hr) ) 
        return HErrorN("DirectInputCreate");

    // Obtain an interface to the system mouse device.
    hr = g_pDI->CreateDevice( GUID_SysMouse, &g_pMouse, NULL );
    if ( FAILED(hr) ) 
        return HErrorN("DirectInput::CreateDevice");

    // Set the data format to "mouse format" - a predefined data format 
    //
    // A data format specifies which controls on a device we
    // are interested in, and how they should be reported.
    //
    // This tells DirectInput that we will be passing a
    // DIMOUSESTATE structure to IDirectInputDevice::GetDeviceState.
    hr = g_pMouse->SetDataFormat( &c_dfDIMouse );
    if ( FAILED(hr) ) 
        return HErrorN("DirectInputMouse::SetDataFormat");

    // Set the cooperativity level to let DirectInput know how
    // this device should interact with the system and with other
    // DirectInput applications.
    hr = g_pMouse->SetCooperativeLevel( hwnd, 
                                        //DISCL_NONEXCLUSIVE | DISCL_FOREGROUND);
                                        DISCL_EXCLUSIVE | DISCL_FOREGROUND);
    if ( FAILED(hr) ) 
        return HErrorN("DirectInputMouse::SetCooperativeLevel");

    g_pMouse->Acquire();

    return r_ok;
}




//-----------------------------------------------------------------------------
// Function: SetAcquire
//
// Description: 
//      Acquire or unacquire the mouse, depending on if the app is active
//       Input device must be acquired before the GetDeviceState is called
//
//-----------------------------------------------------------------------------
/*HRESULT SetAcquire( HWND hDlg )
{
    char szText[128];
    HWND hDlgText;

    // nothing to do if g_pMouse is NULL
    if (NULL == g_pMouse)
        return S_FALSE;
    if (g_bActive) 
    {
        // acquire the input device 
        g_pMouse->Acquire();
    } 
    else 
    {
        g_pMouse->Unacquire();
    }

    return S_OK;
}*/




//-----------------------------------------------------------------------------
// Function: UpdateInputState
//
// Description: 
//      Get the input device's state and display it.
//
//-----------------------------------------------------------------------------
r_val MouseManGetDepl(int *dx,int *dy,int *bt)
//HRESULT UpdateInputState( HWND hDlg )
{
    if (NULL != g_pMouse) 
    {
        DIMOUSESTATE dims;      // DirectInput mouse state structure
        HRESULT hr;

        // if input is lost then acquire and keep trying 
        do
        {
            // get the input's device state, and put the state in dims
            hr = g_pMouse->GetDeviceState( sizeof(DIMOUSESTATE), &dims );

            if ( hr == DIERR_INPUTLOST )
            {
                // DirectInput is telling us that the input stream has
                // been interrupted.  We aren't tracking any state
                // between polls, so we don't have any special reset
                // that needs to be done.  We just re-acquire and
                // try again.
                hr = g_pMouse->Acquire();
                if ( FAILED(hr) )  
									hr = DIERR_INPUTLOST;
                    //return r_err;
            }
        }
				while ( hr == DIERR_INPUTLOST );

        if ( FAILED(hr) )  
            return r_err;

				*dx=dims.lX;
				*dy=dims.lY;

				if (bt)
				{
					*bt=0;
					if ((dims.rgbButtons[0] & 0x80)) *bt|=MOUSE_BTG;
					if ((dims.rgbButtons[1] & 0x80)) *bt|=MOUSE_BTD;
					if ((dims.rgbButtons[2] & 0x80)) *bt|=MOUSE_BTC;
				}
/*
        // The dims structure now has the state of the mouse, so 
        // display mouse coordinates (x, y, z) and buttons.
        wsprintf( szNewText, "(%d, %d, %d) %c %c %c %c",
                     dims.lX, dims.lY, dims.lZ,
                     (dims.rgbButtons[0] & 0x80) ? '0' : ' ',
                     (dims.rgbButtons[1] & 0x80) ? '1' : ' ',
                     (dims.rgbButtons[2] & 0x80) ? '2' : ' ',
                     (dims.rgbButtons[3] & 0x80) ? '3' : ' ');
*/
    }

    return r_ok;
}




//-----------------------------------------------------------------------------
// Function: FreeDirectInput
//
// Description: 
//      Initialize the DirectInput variables.
//
//-----------------------------------------------------------------------------
void MouseManQuit()
//HRESULT FreeDirectInput()
{
    // Unacquire and release any DirectInputDevice objects.
    if (NULL != g_pMouse) 
    {
        // Unacquire the device one last time just in case 
        // the app tried to exit while the device is still acquired.
        g_pMouse->Unacquire();

        g_pMouse->Release();
        g_pMouse = NULL;
    }

    // Release any DirectInput objects.
    if (NULL != g_pDI) 
    {
        g_pDI->Release();
        g_pDI = NULL;
    }

		//CoUninitialize();

    //return S_OK;
}

