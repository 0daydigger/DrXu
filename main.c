#ifndef _ntddk_
#include <ntddk.h>
#define _ntddk_
#endif

#ifndef _ssdt_hook_
#define _ssdt_hook_
#include <ssdt_hook.h>
#endif

#ifndef _drxu_constant_
#define _drxu_constant_
#include <drxu_constant.h>
#endif

__declspec(dllimport) SSDT_Entry KeServiceDescriptorTable;

PMDL g_pmdlSystemCall;
PVOID *MappedSystemCallTable;
//在这里声明要HOOK的函数
ZWLOADDRIVER OldZwLoadDriver;


PDEVICE_OBJECT g_DrXuDevice;

//驱动卸载回调
VOID OnUnload(IN PDRIVER_OBJECT DriverObject)
{
	DbgPrint("Dr.Xu's Gentlmen Sword:Quiting...");


	//脱钩子，所有被钩的函数都要在这里恢复
	UNHOOK_SYSCALL(ZwLoadDriver,OldZwLoadDriver,NewZwLoadDriver);



	//如果之前取得过SSDT地址，那么这里要把映射取消
	if(g_pmdlSystemCall)
	{
		MmUnmapLockedPages(MappedSystemCallTable,g_pmdlSystemCall);
		IoFreeMdl(g_pmdlSystemCall);
	}
}
NTSTATUS OnStubDispatch(IN PDEVICE_OBJECT DeviceObject,
						IN PIRP Irp )
{
	Irp->IoStatus.Status = STATUS_SUCCESS;
	IoCompleteRequest(Irp,
					  IO_NO_INCREMENT );
	return STATUS_SUCCESS;
}
NTSTATUS DriverEntry(IN PDRIVER_OBJECT theDriverObject,
					 IN PUNICODE_STRING theRegistryPath)
{
	//变量区开始
	NTSTATUS ntStatus;
	UNICODE_STRING deviceNameUnicodeString;
	UNICODE_STRING deviceLinkUnicodeString;
	int i;
	//变量区结束
	/*  加载提示信息 */
	theDriverObject->DriverUnload = OnUnload;
	DbgPrint("Dr.Xu's Gentlmen sword on loading...");
	
	/*  分管IRP调度 
		注意：这里的所有调度就写成了一个函数，实际应该写成多个。
	*/
	for(i=0;i< IRP_MJ_MAXIMUM_FUNCTION; i++ )
	{
		theDriverObject->MajorFunction[i] = OnStubDispatch;
	}
	/* IRP调度分管结束 */
	

	/* Regedit Symbolic Link */
	RtlInitUnicodeString(&deviceNameUnicodeString,
							deviceNameBuffer);
	RtlInitUnicodeString(&deviceLinkUnicodeString,
							deviceLinkBuffer);
	//创建设备
	ntStatus = IoCreateDevice(theDriverObject,
							  0,
						      &deviceNameUnicodeString,
							  //defined in drxu_constant.h, value = 0x19930410
							  FILE_DEVICE_DRXU,
							  0,
							  TRUE,
							  &g_DrXuDevice);
	//创建设备成功的话，保存symbolic link
	if( NT_SUCCESS(ntStatus) )
	{
		DbgPrint("Create Symbolic Link");
		ntStatus = IoCreateSymbolicLink(&deviceLinkUnicodeString,
									    &deviceNameUnicodeString);
		if( NT_SUCCESS(ntStatus) )
		{
			DbgPrint("[DriverEntry]Symbolic Link Created,The Name is %wZ", &deviceLinkUnicodeString);
		}
	}
	
	
	
	/* Regedit over */
	
	
	/* 修改SSDT表内存属性  */
	//从SSDT读取原函数内存地址
	OldZwLoadDriver = (ZWLOADDRIVER)(SYSTEMSERVICE(ZwLoadDriver));

	//修改内存保护属性
	g_pmdlSystemCall = MmCreateMdl(NULL,
								  KeServiceDescriptorTable.ServiceTableBase,
								  KeServiceDescriptorTable.NumberOfServices * 4);

	//g_pmdlSystemCall存储的就是SSDT地址了
	if(!g_pmdlSystemCall)
		return STATUS_UNSUCCESSFUL;

	//从【不分页】的内存中映射SSDT
	MmBuildMdlForNonPagedPool(g_pmdlSystemCall);

	//修改内存保护属性

	g_pmdlSystemCall->MdlFlags = g_pmdlSystemCall->MdlFlags | MDL_MAPPED_TO_SYSTEM_VA;

	MappedSystemCallTable = MmMapLockedPages(g_pmdlSystemCall, KernelMode);
	
	/* 修改完毕  */


	/* 开始 HOOK，直接CALL宏，没问题 */
	HOOK_SYSCALL(ZwLoadDriver,NewZwLoadDriver,OldZwLoadDriver);
	/* Hook结束 */
	return ntStatus;
}