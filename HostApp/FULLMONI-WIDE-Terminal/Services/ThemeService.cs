using System.Windows;

namespace FullmoniTerminal.Services;

/// <summary>
/// テーマ切り替えサービス
/// </summary>
public class ThemeService
{
    private static ThemeService? _instance;
    public static ThemeService Instance => _instance ??= new ThemeService();

    private bool _isDarkTheme;

    public bool IsDarkTheme
    {
        get => _isDarkTheme;
        set
        {
            if (_isDarkTheme != value)
            {
                _isDarkTheme = value;
                ApplyTheme();
                ThemeChanged?.Invoke(this, value);
            }
        }
    }

    public event EventHandler<bool>? ThemeChanged;

    private ThemeService()
    {
        _isDarkTheme = false;
    }

    public void ApplyTheme()
    {
        var app = Application.Current;
        if (app == null) return;

        var mergedDicts = app.Resources.MergedDictionaries;

        // 既存のテーマを削除
        var existingTheme = mergedDicts.FirstOrDefault(d =>
            d.Source?.OriginalString.Contains("Theme.xaml") == true);
        if (existingTheme != null)
        {
            mergedDicts.Remove(existingTheme);
        }

        // 新しいテーマを適用
        var themePath = _isDarkTheme
            ? "Themes/DarkPopTheme.xaml"
            : "Themes/PopTheme.xaml";

        try
        {
            var newTheme = new ResourceDictionary
            {
                Source = new Uri(themePath, UriKind.Relative)
            };
            mergedDicts.Add(newTheme);
        }
        catch
        {
            // フォールバック: PopThemeを使用
            var fallback = new ResourceDictionary
            {
                Source = new Uri("Themes/PopTheme.xaml", UriKind.Relative)
            };
            mergedDicts.Add(fallback);
        }
    }

    public void ToggleTheme()
    {
        IsDarkTheme = !IsDarkTheme;
    }
}
