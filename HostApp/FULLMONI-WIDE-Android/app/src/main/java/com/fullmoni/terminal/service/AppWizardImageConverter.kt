package com.fullmoni.terminal.service

import android.graphics.Bitmap
import android.graphics.Color
import java.io.ByteArrayOutputStream
import java.nio.ByteBuffer
import java.nio.ByteOrder

/**
 * BMP/Bitmap画像をSEGGER AppWizard/emWin形式に変換するコンバータ
 *
 * AppWizard形式 (emWin GUI_BITMAP_STREAM):
 * - ヘッダ: 16バイト
 *   - [0-1]: "BM" (0x42, 0x4D) - BMPマジック互換
 *   - [2-3]: 圧縮タイプ (0x0008 = 非圧縮RGB565)
 *   - [4-5]: 幅 (little-endian)
 *   - [6-7]: 高さ (little-endian)
 *   - [8-9]: stride (幅 * 2 bytes/pixel)
 *   - [10-11]: bits per pixel (16 = RGB565)
 *   - [12-13]: NumColors (0 = ダイレクトカラー)
 *   - [14-15]: NumTransPixels (0)
 * - データ: 非圧縮のRGB565ピクセルデータ（上から下へ、左から右へ）
 */
object AppWizardImageConverter {

    // AppWizardヘッダサイズ (BM + Type + Width + Height + BytesPerLine + BPP + NumColors + NumTransPixels)
    private const val HEADER_SIZE = 16

    // 出力サイズ (acmtc配列に合わせる: 765 × 256 × 2 + 16 = 391696)
    private const val OUTPUT_SIZE = 391696

    // 期待する画像サイズ
    const val EXPECTED_WIDTH = 765
    const val EXPECTED_HEIGHT = 256

    /**
     * 24bit/32bit BMPバイナリをAppWizard形式に変換
     *
     * @param bmpData 入力BMPデータ
     * @return AppWizard形式データ (391,696バイト固定)
     */
    fun convertBmpToAppWizard(bmpData: ByteArray): ByteArray {
        // BMPヘッダ検証
        require(bmpData.size >= 54) { "BMPデータが小さすぎます" }
        require(bmpData[0] == 0x42.toByte() && bmpData[1] == 0x4D.toByte()) { "BMP形式ではありません" }

        // BMPヘッダから情報取得 (little-endian)
        val buffer = ByteBuffer.wrap(bmpData).order(ByteOrder.LITTLE_ENDIAN)
        val width = buffer.getInt(18)
        val rawHeight = buffer.getInt(22)
        val height = kotlin.math.abs(rawHeight)
        val isTopDown = rawHeight < 0
        val bitsPerPixel = buffer.getShort(28).toInt()
        val dataOffset = buffer.getInt(10)

        require(width == EXPECTED_WIDTH && height == EXPECTED_HEIGHT) {
            "画像サイズが不正です。期待値: ${EXPECTED_WIDTH}×${EXPECTED_HEIGHT}, 実際: ${width}×${height}"
        }
        require(bitsPerPixel == 24 || bitsPerPixel == 32) {
            "24bit または 32bit BMPが必要です。実際: ${bitsPerPixel}bit"
        }

        // 出力バッファ
        val output = ByteArray(OUTPUT_SIZE)

        // AppWizardヘッダ作成
        output[0] = 0x42  // 'B'
        output[1] = 0x4D  // 'M'
        output[2] = 0x08  // 圧縮タイプ (低) = 0x0008
        output[3] = 0x00
        output[4] = (width and 0xFF).toByte()
        output[5] = ((width shr 8) and 0xFF).toByte()
        output[6] = (height and 0xFF).toByte()
        output[7] = ((height shr 8) and 0xFF).toByte()
        val stride = width * 2  // RGB565は2バイト/ピクセル
        output[8] = (stride and 0xFF).toByte()
        output[9] = ((stride shr 8) and 0xFF).toByte()
        output[10] = 0x10  // 16 bits per pixel
        output[11] = 0x00

        // BMPピクセルデータをRGB565に変換
        val bytesPerPixelIn = bitsPerPixel / 8
        val bmpRowSize = ((width * bytesPerPixelIn + 3) / 4) * 4  // 4バイト境界アライメント

        var outputPos = HEADER_SIZE

        // 画像を上から下、左から右に出力
        for (y in 0 until height) {
            val srcY = if (isTopDown) y else (height - 1 - y)
            val srcOffset = dataOffset + srcY * bmpRowSize

            for (x in 0 until width) {
                val pixelOffset = srcOffset + x * bytesPerPixelIn

                val b: Int
                val g: Int
                val r: Int
                if (pixelOffset + 2 < bmpData.size) {
                    b = bmpData[pixelOffset].toInt() and 0xFF
                    g = bmpData[pixelOffset + 1].toInt() and 0xFF
                    r = bmpData[pixelOffset + 2].toInt() and 0xFF
                } else {
                    b = 0; g = 0; r = 0
                }

                // RGB888 → BGR565変換 (AppWizard/emWinはBGR順)
                // BGR565: BBBBBGGG GGGRRRRR (16bit, little-endian)
                val rgb565 = ((b shr 3) shl 11) or ((g shr 2) shl 5) or (r shr 3)

                // Little-endian出力
                output[outputPos++] = (rgb565 and 0xFF).toByte()
                output[outputPos++] = ((rgb565 shr 8) and 0xFF).toByte()
            }
        }

        return output
    }

