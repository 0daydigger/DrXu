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
//����������ҪHOOK�ĺ���
ZWLOADDRIVER OldZwLoadDriver;

//����ж�ػص�
VOID OnUnload(IN PDRIVER_OBJECT DriverObject)
{
	DbgPrint("Dr.Xu's Gentlmen Sword:Quiting...");


	//�ѹ��ӣ����б����ĺ�����Ҫ������ָ�
	UNHOOK_SYSCALL(ZwLoadDriver,OldZwLoadDriver,NewZwLoadDriver);



	//���֮ǰȡ�ù�SSDT��ַ����ô����Ҫ��ӳ��ȡ��
	if(g_pmdlSystemCall)
	{
		MmUnmapLockedPages(MappedSystemCallTable,g_pmdlSystemCall);
		IoFreeMdl(g_pmdlSystemCall);
	}
}

NTSTATUS DriverEntry(IN PDRIVER_OBJECT theDriverObject,
					 IN PUNICODE_STRING theRegistryPath)
{
	//�������
	
	//ע��ж������
	theDriverObject->DriverUnload = OnUnload;
	DbgPrint("Dr.Xu's Gentlmen sword on loading...");

	
	/* �޸�SSDT���ڴ�����  */
	//��SSDT��ȡԭ�����ڴ��ַ
	OldZwLoadDriver = (ZWLOADDRIVER)(SYSTEMSERVICE(ZwLoadDriver));

	//�޸��ڴ汣������
	g_pmdlSystemCall = MmCreateMdl(NULL,
								  KeServiceDescriptorTable.ServiceTableBase,
								  KeServiceDescriptorTable.NumberOfServices * 4);

	//g_pmdlSystemCall�洢�ľ���SSDT��ַ��
	if(!g_pmdlSystemCall)
		return STATUS_UNSUCCESSFUL;

	//�ӡ�����ҳ�����ڴ���ӳ��SSDT
	MmBuildMdlForNonPagedPool(g_pmdlSystemCall);

	//�޸��ڴ汣������

	g_pmdlSystemCall->MdlFlags = g_pmdlSystemCall->MdlFlags | MDL_MAPPED_TO_SYSTEM_VA;

	MappedSystemCallTable = MmMapLockedPages(g_pmdlSystemCall, KernelMode);
	
	/* �޸����  */


	//HOOK
	HOOK_SYSCALL(ZwLoadDriver,NewZwLoadDriver,OldZwLoadDriver);

	return STATUS_SUCCESS;
}