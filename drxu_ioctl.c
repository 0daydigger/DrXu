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
	DbgPrint("[DrXu_Kernel_OnStubDispatch]:I'm called!:)");
	IoCompleteRequest(Irp,
					  IO_NO_INCREMENT );
	return STATUS_SUCCESS;
}
NTSTATUS DrXuIoDispatch(IN PDEVICE_OBJECT DeviceObject,
						IN PIRP Irp )
{
	NTSTATUS            Status = STATUS_SUCCESS;
    PIO_STACK_LOCATION    irpStack;
    ULONG                inBufLength, outBufLength;
    ULONG                i = 0;
    ULONG                ioControlCode;
    UCHAR                *InputBuffer, *OutputBuffer;
    
    irpStack = IoGetCurrentIrpStackLocation(Irp);
    
    inBufLength = irpStack->Parameters.DeviceIoControl.InputBufferLength;
    outBufLength = irpStack->Parameters.DeviceIoControl.OutputBufferLength;
    ioControlCode = irpStack->Parameters.DeviceIoControl.IoControlCode;
    
    if (ioControlCode == IOCTL_TEST)
    {
        DbgPrint("[Test] IOCTL_TEST : 0x%X", ioControlCode);
        // 读取数据
        InputBuffer = (UCHAR *)Irp->AssociatedIrp.SystemBuffer;
        for (i = 0; i < inBufLength; i++)
        {
            DbgPrint("[Test] %X\n", InputBuffer[i]);
        }
        
        // 写入数据
        OutputBuffer = (UCHAR *)Irp->AssociatedIrp.SystemBuffer;
        memset(OutputBuffer, 0xBB, outBufLength);
        
        Irp->IoStatus.Information = outBufLength;
    } 
    else
    {
        DbgPrint("[Test] Unknown IOCTL: 0x%X (%04X,%04X)", \
                    ioControlCode, DEVICE_TYPE_FROM_CTL_CODE(ioControlCode), IoGetFunctionCodeFromCtlCode(ioControlCode));
 
        Status = STATUS_INVALID_PARAMETER;
        Irp->IoStatus.Information = 0;
    }    
    // 完成IRP
    Irp->IoStatus.Status = Status;
    IoCompleteRequest(Irp, IO_NO_INCREMENT);
    
    return Status;
}