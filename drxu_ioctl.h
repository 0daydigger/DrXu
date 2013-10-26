/* IoControl调度区 */
//调度宏
//定义设备名称
#define FILE_DEVICE_DRXU 0x19930410
//At the beginning there's FILE_DEVICE_UNKNOWN
#define IOCTL_TEST CTL_CODE(FILE_DEVICE_DRXU, 0x800, METHOD_BUFFERED, FILE_ANY_ACCESS)

			

NTSTATUS OnStubDispatch(IN PDEVICE_OBJECT DeviceObject,IN PIRP Irp );
NTSTATUS DrXuIoDispatch(IN PDEVICE_OBJECT DeviceObject,IN PIRP Irp );
