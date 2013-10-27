#ifndef _ntddk_
#include <ntddk.h>
#define _ntddk_
#endif

#ifndef _drxu_ioctl_
#define _drxu_ioctl_
#include <drxu_ioctl.h>
#endif

PKEVENT		NtLoadDriverEvent = NULL;

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
    
    //if (ioControlCode == IOCTL_TEST)
	DbgPrint("[SYSTEM][DrXuIoDispatch Entered");
    switch( ioControlCode )
	{
	/*  注意这里的数据流方向！！！所有的WRITE/READ都是应用层面的！ */
		case IOCTL_TEST:
			DbgPrint("[DrXuIoDispatch] IOCTL_TEST,ioControlCode: 0x%X", ioControlCode);
			break;
		case IOCTL_DRXU_TEST_WRITE:
			// 读取数据
			DbgPrint("[DrXuIoDispatch]IOCTL_DRXU_TEST_WRITE Entered,ioControlCode: 0x%X", ioControlCode);
			InputBuffer = (UCHAR *)Irp->AssociatedIrp.SystemBuffer;
			for (i = 0; i < inBufLength; i++)
			{
				DbgPrint("[DrXuIoDispatch] %c\n", InputBuffer[i]);
			}
			break;
		case IOCTL_DRXU_TEST_READ:
			// 写入数据
			DbgPrint("[DrXuIoDispatch]IOCTL_DRXU_TEST_READ Entered,ioControlCode: 0x%X", ioControlCode);
			OutputBuffer = (UCHAR *)Irp->AssociatedIrp.SystemBuffer;
			memcpy(OutputBuffer, "Hello From Sys", outBufLength);
			
			Irp->IoStatus.Information = outBufLength;
			break;
		case IOCTL_DRXU_OPEN_EVENT_NtLoadDriver:
			RtlInitUnicodeString(&EventNtLoadDriverName,L"\\BaseNamedObjects\\eventNtLoadDriver");
			NtLoadDriverEvent = IoCreateNotificationEvent(&EventNtLoadDriverName,&EventNtLoadDriverNameHandleFromApp);
			
			
			if (NtLoadDriverEvent != NULL)
			{
				DbgPrint("[DRXU_SYSTEM]:NtLoadDriverEvent Got, value is %d",NtLoadDriverEvent);
				ObReferenceObject(NtLoadDriverEvent); //增加引用次数防止意外发生
				
				//TODO:在这里测试用意，先试试看能不能set好啦！
				KeSetEvent(NtLoadDriverEvent,0,FALSE);//激活事件。
				DbgPrint("[DRXU_SYSTEM]:Event has been set!");
				Status = STATUS_SUCCESS;
			}
			else
			{
				DbgPrint("[DrXuIoDispatch][IOCTL_DRXU_OPEN_EVENT_NtLoadDriver] Failded,Cannot Open event!");
				Status = STATUS_UNSUCCESSFUL;
			}
			break;
		default:
        DbgPrint("[DrXuIoDispatch] Unknown IOCTL: 0x%X (%04X,%04X)", \
                    ioControlCode, DEVICE_TYPE_FROM_CTL_CODE(ioControlCode), IoGetFunctionCodeFromCtlCode(ioControlCode));
 
        Status = STATUS_INVALID_PARAMETER;
        Irp->IoStatus.Information = 0;
    }    
    // 完成IRP
    Irp->IoStatus.Status = Status;
    IoCompleteRequest(Irp, IO_NO_INCREMENT);
    
    return Status;
}