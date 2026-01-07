using System.ComponentModel;
using System.Windows;
using System.Windows.Controls;

namespace FullmoniTerminal;

/// <summary>
/// Interaction logic for MainWindow.xaml
/// </summary>
public partial class MainWindow : Window
{
    public MainWindow()
    {
        InitializeComponent();
    }

    private void Window_Closing(object sender, CancelEventArgs e)
    {
        if (DataContext is ViewModels.MainViewModel vm)
        {
            // ファームウェア更新中は閉じられないようにする
            if (vm.IsFirmwareUpdating)
            {
                e.Cancel = true;
                MessageBox.Show(
                    this,
                    "ファームウェア更新中はアプリケーションを終了できません。\n更新が完了するまでお待ちください。",
                    "警告",
                    MessageBoxButton.OK,
                    MessageBoxImage.Warning);
                return;
            }
            vm.Dispose();
        }
    }

    private void FirmwareLogTextBox_TextChanged(object sender, TextChangedEventArgs e)
    {
        if (sender is TextBox textBox)
        {
            textBox.ScrollToEnd();
        }
    }
}
