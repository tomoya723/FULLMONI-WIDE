using System.Windows.Controls;

namespace FullmoniTerminal.Views;

/// <summary>
/// Interaction logic for StartupImageView.xaml
/// </summary>
public partial class StartupImageView : UserControl
{
    public StartupImageView()
    {
        InitializeComponent();
    }
    
    private void LogTextBox_TextChanged(object sender, TextChangedEventArgs e)
    {
        if (sender is TextBox textBox)
        {
            textBox.ScrollToEnd();
        }
    }
}
