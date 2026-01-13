using System;
using System.IO;

namespace FullmoniTerminal.Services;

/// <summary>
/// BMP画像をSEGGER AppWizard/emWin形式に変換するコンバータ
///
/// AppWizard形式 (emWin GUI_BITMAP_STREAM):
/// - ヘッダ: 12バイト
///   - [0-1]: "BM" (0x42, 0x4D) - BMPマジック互換
///   - [2-3]: 圧縮タイプ (0x0008 = 非圧縮RGB565)
///   - [4-5]: 幅 (little-endian)
///   - [6-7]: 高さ (little-endian)
///   - [8-9]: stride (幅 * 2 bytes/pixel)
///   - [10-11]: bits per pixel (16 = RGB565)
/// - データ: 非圧縮のRGB565ピクセルデータ（上から下へ、左から右へ）
/// </summary>
public static class AppWizardImageConverter
{
    // AppWizardヘッダサイズ
    private const int HeaderSize = 12;

    // 出力サイズ (acmtc配列に合わせる: 765 × 256 × 2 + 12 = 391692、パディング含め391696)
    private const int OutputSize = 391696;

    // 期待する画像サイズ
    private const int ExpectedWidth = 765;
    private const int ExpectedHeight = 256;

    /// <summary>
    /// 24bit/32bit BMPをAppWizard形式（非圧縮RGB565）に変換
    /// </summary>
    /// <param name="bmpData">入力BMPデータ</param>
    /// <returns>AppWizard形式データ (391,696バイト固定)</returns>
    public static byte[] ConvertBmpToAppWizard(byte[] bmpData)
    {
        // BMPヘッダ検証
        if (bmpData.Length < 54)
            throw new ArgumentException("BMPデータが小さすぎます");

        if (bmpData[0] != 0x42 || bmpData[1] != 0x4D)
            throw new ArgumentException("BMP形式ではありません");

        // BMPヘッダから情報取得
        int width = BitConverter.ToInt32(bmpData, 18);
        int rawHeight = BitConverter.ToInt32(bmpData, 22);
        int height = Math.Abs(rawHeight);
        bool isTopDown = rawHeight < 0;
        int bitsPerPixel = BitConverter.ToInt16(bmpData, 28);
        int dataOffset = BitConverter.ToInt32(bmpData, 10);

        if (width != ExpectedWidth || height != ExpectedHeight)
            throw new ArgumentException($"画像サイズが不正です。期待値: {ExpectedWidth}×{ExpectedHeight}, 実際: {width}×{height}");

        if (bitsPerPixel != 24 && bitsPerPixel != 32)
            throw new ArgumentException($"24bit または 32bit BMPが必要です。実際: {bitsPerPixel}bit");

        // 出力バッファ（0で初期化）
        var output = new byte[OutputSize];

        // AppWizardヘッダ作成
        output[0] = 0x42;  // 'B'
        output[1] = 0x4D;  // 'M'
        output[2] = 0x08;  // 圧縮タイプ (低) = 0x0008
        output[3] = 0x00;  // 圧縮タイプ (高)
        output[4] = (byte)(width & 0xFF);         // 幅 (低) = 0xFD
        output[5] = (byte)((width >> 8) & 0xFF);  // 幅 (高) = 0x02  → 765
        output[6] = (byte)(height & 0xFF);        // 高さ (低) = 0x00
        output[7] = (byte)((height >> 8) & 0xFF); // 高さ (高) = 0x01  → 256
        int stride = width * 2;  // RGB565は2バイト/ピクセル = 1530
        output[8] = (byte)(stride & 0xFF);        // stride (低) = 0xFA
        output[9] = (byte)((stride >> 8) & 0xFF); // stride (高) = 0x05  → 1530
        output[10] = 0x10;  // 16 bits per pixel
        output[11] = 0x00;

        // BMPピクセルデータをRGB565に変換（非圧縮で出力）
        int bytesPerPixelIn = bitsPerPixel / 8;
        int bmpRowSize = ((width * bytesPerPixelIn + 3) / 4) * 4;  // 4バイト境界アライメント

        int outputPos = HeaderSize;

        // 画像を上から下、左から右に出力
        // BMPのピクセルデータは通常下から上に格納されている（isTopDown=falseの場合）
        for (int y = 0; y < height; y++)
        {
            int srcY = isTopDown ? y : (height - 1 - y);
            int srcOffset = dataOffset + srcY * bmpRowSize;

            for (int x = 0; x < width; x++)
            {
                int pixelOffset = srcOffset + x * bytesPerPixelIn;

                byte b = 0, g = 0, r = 0;
                if (pixelOffset + 2 < bmpData.Length)
                {
                    b = bmpData[pixelOffset];
                    g = bmpData[pixelOffset + 1];
                    r = bmpData[pixelOffset + 2];
                }

                // RGB888 → BGR565変換 (AppWizard/emWinはBGR順)
                // BGR565: BBBBBGGG GGGRRRRR (16bit, little-endian)
                ushort rgb565 = (ushort)(
                    ((b >> 3) << 11) |  // 5bit blue  (bits 15-11)
                    ((g >> 2) << 5) |   // 6bit green (bits 10-5)
                    (r >> 3)            // 5bit red   (bits 4-0)
                );

                // Little-endian出力
                output[outputPos++] = (byte)(rgb565 & 0xFF);
                output[outputPos++] = (byte)((rgb565 >> 8) & 0xFF);
            }
        }

        // 残りは0のまま（パディング）
        // outputPosは 12 + 765*256*2 = 391692 のはず

        return output;
    }

