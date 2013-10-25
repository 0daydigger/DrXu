#ifndef _ntddk_
#include <ntddk.h>
#define _ntddk_
#endif


#define FILE_DEVICE_DRXU 0x19930410
#define IOCTL_TEST \
            CTL_CODE(FILE_DEVICE_UNKNOWN, 0x800, METHOD_BUFFERED, FILE_ANY_ACCESS)
//注册Symbolic Link要用的参数

const WCHAR deviceLinkBuffer[] = L"\\??\\DrXu";
const WCHAR deviceNameBuffer[] = L"\\Device\\Drxu";