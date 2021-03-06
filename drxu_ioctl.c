#ifndef _ntddk_
#include <ntddk.h>
#define _ntddk_
#endif

#ifndef _drxu_ioctl_
#define _drxu_ioctl_
#include <drxu_ioctl.h>
#endif

NTSTATUS OnStubDispatch(IN PDEVICE_OBJECT DeviceObject,
						IN PIRP Irp )
{
	Irp->IoStatus.Status = STATUS_SUCCESS;
	IoCompleteRequest(Irp,
					  IO_NO_INCREMENT );
	return STATUS_SUCCESS;
}