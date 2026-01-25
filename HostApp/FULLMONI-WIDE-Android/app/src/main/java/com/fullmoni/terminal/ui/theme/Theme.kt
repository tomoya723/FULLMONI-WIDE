package com.fullmoni.terminal.ui.theme

import android.os.Build
import androidx.compose.foundation.isSystemInDarkTheme
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.darkColorScheme
import androidx.compose.material3.dynamicDarkColorScheme
import androidx.compose.material3.dynamicLightColorScheme
import androidx.compose.material3.lightColorScheme
import androidx.compose.runtime.Composable
import androidx.compose.ui.platform.LocalContext

private val DarkColorScheme = darkColorScheme(
    primary = FullmoniPrimary,
    onPrimary = TextPrimary,
    primaryContainer = FullmoniPrimaryDark,
    onPrimaryContainer = TextPrimary,
    secondary = FullmoniAccent,
    onSecondary = TextPrimary,
    secondaryContainer = CardBackground,
    onSecondaryContainer = TextPrimary,
    tertiary = FullmoniPrimaryLight,
    onTertiary = TextPrimary,
    background = BackgroundDark,
    onBackground = TextPrimary,
    surface = BackgroundMedium,
    onSurface = TextPrimary,
    surfaceVariant = CardBackground,
    onSurfaceVariant = TextSecondary,
    outline = TextMuted
)

private val LightColorScheme = lightColorScheme(
    primary = FullmoniPrimary,
    onPrimary = TextPrimary,
    primaryContainer = FullmoniPrimaryLight,
    onPrimaryContainer = BackgroundDark,
    secondary = FullmoniAccent,
    onSecondary = TextPrimary,
    background = androidx.compose.ui.graphics.Color(0xFFF5F5F5),
    onBackground = BackgroundDark,
    surface = androidx.compose.ui.graphics.Color.White,
    onSurface = BackgroundDark
)

@Composable
fun FullmoniTheme(
    darkTheme: Boolean = true, // Default to dark theme like Windows app
    dynamicColor: Boolean = false,
    content: @Composable () -> Unit
) {
    val colorScheme = when {
        dynamicColor && Build.VERSION.SDK_INT >= Build.VERSION_CODES.S -> {
            val context = LocalContext.current
            if (darkTheme) dynamicDarkColorScheme(context) else dynamicLightColorScheme(context)
        }
        darkTheme -> DarkColorScheme
        else -> LightColorScheme
    }

    MaterialTheme(
        colorScheme = colorScheme,
        typography = Typography,
        content = content
    )
}
