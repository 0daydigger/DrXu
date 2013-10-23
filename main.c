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

//ע��Symbolic LinkҪ�õĲ���

const WCHAR deviceLinkBuffer[] = L"\\DosDevices\\Drxu";
const WCHAR deviceNameBuffer[] = L"\\Device\\Drxu";
PDEVICE_OBJECT g_DrXuDevice;

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
	//��������ʼ
	NTSTATUS ntStatus;
	UNICODE_STRING deviceNameUnicodeString;
	UNICODE_STRING deviceLinkUnicodeString;
	//����������
	theDriverObject->DriverUnload = OnUnload;
	DbgPrint("Dr.Xu's Gentlmen sword on loading...");
	/* Regedit Symbolic Link */
	RtlInitUnicodeString(&deviceNameUnicodeString,
							deviceNameBuffer);
	RtlInitUnicodeString(&deviceLinkUnicodeString,
							deviceLinkBUffer);
	ntStatus = IoCreateDevice(theDriverObject,0,
						      &deviceNameUnicodeString,
							  FILE_DEVICE_ROOTKIT,
							  0,
							  TRUE,
							  &g_DrXuDevice);
	if( NT_SUCCESS(ntStatus) )
	{
		ntStatus = IoCreateSymbolicLink(&deviceLinkUnicodeString,
									    &deviceNameUnicodeString);
	}
	
	
	
	/* Regedit over */
	
	
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


	/* ��ʼ HOOK��ֱ��CALL�꣬û���� */
	HOOK_SYSCALL(ZwLoadDriver,NewZwLoadDriver,OldZwLoadDriver);
	/* Hook���� */
	return ntStatus;
}