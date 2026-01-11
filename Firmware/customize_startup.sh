#!/bin/bash

# スタートアップ画像カスタマイズヘルパースクリプト
# Startup Image Customization Helper Script
#
# このスクリプトは、カスタムスタートアップ画像を簡単に設定するためのものです
# This script helps easily configure custom startup images
#
# 使い方 / Usage:
#   ./customize_startup.sh <画像ファイル> <aw001|aw002>
#   ./customize_startup.sh <image_file> <aw001|aw002>
#
# 例 / Example:
#   ./customize_startup.sh my_logo.bmp aw001

set -e

# カラー出力用 / For colored output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# 引数チェック / Check arguments
if [ $# -ne 2 ]; then
    echo -e "${RED}エラー: 引数が不正です${NC}"
    echo -e "${RED}Error: Invalid arguments${NC}"
    echo ""
    echo "使い方 / Usage:"
    echo "  $0 <画像ファイル> <aw001|aw002>"
    echo ""
    echo "例 / Example:"
    echo "  $0 my_logo.bmp aw001"
    exit 1
fi

IMAGE_FILE="$1"
TARGET_AW="$2"

# ターゲットチェック / Check target
if [ "$TARGET_AW" != "aw001" ] && [ "$TARGET_AW" != "aw002" ]; then
    echo -e "${RED}エラー: ターゲットは aw001 または aw002 である必要があります${NC}"
    echo -e "${RED}Error: Target must be aw001 or aw002${NC}"
    exit 1
fi

# 画像ファイルの存在確認 / Check if image file exists
if [ ! -f "$IMAGE_FILE" ]; then
    echo -e "${RED}エラー: 画像ファイルが見つかりません: $IMAGE_FILE${NC}"
    echo -e "${RED}Error: Image file not found: $IMAGE_FILE${NC}"
    exit 1
fi

# 画像ファイルの拡張子チェック / Check image file extension
EXT="${IMAGE_FILE##*.}"
if [ "$EXT" != "bmp" ] && [ "$EXT" != "BMP" ]; then
    echo -e "${YELLOW}警告: BMP形式が推奨されます (現在: $EXT)${NC}"
    echo -e "${YELLOW}Warning: BMP format is recommended (current: $EXT)${NC}"
fi

# スクリプトのディレクトリを取得 / Get script directory
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
FIRMWARE_DIR="$SCRIPT_DIR"
CUSTOM_DIR="$FIRMWARE_DIR/custom_resources/startup_images"
TARGET_DIR="$FIRMWARE_DIR/$TARGET_AW/Resource/Image"

echo -e "${GREEN}========================================${NC}"
echo -e "${GREEN}スタートアップ画像カスタマイズ${NC}"
echo -e "${GREEN}Startup Image Customization${NC}"
echo -e "${GREEN}========================================${NC}"
echo ""
echo "画像ファイル / Image file: $IMAGE_FILE"
echo "ターゲット / Target: $TARGET_AW"
echo ""

# カスタムディレクトリに画像をコピー / Copy image to custom directory
echo -e "${YELLOW}[1/3]${NC} カスタムディレクトリに画像をコピー中..."
echo -e "${YELLOW}[1/3]${NC} Copying image to custom directory..."
BASENAME=$(basename "$IMAGE_FILE")
cp "$IMAGE_FILE" "$CUSTOM_DIR/$BASENAME"
echo -e "${GREEN}✓${NC} コピー完了 / Copy complete: $CUSTOM_DIR/$BASENAME"
echo ""

# 画像情報を表示 / Display image information
echo -e "${YELLOW}[2/3]${NC} 画像情報を確認中..."
echo -e "${YELLOW}[2/3]${NC} Checking image information..."
if command -v identify > /dev/null 2>&1; then
    IMAGE_INFO=$(identify "$IMAGE_FILE")
    echo "$IMAGE_INFO"
    
    # サイズチェック / Check size
    WIDTH=$(identify -format "%w" "$IMAGE_FILE")
    HEIGHT=$(identify -format "%h" "$IMAGE_FILE")
    
    if [ "$WIDTH" != "765" ] || [ "$HEIGHT" != "256" ]; then
        echo -e "${YELLOW}警告: 推奨サイズは 765x256 です (現在: ${WIDTH}x${HEIGHT})${NC}"
        echo -e "${YELLOW}Warning: Recommended size is 765x256 (current: ${WIDTH}x${HEIGHT})${NC}"
    else
        echo -e "${GREEN}✓${NC} サイズ確認OK / Size OK: ${WIDTH}x${HEIGHT}"
    fi
else
    echo -e "${YELLOW}注意: ImageMagick がインストールされていないため、画像情報を確認できません${NC}"
    echo -e "${YELLOW}Note: Cannot check image info (ImageMagick not installed)${NC}"
fi
echo ""

# 次のステップを案内 / Guide next steps
echo -e "${YELLOW}[3/3]${NC} 次のステップ / Next steps:"
echo ""
echo "カスタム画像を適用するには、以下のいずれかの方法を使用してください:"
echo "To apply the custom image, use one of the following methods:"
echo ""
echo -e "${GREEN}方法1: emWin AppWizard を使用 (推奨)${NC}"
echo -e "${GREEN}Method 1: Use emWin AppWizard (Recommended)${NC}"
echo "  1. e² studio で $TARGET_AW/$TARGET_AW.AppWizard を開く"
echo "     Open $TARGET_AW/$TARGET_AW.AppWizard in e² studio"
echo "  2. Resource → Images → Add から $BASENAME を追加"
echo "     Add $BASENAME via Resource → Images → Add"
echo "  3. Screens → ID_SCREEN_00 → ID_IMAGE_01 の Bitmap を変更"
echo "     Change Bitmap of Screens → ID_SCREEN_00 → ID_IMAGE_01"
echo "  4. Generate Code してビルド"
echo "     Generate Code and build"
echo ""
echo -e "${GREEN}方法2: 手動で編集 (上級者向け)${NC}"
echo -e "${GREEN}Method 2: Manual Edit (Advanced)${NC}"
echo "  詳細は docs/STARTUP_SCREEN_CUSTOMIZATION.md を参照"
echo "  See docs/STARTUP_SCREEN_CUSTOMIZATION.md for details"
echo ""
echo -e "${GREEN}========================================${NC}"
echo -e "${GREEN}✓ 準備完了 / Ready${NC}"
echo -e "${GREEN}========================================${NC}"
