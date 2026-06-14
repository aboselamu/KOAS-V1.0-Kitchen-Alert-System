#ifndef __CMSIS_VERSION_H
#define __CMSIS_VERSION_H

/* CMSIS Core version definitions */
#define __CM_CMSIS_VERSION_MAIN  ( 5U) /*!< [31:16] main version */
#define __CM_CMSIS_VERSION_SUB   ( 6U) /*!< [15:0]  sub version */
#define __CM_CMSIS_VERSION       ((__CM_CMSIS_VERSION_MAIN << 16U) | \
                                   __CM_CMSIS_VERSION_SUB           ) /*!< CMSIS Core version number */
#endif