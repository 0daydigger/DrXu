#ifndef _ntddk_
#include <ntddk.h>
#define _ntddk_
#endif

#ifndef _ssdt_hook_
#define _ssdt_hook_
#include <ssdt_hook.h>
#endif
/* 在这里声明HOOK的函数 */
extern ZWLOADDRIVER OldZwLoadDriver;
//新LoadDriver入口
#define BUFFER_SIZE 1024
NTSTATUS NewZwLoadDriver(IN PUNICODE_STRING DriverServiceName)
{
	NTSTATUS ntStatus;

	ANSI_STRING strDriverRegPath;

	//Initalize ANSI_STRING

	RtlInitAnsiString(&strDriverRegPath,"");


	/*
	strDriverRegPath.MaximumLength = BUFFER_SIZE;
	strDriverRegPath.Buffer = (PCHAR)ExAllocatePool(NonPagedPool, BUFFER_SIZE);
	strDriverRegPath.Length = ( (DriverServiceName->Length) / 2 );
	*/


	DbgPrint("Dr.Xu's gentlmen sword:ZwLoadDriver called.\n");

	RtlUnicodeStringToAnsiString(&strDriverRegPath,DriverServiceName,TRUE);

	DbgPrint(strDriverRegPath.Buffer);

	// 自动释放，崩内核了？
	//RtlFreeAnsiString(&strDriverRegPath);

	//	RtlFreeUnicodeString(&strDriverRegPath);



	ntStatus = ( (ZWLOADDRIVER)(OldZwLoadDriver) )(DriverServiceName);
	return ntStatus;
}