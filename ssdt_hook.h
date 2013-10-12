#ifndef _ntddk_
#include <ntddk.h>
#define _ntddk_
#endif


//对齐方式，不改你就死定了。
#pragma pack(1)

/*
系统服务描述表的数据结构
*/
typedef struct ServiceDescriptorEntry{
	unsigned int *ServiceTableBase;
	unsigned int *ServiceCounterTableBase;
	unsigned int NumberOfServices;
	unsigned char *ParamTableBase;
}SSDT_Entry;

#pragma pack()


/* 找到ntoskrnl.exe对应的输出函数并返回ssdt中对应的FUNCTION，输入应该是Zw*,输出是Nt*  */
#define SYSTEMSERVICE(_func) \
	KeServiceDescriptorTable.ServiceTableBase[ *(PULONG) ((PUCHAR)_func+1)]

/* 输入Zw* FUNCTION，返回在SSDT中对应的编号 */
#define SYSCALL_INDEX(_Function) *(PULONG)((PUCHAR)_Function+1)

/* HOOK系统调用：

参数说明：
第一个参数：原函数所在地址
第二个参数：新函数（Hook的函数）所在【地址】，命名规则：New+第一个参数名称
第三个参数：用于保存原函数的地址的变量		  命名规则：Old+第一个参数名称

*/

#define HOOK_SYSCALL(_Function,_Hook,_Orig) \
	_Orig = (PVOID) InterlockedExchange( (PLONG) \
	&MappedSystemCallTable[SYSCALL_INDEX(_Function)], (LONG) _Hook)

/* 恢复系统调用 
参数说明：
第一个参数：原函数所在地址
第二个参数：新函数（Hook的函数）所在【地址】，命名规则：New+第一个参数名称
第三个参数：用于保存原函数的地址的变量		  命名规则：Old+第一个参数名称
*/
#define UNHOOK_SYSCALL(_Func,_Hook,_Orig) \
	InterlockedExchange((PLONG) \
	&MappedSystemCallTable[SYSCALL_INDEX(_Func)],(LONG) _Hook)


typedef NTSTATUS (__stdcall *ZWLOADDRIVER)( IN PUNICODE_STRING DriverServiceName);

/* 

Hook定义区
命名规则：New+函数原入口
*/

/*
函数说明：
DriverServiceName [in]
Pointer to a counted Unicode string that specifies a path to the driver's registry key, 
\Registry\Machine\System\CurrentControlSet\Services\DriverName,
where DriverName is the name of the driver.
*/
NTSTATUS NewZwLoadDriver(IN PUNICODE_STRING DriverServiceName);





/*定义区结束*/
/*
这些在ntdll里面已经有定义了，放在这里是为了方便看。
懒得查手册，懒！
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



