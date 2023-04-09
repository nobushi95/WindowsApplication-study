using Shared;
using System.Windows;

namespace EventSetterWPFNative
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        public MainWindow()
        {
            InitializeComponent();
        }

        private void ShowButton_Click(object sender, RoutedEventArgs e)
        {
            var handle = NativeMethods.OpenEvent(NativeMethods.EVENT_MODIFY_STATE, false, Constatns.ShowEventName);
            NativeMethods.SetEvent(handle);
            NativeMethods.CloseHandle(handle);
        }

        private void HideButton_Click(object sender, RoutedEventArgs e)
        {
            var handle = NativeMethods.OpenEvent(NativeMethods.EVENT_MODIFY_STATE, false, Constatns.HideEventName);
            NativeMethods.SetEvent(handle);
            NativeMethods.CloseHandle(handle);
        }
    }
}
