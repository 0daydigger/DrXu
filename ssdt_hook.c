#ifndef _ntddk_
#include <ntddk.h>
#define _ntddk_
#endif

#ifndef _ssdt_hook_
#define _ssdt_hook_
#include <ssdt_hook.h>
#endif
/* 在这里声明HOOK的函数，用extern修饰，真正函数变量在main.c里 */
extern ZWLOADDRIVER OldZwLoadDriver;

#define BUFFER_SIZE 1024

//新LoadDriver入口
/*
调试蓝屏次数：16
最终排查原因：
ReleaseANSIString和ReleaseUnicodeString在系统中【未必】就没释放
换言之，如果已经释放你又Release，喝喝，恭喜你，死定了。
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
	
	/* 测试用变量区
	PKEY_FULL_INFORMATION pfi;
	int i;
	PKEY_BASIC_INFORMATION pbi;
	*/

	//STEP1: 获取驱动所对应的注册表键值，输出调试信息
	//Ansi字符串初始化,strDriverRegPath表示注册表项，strDriverFilePath表示驱动文件路径
	RtlInitAnsiString(&strDriverRegPath,"");
	RtlInitAnsiString(&strDriverFilePath,"");
	//查询对应驱动名称指定的驱动文件。 ImagePath为对应的路径，保存在strDriverFilePath中
	//这个初始化值可以随便写，我们只是写成ImagePath方便以后我们查询Index

	//这里一定加L！不加蓝死你
	RtlInitUnicodeString(&ustrKeyName,L"ImagePath");

	DbgPrint("Dr.Xu's gentlmen sword:ZwLoadDriver called.\n");
																	//该参数为TRUE要记得释放！【此处释放可导致蓝屏】
	RtlUnicodeStringToAnsiString(&strDriverRegPath,DriverServiceName,TRUE);
	DbgPrint("The DriverServiceName is %s",strDriverRegPath.Buffer);

<<<<<<< HEAD
	DbgPrint(strDriverRegPath.Buffer);

	// 自动释放，崩内核了？
	//RtlFreeAnsiString(&strDriverRegPath);

	//	RtlFreeUnicodeString(&strDriverRegPath);


=======
	
    InitializeObjectAttributes(&objQueryDriverFilePath, //OBJECT_ATTRIBUTES变量
                            DriverServiceName,  //驱动对应的注册表项(Reg String);
                            OBJ_CASE_INSENSITIVE, //大小写不敏感
                            NULL, //DriverServiceName里是全名，因此这里是NULL
                            NULL); //默认安全级别
    status = ZwOpenKey(&hRegister,  //保存打开后注册表的句柄
									   KEY_ALL_ACCESS, //全访问权限
									   &objQueryDriverFilePath); //对应文件操作所需的OBJECT_ATTRIBUTES
	if ( !NT_SUCCESS(status) )
	{
		DbgPrint("[NewZwLoadDriver] ZwOpenKey %wZ Faild",DriverServiceName);
	}
	else
	{
		DbgPrint("[NewZwLoadDriver] ZwOpenKey %wZ Successfully",DriverServiceName);
	}
	//获取长度,ulSize里保存的是长度，分配内存
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