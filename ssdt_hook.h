#ifndef _ntddk_
#include <ntddk.h>
#define _ntddk_
#endif


//���뷽ʽ��������������ˡ�
#pragma pack(1)

/*
ϵͳ��������������ݽṹ
*/
typedef struct ServiceDescriptorEntry{
	unsigned int *ServiceTableBase;
	unsigned int *ServiceCounterTableBase;
	unsigned int NumberOfServices;
	unsigned char *ParamTableBase;
}SSDT_Entry;

#pragma pack()


/* �ҵ�ntoskrnl.exe��Ӧ���������������ssdt�ж�Ӧ��FUNCTION������Ӧ����Zw*,�����Nt*  */
#define SYSTEMSERVICE(_func) \
	KeServiceDescriptorTable.ServiceTableBase[ *(PULONG) ((PUCHAR)_func+1)]

/* ����Zw* FUNCTION��������SSDT�ж�Ӧ�ı�� */
#define SYSCALL_INDEX(_Function) *(PULONG)((PUCHAR)_Function+1)

/* HOOKϵͳ���ã�

����˵����
��һ��������ԭ�������ڵ�ַ
�ڶ����������º�����Hook�ĺ��������ڡ���ַ������������New+��һ����������
���������������ڱ���ԭ�����ĵ�ַ�ı���		  ��������Old+��һ����������

*/

#define HOOK_SYSCALL(_Function,_Hook,_Orig) \
	_Orig = (PVOID) InterlockedExchange( (PLONG) \
	&MappedSystemCallTable[SYSCALL_INDEX(_Function)], (LONG) _Hook)

/* �ָ�ϵͳ���� 
����˵����
��һ��������ԭ�������ڵ�ַ
�ڶ����������º�����Hook�ĺ��������ڡ���ַ������������New+��һ����������
���������������ڱ���ԭ�����ĵ�ַ�ı���		  ��������Old+��һ����������
*/
#define UNHOOK_SYSCALL(_Func,_Hook,_Orig) \
	InterlockedExchange((PLONG) \
	&MappedSystemCallTable[SYSCALL_INDEX(_Func)],(LONG) _Hook)


typedef NTSTATUS (__stdcall *ZWLOADDRIVER)( IN PUNICODE_STRING DriverServiceName);

/* 

Hook������
��������New+����ԭ���
*/

/*
����˵����
DriverServiceName [in]
Pointer to a counted Unicode string that specifies a path to the driver's registry key, 
\Registry\Machine\System\CurrentControlSet\Services\DriverName,
where DriverName is the name of the driver.
*/
NTSTATUS NewZwLoadDriver(IN PUNICODE_STRING DriverServiceName);





/*����������*/



