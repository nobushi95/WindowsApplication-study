using CommunityToolkit.Mvvm.ComponentModel;
using Shared;

namespace EventWaiterWPFNative.ViewModels
{
    internal partial class MainWindowViewModel : ObservableObject
    {
        [ObservableProperty]
        private string _receivedEventName = "Nothing";

        [ObservableProperty]
        private bool _isWindowVisible = true;

        public MainWindowViewModel()
        {
            // 投げっぱなしタスク
            Task.Run(EventWait);
        }

        private void EventWait()
        {
            var events = new IntPtr[]
            {
                NativeMethods.CreateEvent(IntPtr.Zero, true, false, EventNames.Show),
                NativeMethods.CreateEvent(IntPtr.Zero, true, false, EventNames.Hide),
                NativeMethods.CreateEvent(IntPtr.Zero, true, false, EventNames.Close),
            };

            if (events.Any(x => x == IntPtr.Zero))
            {
                CloseEventHandles();
                return;
            }

            var isLoop = true;
            while (isLoop)
            {
                var result = NativeMethods.WaitForMultipleObjects((uint)events.Length, events, false, NativeMethods.INFINITE);

                switch (result)
                {
                    // Showイベント
                    case NativeMethods.WAIT_OBJECT_0:
                        ReceivedEventName = "Show";
                        // Windowを非アクティブにしてCollapsed->Showにすると非アクティブのままだが、
                        // もう一度非アクティブのままCoolaped->Showにするとアクティブになる？
                        IsWindowVisible = true;
                        NativeMethods.ResetEvent(events[0]);
                        break;

                    // Hideイベント
                    case NativeMethods.WAIT_OBJECT_0 + 1:
                        ReceivedEventName = "Hide";
                        IsWindowVisible = false;
                        NativeMethods.ResetEvent(events[1]);
                        break;

                    // Closeイベント
                    case NativeMethods.WAIT_OBJECT_0 + 2:
                        ReceivedEventName = "Close";
                        isLoop = false;
                        NativeMethods.ResetEvent(events[2]);
                        // TODO: WindowをCloseする(ループ抜けた後かも検討する)
                        break;

                    case NativeMethods.WAIT_FAILED:
                        isLoop = false;
                        break;
                }
            }

            CloseEventHandles();

            void CloseEventHandles()
            {
                events.Where(x => x != IntPtr.Zero).Select(x =>
                {
                    NativeMethods.CloseHandle(x);
                    return true;
                });
            }
        }
    }
}
