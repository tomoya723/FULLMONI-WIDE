using System.ComponentModel;
using System.Diagnostics;
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

    private void OpenGitHubLink_Click(object sender, RoutedEventArgs e)
    {
        OpenUrl("https://github.com/tomoya723/FULLMONI-WIDE");
    }

    private void OpenDocsLink_Click(object sender, RoutedEventArgs e)
    {
        OpenUrl("https://github.com/tomoya723/FULLMONI-WIDE/blob/main/Firmware/docs/PARAM_CONSOLE.md");
    }

    private void OpenIssuesLink_Click(object sender, RoutedEventArgs e)
    {
        OpenUrl("https://github.com/tomoya723/FULLMONI-WIDE/issues");
    }

    private void AboutScrollViewer_Loaded(object sender, RoutedEventArgs e)
    {
        if (sender is ScrollViewer scrollViewer)
        {
            scrollViewer.ScrollToTop();
        }
    }

    private static void OpenUrl(string url)
    {
        try
        {
            Process.Start(new ProcessStartInfo
            {
                FileName = url,
                UseShellExecute = true
            });
        }
        catch
        {
            // ブラウザで開けなかった場合は無視
        }
    }
}
