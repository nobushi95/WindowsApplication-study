using System.Runtime.InteropServices;
using System.Security;

namespace Shared
{
    internal class NativeMethods
    {
        [DllImport("kernel32.dll")]
        public static extern uint WaitForMultipleObjects(uint nCount, IntPtr[] lpHandles,
   bool bWaitAll, uint dwMilliseconds);

        public static readonly uint INFINITE = 0xffffffff;
        // switch文で使用するのでconst宣言
        public const uint WAIT_OBJECT_0 = 0x00000000;
        public const uint WAIT_FAILED = 0xffffffff;

        [DllImport("kernel32.dll")]
        public static extern IntPtr CreateEvent(IntPtr lpEventAttributes, bool bManualReset, bool bInitialState, string lpName);

        [DllImport("Kernel32.dll", SetLastError = true)]
        public static extern IntPtr OpenEvent(uint dwDesiredAccess, bool bInheritHandle, string lpName);

        public static readonly uint EVENT_MODIFY_STATE = 0x0002;

        [DllImport("kernel32.dll")]
        public static extern bool SetEvent(IntPtr hEvent);

        [DllImport("kernel32.dll")]
        internal static extern bool ResetEvent(IntPtr hEvent);

        [DllImport("kernel32.dll", SetLastError = true)]
        [SuppressUnmanagedCodeSecurity]
        [return: MarshalAs(UnmanagedType.Bool)]
        public static extern bool CloseHandle(IntPtr hObject);
    }
}
