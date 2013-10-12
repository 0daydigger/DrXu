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
��������������6
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
	PKEY_VALUE_FULL_INFORMATION pvfi;

	//STEP1: ��ȡ��������Ӧ��ע����ֵ�����������Ϣ
	//Ansi�ַ�����ʼ��,strDriverRegPath��ʾע����strDriverFilePath��ʾ�����ļ�·��
	RtlInitAnsiString(&strDriverRegPath,"");
	RtlInitAnsiString(&strDriverFilePath,"");
	//��ѯ��Ӧ��������ָ���������ļ��� ImagePathΪ��Ӧ��·����������strDriverFilePath��
	//�����ʼ��ֵ�������д������ֻ��д��ImagePath�����Ժ����ǲ�ѯIndex

	//����һ����L������������
	//RtlInitUnicodeString(&ustrKeyName,L"ImagePath");

	DbgPrint("Dr.Xu's gentlmen sword:ZwLoadDriver called.\n");
	/*																//�ò���ΪTRUEҪ�ǵ��ͷţ�
	RtlUnicodeStringToAnsiString(&strDriverRegPath,DriverServiceName,TRUE);
	DbgPrint("The DriverServiceName is %s",strDriverRegPath.Buffer);

	
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
	DbgPrint("[NewZwLoadDriver] ZwOpenKey %wZ Successfully",DriverServiceName);
	*/
	/* ��������˼·�ǣ�
	��1��Queryһ�£���ȡ�ü��ĳ��ȣ���Ϊ��������KEY��������0����ֻ��һ���
	��2�����ݳ��ȣ��ٳ�ʼ��������ü���Ϣ��pif��Ȼ�������ѯ
	���ǣ�������������Ϊ�ٲ鷽����С����
	*/

	//��ȡ����,ulSize�ﱣ����ǳ��ȣ������ڴ�
	//status = 
	/*
	status = ZwQueryValueKey(hRegister,&ustrKeyName,KeyValueFullInformation,NULL,0,&ulSize);
	if( !NT_SUCCESS(status) )
	{
		DbgPrint("[NewZwLoadDriver] ZwQueryValueKey %wZ Faild - Get Size",&ustrKeyName);
	}
	
	
	pvfi =(PKEY_VALUE_FULL_INFORMATION)ExAllocatePool(PagedPool,ulSize);

	status = ZwQueryValueKey(hRegister,&ustrKeyName,KeyValueFullInformation,pvfi,ulSize,&ulSize);
	if( !NT_SUCCESS(status) )
	{
		DbgPrint("[NewZwLoadDriver] ZwQueryValueKey %wZ Faild - Get Path",&ustrKeyName);
	}
	//��ע�⣡���ﴫ�ݸ�R3��ʱ��Ӧ�øĳ�ANSI��ʽ�ģ���
	DbgPrint(("[NewZwLoadDriver] The Driver File Path : %wZ.\n", &ustrKeyName));
	*/

	//�ͷ��ڴ�
	//ExFreePool(pvfi);
	
	RtlFreeAnsiString(&strDriverFilePath);
	RtlFreeAnsiString(&strDriverRegPath);
	//RtlFreeUnicodeString(&ustrKeyName);
	ntStatus = ( (ZWLOADDRIVER)(OldZwLoadDriver) )(DriverServiceName);
	return ntStatus;
}