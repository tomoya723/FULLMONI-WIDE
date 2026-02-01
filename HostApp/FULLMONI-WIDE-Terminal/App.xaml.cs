using System.Windows;
using System.Windows.Threading;

namespace FullmoniTerminal;

/// <summary>
/// Interaction logic for App.xaml
/// </summary>
public partial class App : Application
{
    protected override void OnStartup(StartupEventArgs e)
    {
        base.OnStartup(e);

        // 未処理の例外をキャッチ
        DispatcherUnhandledException += App_DispatcherUnhandledException;
        AppDomain.CurrentDomain.UnhandledException += CurrentDomain_UnhandledException;
        TaskScheduler.UnobservedTaskException += TaskScheduler_UnobservedTaskException;
    }

    private void App_DispatcherUnhandledException(object sender, DispatcherUnhandledExceptionEventArgs e)
    {
        System.Diagnostics.Debug.WriteLine($"[UI Exception] {e.Exception}");
        MessageBox.Show($"UI Error: {e.Exception.Message}\n\n{e.Exception.StackTrace}", "Error", MessageBoxButton.OK, MessageBoxImage.Error);
        e.Handled = true;
    }

    private void CurrentDomain_UnhandledException(object sender, UnhandledExceptionEventArgs e)
    {
        var ex = e.ExceptionObject as Exception;
        System.Diagnostics.Debug.WriteLine($"[Domain Exception] {ex}");
        MessageBox.Show($"Domain Error: {ex?.Message}\n\n{ex?.StackTrace}", "Error", MessageBoxButton.OK, MessageBoxImage.Error);
    }

    private void TaskScheduler_UnobservedTaskException(object? sender, UnobservedTaskExceptionEventArgs e)
    {
        System.Diagnostics.Debug.WriteLine($"[Task Exception] {e.Exception}");
        e.SetObserved();
    }

    protected override void OnExit(ExitEventArgs e)
    {
        base.OnExit(e);
    }
}
