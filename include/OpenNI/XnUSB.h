/****************************************************************************
*                                                                           *
*  OpenNI 1.1 Alpha                                                         *
*  Copyright (C) 2011 PrimeSense Ltd.                                       *
*                                                                           *
*  This file is part of OpenNI.                                             *
*                                                                           *
*  OpenNI is free software: you can redistribute it and/or modify           *
*  it under the terms of the GNU Lesser General Public License as published *
*  by the Free Software Foundation, either version 3 of the License, or     *
*  (at your option) any later version.                                      *
*                                                                           *
*  OpenNI is distributed in the hope that it will be useful,                *
*  but WITHOUT ANY WARRANTY; without even the implied warranty of           *
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the             *
*  GNU Lesser General Public License for more details.                      *
*                                                                           *
*  You should have received a copy of the GNU Lesser General Public License *
*  along with OpenNI. If not, see <http://www.gnu.org/licenses/>.           *
*                                                                           *
****************************************************************************/
#ifndef _XN_USB_H_
#define _XN_USB_H_

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include "XnOS.h"

//---------------------------------------------------------------------------
// Structures & Enums
//---------------------------------------------------------------------------
typedef enum {
	XN_USB_DEVICE_LOW_SPEED = 0,
	XN_USB_DEVICE_FULL_SPEED,
	XN_USB_DEVICE_HIGH_SPEED,
} XnUSBDeviceSpeed;

typedef enum {
	XN_USB_EP_BULK = 0,
	XN_USB_EP_ISOCHRONOUS,
	XN_USB_EP_INTERRUPT,
} XnUSBEndPointType;

typedef enum {
	XN_USB_DIRECTION_IN = 0,
	XN_USB_DIRECTION_OUT,
} XnUSBDirectionType;

typedef enum {
	XN_USB_CONTROL_TYPE_STANDARD = 0,
	XN_USB_CONTROL_TYPE_CLASS,
	XN_USB_CONTROL_TYPE_VENDOR,
} XnUSBControlType;

typedef enum {
	XN_USB_EVENT_DEVICE_CONNECT = 0,
	XN_USB_EVENT_DEVICE_DISCONNECT,
} XnUSBEventType;

struct XnUSBDeviceHandle;
struct XnUSBEndPointHandle;

typedef XnChar XnUSBConnectionString[XN_FILE_MAX_PATH];

typedef XnUSBDeviceHandle*  XN_USB_DEV_HANDLE;
typedef XnUSBEndPointHandle* XN_USB_EP_HANDLE;

typedef XnBool (XN_CALLBACK_TYPE* XnUSBReadCallbackFunctionPtr)(XnUChar* pBuffer, XnUInt32 nBufferSize, void* pCallbackData);
typedef XnBool (XN_CALLBACK_TYPE* XnUSBEventCallbackFunctionPtr)(XnUSBEventType USBEventType, XnChar* cpDevPath, void* pCallbackData);

//---------------------------------------------------------------------------
// Exported Function Declaration
//---------------------------------------------------------------------------
XN_C_API XnStatus xnUSBInit();
XN_C_API XnStatus xnUSBShutdown();

XN_C_API XnStatus XN_API_DEPRECATED("Use xnUSBEnumerateDevices() instead") xnUSBIsDevicePresent(XnUInt16 nVendorID, XnUInt16 nProductID, void* pExtraParam, XnBool* pbDevicePresent);

XN_C_API XnStatus xnUSBEnumerateDevices(XnUInt16 nVendorID, XnUInt16 nProductID, const XnUSBConnectionString** pastrDevicePaths, XnUInt32* pnCount);
XN_C_API void xnUSBFreeDevicesList(const XnUSBConnectionString* astrDevicePaths);

XN_C_API XnStatus XN_API_DEPRECATED("Use xnUSBOpenDeviceByPath() instead") xnUSBOpenDevice(XnUInt16 nVendorID, XnUInt16 nProductID, void* pExtraParam, void* pExtraParam2, XN_USB_DEV_HANDLE* pDevHandlePtr);
XN_C_API XnStatus xnUSBOpenDeviceByPath(const XnUSBConnectionString strDevicePath, XN_USB_DEV_HANDLE* pDevHandlePtr);
XN_C_API XnStatus xnUSBCloseDevice(XN_USB_DEV_HANDLE pDevHandle);