    /// <summary>
    /// ファイルからBMPを読み込んでAppWizard形式に変換
    /// </summary>
    public static byte[] ConvertBmpFileToAppWizard(string bmpFilePath)
    {
        var bmpData = File.ReadAllBytes(bmpFilePath);
        return ConvertBmpToAppWizard(bmpData);
    }

    /// <summary>
    /// AppWizard形式からBMPに変換（逆変換）
    /// </summary>
    public static byte[] ConvertAppWizardToBmp(byte[] appWizardData)
    {
        // AppWizardヘッダー確認
        if (appWizardData.Length < HeaderSize)
        {
            throw new ArgumentException("AppWizardデータが短すぎます");
        }

        // AppWizardヘッダ構造:
        // [0-1]: "BM" マジック
        // [2-3]: 圧縮タイプ
        // [4-5]: 幅
        // [6-7]: 高さ
        // [8-9]: stride
        // [10-11]: bits per pixel
        int width = appWizardData[4] | (appWizardData[5] << 8);
        int height = appWizardData[6] | (appWizardData[7] << 8);

        // デフォルト値で上書き（ヘッダーが不正な場合）
        if (width != ExpectedWidth || height != ExpectedHeight)
        {
            width = ExpectedWidth;
            height = ExpectedHeight;
        }

        // BMPファイル作成
        int rowSize = ((width * 3 + 3) / 4) * 4;  // 4バイト境界
        int imageSize = rowSize * height;
        int fileSize = 54 + imageSize;

        var bmpData = new byte[fileSize];
        int pos = 0;

        // BMPファイルヘッダー (14バイト)
        bmpData[pos++] = 0x42;  // 'B'
        bmpData[pos++] = 0x4D;  // 'M'
        bmpData[pos++] = (byte)(fileSize & 0xFF);
        bmpData[pos++] = (byte)((fileSize >> 8) & 0xFF);
        bmpData[pos++] = (byte)((fileSize >> 16) & 0xFF);
        bmpData[pos++] = (byte)((fileSize >> 24) & 0xFF);
        bmpData[pos++] = 0; bmpData[pos++] = 0;  // Reserved
        bmpData[pos++] = 0; bmpData[pos++] = 0;  // Reserved
        bmpData[pos++] = 54;  // Pixel data offset
        bmpData[pos++] = 0; bmpData[pos++] = 0; bmpData[pos++] = 0;

        // BMP情報ヘッダー (40バイト)
        bmpData[pos++] = 40; bmpData[pos++] = 0; bmpData[pos++] = 0; bmpData[pos++] = 0;  // Header size
        bmpData[pos++] = (byte)(width & 0xFF);
        bmpData[pos++] = (byte)((width >> 8) & 0xFF);
        bmpData[pos++] = (byte)((width >> 16) & 0xFF);
        bmpData[pos++] = (byte)((width >> 24) & 0xFF);
        bmpData[pos++] = (byte)(height & 0xFF);
        bmpData[pos++] = (byte)((height >> 8) & 0xFF);
        bmpData[pos++] = (byte)((height >> 16) & 0xFF);
        bmpData[pos++] = (byte)((height >> 24) & 0xFF);
        bmpData[pos++] = 1; bmpData[pos++] = 0;  // Planes
        bmpData[pos++] = 24; bmpData[pos++] = 0;  // Bits per pixel
        bmpData[pos++] = 0; bmpData[pos++] = 0; bmpData[pos++] = 0; bmpData[pos++] = 0;  // Compression
        bmpData[pos++] = (byte)(imageSize & 0xFF);
        bmpData[pos++] = (byte)((imageSize >> 8) & 0xFF);
        bmpData[pos++] = (byte)((imageSize >> 16) & 0xFF);
        bmpData[pos++] = (byte)((imageSize >> 24) & 0xFF);
        // Resolution and colors (16 bytes of zeros)
        for (int i = 0; i < 16; i++) bmpData[pos++] = 0;

        // ピクセルデータ変換 (BGR565 → BGR888)
        // BMPは下から上に格納されるため、AppWizardの上から下のデータを反転
        // 注意: AppWizardヘッダは12バイトだが、データは16バイト目から開始
        // (12バイトヘッダ + 4バイトパディング)
        int appWizardOffset = 16;  // 16バイトオフセット
        int srcStride = width * 2;  // AppWizardの1行のバイト数 (RGB565)

        for (int y = height - 1; y >= 0; y--)
        {
            int bmpRowStart = 54 + y * rowSize;
            int srcRow = height - 1 - y;  // AppWizardの対応する行

            for (int x = 0; x < width; x++)
            {
                int srcOffset = appWizardOffset + srcRow * srcStride + x * 2;

                if (srcOffset + 1 < appWizardData.Length)
                {
                    // BGR565 (little-endian)
                    ushort bgr565 = (ushort)(appWizardData[srcOffset] | (appWizardData[srcOffset + 1] << 8));

                    // BGR565 → BGR888
                    byte b = (byte)((bgr565 >> 11) << 3);  // 5bit blue
                    byte g = (byte)(((bgr565 >> 5) & 0x3F) << 2);  // 6bit green
                    byte r = (byte)((bgr565 & 0x1F) << 3);  // 5bit red

                    bmpData[bmpRowStart + x * 3 + 0] = b;
                    bmpData[bmpRowStart + x * 3 + 1] = g;
                    bmpData[bmpRowStart + x * 3 + 2] = r;
                }
            }
        }

        return bmpData;
    }

    /// <summary>
    /// 変換結果のサイズを取得
    /// </summary>
    public static int GetOutputSize() => OutputSize;
}
