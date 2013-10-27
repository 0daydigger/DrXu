/* IoControl调度区 */
//调度宏
//定义设备名称
#define FILE_DEVICE_DRXU 0x19930410
// IoDeviceControl消息
#define IOCTL_TEST CTL_CODE(FILE_DEVICE_DRXU, 0x800, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_DRXU_TEST_READ CTL_CODE(FILE_DEVICE_DRXU, 0x801, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_DRXU_TEST_WRITE CTL_CODE(FILE_DEVICE_DRXU, 0x802, METHOD_BUFFERED, FILE_ANY_ACCESS)
//传递NtLoadDriver事件
#define IOCTL_DRXU_OPEN_EVENT_NtLoadDriver CTL_CODE(FILE_DEVICE_DRXU, 0x803, METHOD_BUFFERED, FILE_ANY_ACCESS)

//创建时负责接管IRP调度的函数，We don't care
NTSTATUS OnStubDispatch(IN PDEVICE_OBJECT DeviceObject,IN PIRP Irp );
//通信函数，必须care了这个，主要负责通信调度
NTSTATUS DrXuIoDispatch(IN PDEVICE_OBJECT DeviceObject,IN PIRP Irp );

//负责传递NtLoadDriver Call事件的事件句柄
UNICODE_STRING	   EventNtLoadDriverName;
//应用层对应句柄,SharedEventHandle
HANDLE		EventNtLoadDriverNameHandleFromApp;
//SharedEvent
extern PKEVENT		NtLoadDriverEvent;