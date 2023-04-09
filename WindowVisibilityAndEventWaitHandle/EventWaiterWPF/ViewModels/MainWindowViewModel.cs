using CommunityToolkit.Mvvm.ComponentModel;
using Shared;

namespace EventWaiterWPF.ViewModels
{
    internal partial class MainWindowViewModel : ObservableObject
    {
        [ObservableProperty]
        private string _receivedEventName = "Nothing";

        [ObservableProperty]
        private bool _isWindowVisible = true;

        public MainWindowViewModel()
        {
            Task.Run(EventWait);
        }

        private void EventWait()
        {
            var created = Enumerable.Repeat(true, 3).ToArray();
            var events = new[]
            {
                new EventWaitHandle(false, EventResetMode.ManualReset, EventNames.Show, out created[0]),
                new EventWaitHandle(false, EventResetMode.ManualReset, EventNames.Hide, out created[1]),
                new EventWaitHandle(false, EventResetMode.ManualReset, EventNames.Close, out created[2]),
            };

            if (created.Any(x => !x))
            {
                Array.ForEach(events, x => x.Dispose());
                return;
            }

            var isLoop = true;
            while (isLoop)
            {
                try
                {
                    var signaledIdx = WaitHandle.WaitAny(events);
                    switch (signaledIdx)
                    {
                        // Showイベント
                        case 0:
                            ReceivedEventName = "Show";
                            IsWindowVisible = true;
                            events[0].Reset();
                            break;

                        // Hideイベント
                        case 1:
                            ReceivedEventName = "Hide";
                            IsWindowVisible = false;
                            events[1].Reset();
                            break;

                        // Closeイベント
                        case 2:
                            ReceivedEventName = "Close";
                            isLoop = false;
                            events[2].Reset();
                            break;

                        default:
                            isLoop = false;
                            break;
                    }
                }
                catch
                {
                    isLoop = false;
                }
            }

            Array.ForEach(events, x => x.Dispose());
        }
    }
}
