#ifndef _ntddk_
#include <ntddk.h>
#define _ntddk_
#endif

#ifndef _ssdt_hook_
#define _ssdt_hook_
#include <ssdt_hook.h>
#endif
/* ����������HOOK�ĺ��� */
extern ZWLOADDRIVER OldZwLoadDriver;
//��LoadDriver���
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

	// �Զ��ͷţ����ں��ˣ�
	//RtlFreeAnsiString(&strDriverRegPath);

	//	RtlFreeUnicodeString(&strDriverRegPath);



	ntStatus = ( (ZWLOADDRIVER)(OldZwLoadDriver) )(DriverServiceName);
	return ntStatus;
}