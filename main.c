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
//����������ҪHOOK�ĺ���
ZWLOADDRIVER OldZwLoadDriver;


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
	//��������ʼ
	NTSTATUS ntStatus;
	UNICODE_STRING deviceNameUnicodeString;
	UNICODE_STRING deviceLinkUnicodeString;
	int i;
	//����������
	/*  ������ʾ��Ϣ */
	theDriverObject->DriverUnload = OnUnload;
	DbgPrint("Dr.Xu's Gentlmen sword on loading...");
	
	/*  �ֹ�IRP���� 
		ע�⣺��������е��Ⱦ�д����һ��������ʵ��Ӧ��д�ɶ����
	*/
	for(i=0;i< IRP_MJ_MAXIMUM_FUNCTION; i++ )
	{
		theDriverObject->MajorFunction[i] = OnStubDispatch;
	}
	/* IRP���ȷֹܽ��� */
	

	/* Regedit Symbolic Link */
	RtlInitUnicodeString(&deviceNameUnicodeString,
							deviceNameBuffer);
	RtlInitUnicodeString(&deviceLinkUnicodeString,
							deviceLinkBuffer);
	//�����豸
	ntStatus = IoCreateDevice(theDriverObject,
							  0,
						      &deviceNameUnicodeString,
							  //defined in drxu_constant.h, value = 0x19930410
							  FILE_DEVICE_DRXU,
							  0,
							  TRUE,
							  &g_DrXuDevice);
	//�����豸�ɹ��Ļ�������symbolic link
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