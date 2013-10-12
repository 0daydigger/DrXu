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
/*
��Щ��ntdll�����Ѿ��ж����ˣ�����������Ϊ�˷��㿴��
���ò��ֲᣬ����
typedef struct _MDL{
	struct _MDL *Next;
	CSHORT Size;
	CSHORT MdlFlags;
	struct _EPROCESS *Process;
	PVOID MappedSystemVa;
	PVOID StartVa;
	ULONG ByteCount;
	ULONG ByteOffset;
}MDL,*PMDL; 
//MDL FLAGS
#define MDL_MAPPED_TO_SYSTEM_VA		 0x0001
#define MDL_PAGES_LOCKED			 0x0002
#define MDL_SOURCE_IS_NONPAGED_POOL	 0x0004
#define MDL_ALLOCATED_FIXED_SIZE	 0x0008
#define MDL_PARTIAL					 0x0010
#define MDL_PARTIAL_HAS_BEEN_MAPPED	 0x0020
#define MDL_IO_PAGE_READ			 0x0040
#define MDL_WRITE_OPERATION			 0x0080
#define MDL_PARENT_MAPPED_SYSTEM_VA  0x0100
#define MDL_LOCK_HELD				 0x0200
#define MDL_PHYSICAL_VIEW			 0x0400
#define MDL_IO_SPACE				 0x0800
#define MDL_NETWORK_HEADER			 0x1000
#define MDL_MAPPING_CAN_FAIL		 0x2000
#define MDL_ALLOCATED_MUST_SUCCEED	 0x4000
*/