    /**
     * Android BitmapをAppWizard形式に変換
     *
     * @param bitmap 入力Bitmap (765x256推奨)
     * @return AppWizard形式データ (391,696バイト固定)
     */
    fun convertBitmapToAppWizard(bitmap: Bitmap): ByteArray {
        val width = bitmap.width
        val height = bitmap.height

        require(width == EXPECTED_WIDTH && height == EXPECTED_HEIGHT) {
            "画像サイズが不正です。期待値: ${EXPECTED_WIDTH}×${EXPECTED_HEIGHT}, 実際: ${width}×${height}"
        }

        // 出力バッファ
        val output = ByteArray(OUTPUT_SIZE)

        // AppWizardヘッダ作成
        output[0] = 0x42  // 'B'
        output[1] = 0x4D  // 'M'
        output[2] = 0x08  // 圧縮タイプ = 0x0008
        output[3] = 0x00
        output[4] = (width and 0xFF).toByte()
        output[5] = ((width shr 8) and 0xFF).toByte()
        output[6] = (height and 0xFF).toByte()
        output[7] = ((height shr 8) and 0xFF).toByte()
        val stride = width * 2
        output[8] = (stride and 0xFF).toByte()
        output[9] = ((stride shr 8) and 0xFF).toByte()
        output[10] = 0x10  // 16 bits per pixel
        output[11] = 0x00

        var outputPos = HEADER_SIZE

        // 画像を上から下、左から右に出力
        for (y in 0 until height) {
            for (x in 0 until width) {
                val pixel = bitmap.getPixel(x, y)
                val r = Color.red(pixel)
                val g = Color.green(pixel)
                val b = Color.blue(pixel)

                // RGB888 → BGR565変換
                val rgb565 = ((b shr 3) shl 11) or ((g shr 2) shl 5) or (r shr 3)

                output[outputPos++] = (rgb565 and 0xFF).toByte()
                output[outputPos++] = ((rgb565 shr 8) and 0xFF).toByte()
            }
        }

        return output
    }

    /**
     * AppWizard形式からBitmapに変換（逆変換・読み取り用）
     *
     * @param appWizardData AppWizard形式データ
     * @return Bitmap
     */
    fun convertAppWizardToBitmap(appWizardData: ByteArray): Bitmap {
        // AppWizardヘッダから情報取得
        val width = (appWizardData[4].toInt() and 0xFF) or ((appWizardData[5].toInt() and 0xFF) shl 8)
        val height = (appWizardData[6].toInt() and 0xFF) or ((appWizardData[7].toInt() and 0xFF) shl 8)

        // デフォルト値（ヘッダーが不正な場合）
        val actualWidth = if (width == EXPECTED_WIDTH) width else EXPECTED_WIDTH
        val actualHeight = if (height == EXPECTED_HEIGHT) height else EXPECTED_HEIGHT

        val bitmap = Bitmap.createBitmap(actualWidth, actualHeight, Bitmap.Config.ARGB_8888)

        // AppWizardデータ開始位置（16バイトオフセット - 12バイトヘッダ + 4バイトパディング）
        val appWizardOffset = 16
        val srcStride = actualWidth * 2

        for (y in 0 until actualHeight) {
            for (x in 0 until actualWidth) {
                val srcOffset = appWizardOffset + y * srcStride + x * 2

                if (srcOffset + 1 < appWizardData.size) {
                    // BGR565 (little-endian)
                    val bgr565 = (appWizardData[srcOffset].toInt() and 0xFF) or
                                 ((appWizardData[srcOffset + 1].toInt() and 0xFF) shl 8)

                    // BGR565 → RGB888
                    val b = ((bgr565 shr 11) and 0x1F) shl 3
                    val g = ((bgr565 shr 5) and 0x3F) shl 2
                    val r = (bgr565 and 0x1F) shl 3

                    bitmap.setPixel(x, y, Color.rgb(r, g, b))
                }
            }
        }

        return bitmap
    }

    /**
     * 変換結果のサイズを取得
     */
    fun getOutputSize(): Int = OUTPUT_SIZE
}
