#ifndef _ntddk_
#include <ntddk.h>
#define _ntddk_
#endif

#ifndef _ssdt_hook_
#define _ssdt_hook_
#include <ssdt_hook.h>
#endif


__declspec(dllimport) SSDT_Entry KeServiceDescriptorTable;

PMDL g_pmdlSystemCall;
PVOID *MappedSystemCallTable;
//在这里声明要HOOK的函数
ZWLOADDRIVER OldZwLoadDriver;

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

NTSTATUS DriverEntry(IN PDRIVER_OBJECT theDriverObject,
					 IN PUNICODE_STRING theRegistryPath)
{
	//驱动入口
	
	//注册卸载例程
	theDriverObject->DriverUnload = OnUnload;
	DbgPrint("Dr.Xu's Gentlmen sword on loading...");

	
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


	//HOOK
	HOOK_SYSCALL(ZwLoadDriver,NewZwLoadDriver,OldZwLoadDriver);

	return STATUS_SUCCESS;
}