XN_C_API XnStatus xnUSBGetDeviceSpeed(XN_USB_DEV_HANDLE pDevHandle, XnUSBDeviceSpeed* pDevSpeed);

XN_C_API XnStatus xnUSBSetConfig(XN_USB_DEV_HANDLE pDevHandle, XnUInt8 nConfig);
XN_C_API XnStatus xnUSBGetConfig(XN_USB_DEV_HANDLE pDevHandle, XnUInt8* pnConfig);

XN_C_API XnStatus xnUSBSetInterface(XN_USB_DEV_HANDLE pDevHandle, XnUInt8 nInterface, XnUInt8 nAltInterface);
XN_C_API XnStatus xnUSBGetInterface(XN_USB_DEV_HANDLE pDevHandle, XnUInt8* pnInterface, XnUInt8* pnAltInterface);

XN_C_API XnStatus xnUSBOpenEndPoint(XN_USB_DEV_HANDLE pDevHandle, XnUInt16 nEndPointID, XnUSBEndPointType nEPType, XnUSBDirectionType nDirType, XN_USB_EP_HANDLE* pEPHandlePtr);
XN_C_API XnStatus xnUSBCloseEndPoint(XN_USB_EP_HANDLE pEPHandle);
XN_C_API XnStatus xnUSBGetEndPointMaxPacketSize(XN_USB_EP_HANDLE pEPHandle, XnUInt32* pnMaxPacketSize);

XN_C_API XnStatus xnUSBAbortEndPoint(XN_USB_EP_HANDLE pEPHandle);
XN_C_API XnStatus xnUSBFlushEndPoint(XN_USB_EP_HANDLE pEPHandle);
XN_C_API XnStatus xnUSBResetEndPoint(XN_USB_EP_HANDLE pEPHandle);

XN_C_API XnStatus xnUSBSendControl(XN_USB_DEV_HANDLE pDevHandle, XnUSBControlType nType, XnUInt8 nRequest, XnUInt16 nValue, XnUInt16 nIndex, XnUChar* pBuffer, XnUInt32 nBufferSize, XnUInt32 nTimeOut);
XN_C_API XnStatus xnUSBReceiveControl(XN_USB_DEV_HANDLE pDevHandle, XnUSBControlType nType, XnUInt8 nRequest, XnUInt16 nValue, XnUInt16 nIndex, XnUChar* pBuffer, XnUInt32 nBufferSize, XnUInt32* pnBytesReceived, XnUInt32 nTimeOut);

XN_C_API XnStatus xnUSBReadEndPoint(XN_USB_EP_HANDLE pEPHandle, XnUChar* pBuffer, XnUInt32 nBufferSize, XnUInt32* pnBytesReceived, XnUInt32 nTimeOut);
XN_C_API XnStatus xnUSBWriteEndPoint(XN_USB_EP_HANDLE pEPHandle, XnUChar* pBuffer, XnUInt32 nBufferSize, XnUInt32 nTimeOut);

XN_C_API XnStatus xnUSBQueueReadEndPoint(XN_USB_EP_HANDLE pEPHandle, XnUChar* pBuffer, XnUInt32 nBufferSize, XnUInt32 nTimeOut);
XN_C_API XnStatus xnUSBFinishReadEndPoint(XN_USB_EP_HANDLE pEPHandle, XnUInt32* pnBytesReceived, XnUInt32 nTimeOut);

XN_C_API XnStatus xnUSBInitReadThread(XN_USB_EP_HANDLE pEPHandle, XnUInt32 nBufferSize, XnUInt32 nNumBuffers, XnUInt32 nTimeOut, XnUSBReadCallbackFunctionPtr pCallbackFunction, void* pCallbackData);
XN_C_API XnStatus xnUSBShutdownReadThread(XN_USB_EP_HANDLE pEPHandle);

XN_C_API XnStatus xnUSBSetCallbackHandler(XnUInt16 nVendorID, XnUInt16 nProductID, void* pExtraParam, XnUSBEventCallbackFunctionPtr pCallbackFunction, void* pCallbackData);

#endif //_XN_USB_H_
