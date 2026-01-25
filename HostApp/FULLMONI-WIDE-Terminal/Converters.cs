using System.Globalization;
using System.Windows;
using System.Windows.Data;
using System.Windows.Media;

namespace FullmoniTerminal;

/// <summary>
/// bool を反転するコンバーター
/// </summary>
public class InverseBoolConverter : IValueConverter
{
    public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
    {
        if (value is bool b)
            return !b;
        return false;
    }

    public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
    {
        if (value is bool b)
            return !b;
        return false;
    }
}

/// <summary>
/// null かどうかで Visibility を切り替えるコンバーター
/// null でなければ Visible、null なら Collapsed
/// </summary>
public class NullToVisibilityConverter : IValueConverter
{
    public object Convert(object? value, Type targetType, object parameter, CultureInfo culture)
    {
        return value != null ? Visibility.Visible : Visibility.Collapsed;
    }

    public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
    {
        throw new NotSupportedException();
    }
}

/// <summary>
/// null かどうかで bool を返すコンバーター
/// null でなければ true、null なら false
/// </summary>
public class NullToBoolConverter : IValueConverter
{
    public object Convert(object? value, Type targetType, object parameter, CultureInfo culture)
    {
        return value != null;
    }

    public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
    {
        throw new NotSupportedException();
    }
}

/// <summary>
/// bool を BorderBrush に変換するコンバーター
/// true なら アクセントカラー、false なら 透明
/// </summary>
public class BoolToBorderBrushConverter : IValueConverter
{
    private static readonly SolidColorBrush SelectedBrush = new(Color.FromRgb(0x08, 0x91, 0xB2)); // Cyan-600
    private static readonly SolidColorBrush UnselectedBrush = new(Colors.Transparent);

    public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
    {
        if (value is bool b && b)
            return SelectedBrush;
        return UnselectedBrush;
    }

    public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
    {
        throw new NotSupportedException();
    }
}
