/*HEADER**********************************************************************
*
* Copyright 2008 Freescale Semiconductor, Inc.
* Copyright 2004-2008 Embedded Access Inc.
* Copyright 1989-2008 ARC International
*
* This software is owned or controlled by Freescale Semiconductor.
* Use of this software is governed by the Freescale MQX RTOS License
* distributed with this Material.
* See the MQX_RTOS_LICENSE file distributed for more details.
*
* Brief License Summary:
* This software is provided in source form for you to use free of charge,
* but it is not open source software. You are allowed to use this software
* but you cannot redistribute it or derivative works of it in source form.
* The software may be used only in connection with a product containing
* a Freescale microprocessor, microcontroller, or digital signal processor.
* See license agreement file for full license terms including other
* restrictions.
*****************************************************************************
*
* Comments:
*
*   The file exports USB KHCI host initial structure
*
*
*END************************************************************************/
#ifndef __if_dev_khci_h__
#define __if_dev_khci_h__ 1

#include <mqx.h> //pointer types

#ifdef __cplusplus
extern "C" {
#endif

extern const struct usb_dev_callback_functions_struct _usb_khci_dev_callback_table;

typedef struct usb_khci_dev_init_struct {
    void         *BASE_PTR;
    _mqx_uint     VECTOR;
    _mqx_uint     PRIORITY;
    _mqx_uint     MAX_ENDPOINTS;
} USB_KHCI_DEV_INIT_STRUCT, * USB_KHCI_DEV_INIT_STRUCT_PTR;

#ifdef __cplusplus
}
#endif

#endif
