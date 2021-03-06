
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
* See license agreement file for full license terms including other restrictions.
*****************************************************************************
*
* Comments:
*
*   Serial Embedded Debug Server header file
*
*
*END************************************************************************/
#ifndef __edserial_h__
#define __edserial_h__

/* Error codes */
#define EDS_SERIAL_CANT_CREATE     (EDS_SERIAL_ERROR_BASE | 0x01)
#define EDS_SERIAL_ALREADY_CREATED (EDS_SERIAL_ERROR_BASE | 0x02)
#define EDS_SERIAL_CANT_DESTROY    (EDS_SERIAL_ERROR_BASE | 0x03)
#define EDS_SERIAL_IO_FAILED       (EDS_SERIAL_ERROR_BASE | 0x04)

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __TAD_COMPILE__
extern _mqx_uint _eds_serial_create_component(char *, uint32_t, _mqx_uint);
extern _mqx_uint _eds_serial_add(char *, uint32_t, _mqx_uint);
extern uint32_t _eds_serial_remove(void);
#endif

#ifdef __cplusplus
}
#endif


#endif

/* EOF */
