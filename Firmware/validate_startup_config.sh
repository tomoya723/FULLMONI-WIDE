#!/bin/bash

# スタートアップ画面設定検証スクリプト
# Startup Screen Configuration Validation Script
#
# このスクリプトは、スタートアップ画面の設定ファイルが正しく配置されているか検証します
# This script validates that startup screen configuration files are properly placed

set -e

# カラー出力用 / For colored output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

echo -e "${GREEN}========================================${NC}"
echo -e "${GREEN}スタートアップ画面設定検証${NC}"
echo -e "${GREEN}Startup Screen Configuration Validation${NC}"
echo -e "${GREEN}========================================${NC}"
echo ""

# スクリプトのディレクトリを取得 / Get script directory
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
FIRMWARE_DIR="$SCRIPT_DIR"

ERRORS=0
WARNINGS=0

# 関数: ファイル存在チェック / Function: Check file existence
check_file() {
    local file="$1"
    local description="$2"
    
    if [ -f "$file" ]; then
        echo -e "${GREEN}✓${NC} $description: $file"
        return 0
    else
        echo -e "${RED}✗${NC} $description が見つかりません / not found: $file"
        ERRORS=$((ERRORS + 1))
        return 1
    fi
}

# 関数: ディレクトリ存在チェック / Function: Check directory existence
check_dir() {
    local dir="$1"
    local description="$2"
    
    if [ -d "$dir" ]; then
        echo -e "${GREEN}✓${NC} $description: $dir"
        return 0
    else
        echo -e "${RED}✗${NC} $description が見つかりません / not found: $dir"
        ERRORS=$((ERRORS + 1))
        return 1
    fi
}

# 1. 設定ファイルの存在確認 / Check configuration file
echo "[1] 設定ファイル / Configuration Files"
check_file "$FIRMWARE_DIR/src/startup_config.h" "スタートアップ設定ヘッダ / Startup config header"
echo ""

# 2. ドキュメントの存在確認 / Check documentation
echo "[2] ドキュメント / Documentation"
check_file "$FIRMWARE_DIR/docs/STARTUP_SCREEN_CUSTOMIZATION.md" "カスタマイズガイド / Customization guide"
check_file "$FIRMWARE_DIR/README.md" "Firmware README"
echo ""

# 3. カスタムリソースディレクトリの確認 / Check custom resources directory
echo "[3] カスタムリソースディレクトリ / Custom Resources Directory"
check_dir "$FIRMWARE_DIR/custom_resources/startup_images" "カスタム画像ディレクトリ / Custom images directory"
check_file "$FIRMWARE_DIR/custom_resources/startup_images/README.md" "カスタム画像README / Custom images README"
check_file "$FIRMWARE_DIR/custom_resources/startup_images/.gitignore" "カスタム画像.gitignore / Custom images .gitignore"
echo ""

# 4. ヘルパースクリプトの確認 / Check helper script
echo "[4] ヘルパースクリプト / Helper Scripts"
check_file "$FIRMWARE_DIR/customize_startup.sh" "カスタマイズスクリプト / Customization script"
if [ -f "$FIRMWARE_DIR/customize_startup.sh" ]; then
    if [ -x "$FIRMWARE_DIR/customize_startup.sh" ]; then
        echo -e "${GREEN}✓${NC} customize_startup.sh は実行可能 / is executable"
    else
        echo -e "${YELLOW}⚠${NC} customize_startup.sh が実行可能ではありません / is not executable"
        echo "    chmod +x customize_startup.sh を実行してください / Run chmod +x customize_startup.sh"
        WARNINGS=$((WARNINGS + 1))
    fi
fi
echo ""

# 5. デフォルト画像リソースの確認 / Check default image resources
echo "[5] デフォルト画像リソース / Default Image Resources"
check_file "$FIRMWARE_DIR/aw001/Resource/Image/mtc.bmp" "aw001 デフォルト画像 / Default image"
check_file "$FIRMWARE_DIR/aw002/Resource/Image/mtc.bmp" "aw002 デフォルト画像 / Default image"
echo ""

# 6. 生成ファイルの確認 / Check generated files
echo "[6] 生成ファイル / Generated Files"
check_file "$FIRMWARE_DIR/aw001/Source/Generated/ID_SCREEN_00.c" "aw001 スタートアップ画面コード / Startup screen code"
check_file "$FIRMWARE_DIR/aw001/Source/Generated/Resource.h" "aw001 リソースヘッダ / Resource header"
check_file "$FIRMWARE_DIR/aw002/Source/Generated/ID_SCREEN_00.c" "aw002 スタートアップ画面コード / Startup screen code"
check_file "$FIRMWARE_DIR/aw002/Source/Generated/Resource.h" "aw002 リソースヘッダ / Resource header"
echo ""

# 7. settings.h の確認 / Check settings.h
echo "[7] ビルド設定 / Build Configuration"
check_file "$FIRMWARE_DIR/src/settings.h" "設定ヘッダ / Settings header"
if [ -f "$FIRMWARE_DIR/src/settings.h" ]; then
    DISP_VALUE=$(grep "^#define DISP" "$FIRMWARE_DIR/src/settings.h" | awk '{print $3}' | tr -d '()')
    if [ -n "$DISP_VALUE" ]; then
        echo -e "${GREEN}✓${NC} 現在の DISP 設定 / Current DISP setting: $DISP_VALUE"
        if [ "$DISP_VALUE" != "1" ] && [ "$DISP_VALUE" != "2" ]; then
            echo -e "${YELLOW}⚠${NC} DISP値が予期しない値です / Unexpected DISP value: $DISP_VALUE"
            WARNINGS=$((WARNINGS + 1))
        fi
    else
        echo -e "${YELLOW}⚠${NC} DISP 定義が見つかりません / DISP definition not found"
        WARNINGS=$((WARNINGS + 1))
    fi
fi
echo ""

# 結果サマリー / Results summary
echo -e "${GREEN}========================================${NC}"
echo -e "${GREEN}検証結果 / Validation Results${NC}"
echo -e "${GREEN}========================================${NC}"

if [ $ERRORS -eq 0 ] && [ $WARNINGS -eq 0 ]; then
    echo -e "${GREEN}✓ すべてのチェックに合格しました！${NC}"
    echo -e "${GREEN}✓ All checks passed!${NC}"
    exit 0
elif [ $ERRORS -eq 0 ]; then
    echo -e "${YELLOW}⚠ $WARNINGS 個の警告があります${NC}"
    echo -e "${YELLOW}⚠ $WARNINGS warning(s) found${NC}"
    exit 0
else
    echo -e "${RED}✗ $ERRORS 個のエラーがあります${NC}"
    echo -e "${RED}✗ $ERRORS error(s) found${NC}"
    if [ $WARNINGS -gt 0 ]; then
        echo -e "${YELLOW}⚠ $WARNINGS 個の警告があります${NC}"
        echo -e "${YELLOW}⚠ $WARNINGS warning(s) found${NC}"
    fi
    exit 1
fi
