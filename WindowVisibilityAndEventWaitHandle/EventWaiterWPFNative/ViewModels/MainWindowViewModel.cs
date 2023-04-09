using CommunityToolkit.Mvvm.ComponentModel;

namespace EventWaiterWPFNative.ViewModels
{
    internal partial class MainWindowViewModel : ObservableObject
    {
        [ObservableProperty]
        private string _receivedEventName = "Nothing";

        [ObservableProperty]
        private bool _isWindowVisible;
    }
}
