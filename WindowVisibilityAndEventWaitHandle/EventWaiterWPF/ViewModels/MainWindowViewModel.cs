using CommunityToolkit.Mvvm.ComponentModel;

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

        }
    }
}
