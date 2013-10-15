#ifndef _ntddk_
#include <ntddk.h>
#define _ntddk_
#endif

#ifndef _ssdt_hook_
#define _ssdt_hook_
#include <ssdt_hook.h>
#endif
/* ����������HOOK�ĺ�������extern���Σ���������������main.c�� */
extern ZWLOADDRIVER OldZwLoadDriver;

#define BUFFER_SIZE 1024

//��LoadDriver���
/*
��������������16
�����Ų�ԭ��
ReleaseANSIString��ReleaseUnicodeString��ϵͳ�С�δ�ء���û�ͷ�
����֮������Ѿ��ͷ�����Release���Ⱥȣ���ϲ�㣬�����ˡ�
*/
NTSTATUS NewZwLoadDriver(IN PUNICODE_STRING DriverServiceName)
{
	NTSTATUS ntStatus;
	ANSI_STRING strDriverRegPath;
	ANSI_STRING strDriverFilePath;
	OBJECT_ATTRIBUTES objQueryDriverFilePath;
	UNICODE_STRING ustrKeyName;
	ULONG ulSize = 0;
	HANDLE hRegister;
	NTSTATUS status;

	KEY_VALUE_PARTIAL_INFORMATION try_Key_Value_Info;
	PKEY_VALUE_PARTIAL_INFORMATION ac_Key_Value_Info;
	
	/* �����ñ�����
	PKEY_FULL_INFORMATION pfi;
	int i;
	PKEY_BASIC_INFORMATION pbi;
	*/

	//STEP1: ��ȡ��������Ӧ��ע����ֵ�����������Ϣ
	//Ansi�ַ�����ʼ��,strDriverRegPath��ʾע����strDriverFilePath��ʾ�����ļ�·��
	RtlInitAnsiString(&strDriverRegPath,"");
	RtlInitAnsiString(&strDriverFilePath,"");
	//��ѯ��Ӧ��������ָ���������ļ��� ImagePathΪ��Ӧ��·����������strDriverFilePath��
	//�����ʼ��ֵ�������д������ֻ��д��ImagePath�����Ժ����ǲ�ѯIndex

	//����һ����L������������
	RtlInitUnicodeString(&ustrKeyName,L"ImagePath");

	DbgPrint("Dr.Xu's gentlmen sword:ZwLoadDriver called.\n");
																	//�ò���ΪTRUEҪ�ǵ��ͷţ����˴��ͷſɵ���������
	RtlUnicodeStringToAnsiString(&strDriverRegPath,DriverServiceName,TRUE);
	DbgPrint("The DriverServiceName is %s",strDriverRegPath.Buffer);

<<<<<<< HEAD
	DbgPrint(strDriverRegPath.Buffer);

	// �Զ��ͷţ����ں��ˣ�
	//RtlFreeAnsiString(&strDriverRegPath);

	//	RtlFreeUnicodeString(&strDriverRegPath);


=======
	
    InitializeObjectAttributes(&objQueryDriverFilePath, //OBJECT_ATTRIBUTES����
                            DriverServiceName,  //������Ӧ��ע�����(Reg String);
                            OBJ_CASE_INSENSITIVE, //��Сд������
                            NULL, //DriverServiceName����ȫ�������������NULL
                            NULL); //Ĭ�ϰ�ȫ����
    status = ZwOpenKey(&hRegister,  //����򿪺�ע���ľ��
									   KEY_ALL_ACCESS, //ȫ����Ȩ��
									   &objQueryDriverFilePath); //��Ӧ�ļ����������OBJECT_ATTRIBUTES
	if ( !NT_SUCCESS(status) )
	{
		DbgPrint("[NewZwLoadDriver] ZwOpenKey %wZ Faild",DriverServiceName);
	}
	else
	{
		DbgPrint("[NewZwLoadDriver] ZwOpenKey %wZ Successfully",DriverServiceName);
	}
	//��ȡ����,ulSize�ﱣ����ǳ��ȣ������ڴ�
>>>>>>> getServiceFileName

	status = ZwQueryValueKey(hRegister,&ustrKeyName,KeyValuePartialInformation,NULL,0,&ulSize);
	if( status != STATUS_SUCCESS )
	{
		DbgPrint("[NewZwLoadDriver] ZwQueryValueKey %wZ Faild - Get Size",&ustrKeyName);
	}
	else
	{
		DbgPrint("[NewZwLoadDriver] ZwQueryValueKey %wZ Sucessfull - Get Size",&ustrKeyName);
	}
	
	ac_Key_Value_Info =ExAllocatePoolWithTag(NonPagedPool,ulSize,"tag1");
	if ( ac_Key_Value_Info != NULL )
	{
			status = ZwQueryValueKey(hRegister,&ustrKeyName,KeyValuePartialInformation,ac_Key_Value_Info,ulSize,&ulSize);
			if( status != STATUS_SUCCESS  )
			{
				DbgPrint("[NewZwLoadDriver] ZwQueryValueKey %wZ Faild - Get Path",&ustrKeyName);
			}
			else
			{
				DbgPrint("[NewZwLoadDriver] ZwQueryValueKey %wZ Successful - Get Path",&ustrKeyName);
			}
			ustrKeyName.Length = (ac_Key_Value_Info->DataLength );
			ustrKeyName.Buffer = (ac_Key_Value_Info->Data );
			DbgPrint("[NewZwLoadDriver] The Driver File Path : %wZ.\n",&ustrKeyName);
			ExFreePool(ac_Key_Value_Info);
	}	
	
	ntStatus = ( (ZWLOADDRIVER)(OldZwLoadDriver) )(DriverServiceName);
	return ntStatus;
}