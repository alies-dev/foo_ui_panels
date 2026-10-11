#!/usr/bin/env bash
usage() {
  cat <<'USAGE'
Usage: scripts/release-build.sh <version>
Builds the Windows DLLs (x64, ARM64EC, x86) with PUI_VERSION=<version> and packages
foo_ui_panels_<version>.fb2k-component (x64/ + arm64ec/ + the x86 DLL at the root) in the repo root.
Called by semantic-release (.releaserc.json prepareCmd); writes released_version to
$GITHUB_OUTPUT when run in Actions.
PREBUILT_DIR=<dir>  reuse the Build run's artifacts instead of compiling when <dir>/stamp/version
                    equals <version>: <dir>/{x64,arm64ec,x86}/foo_ui_panels.dll, and
                    <dir>/mac/foo_ui_panels-mac.zip when present (then has_mac=true is written to
                    $GITHUB_OUTPUT and the asset already carries the macOS bundle).
USAGE
}
case "${1:-}" in -h|--help) usage; exit 0 ;; esac

set -euo pipefail

[ $# -ge 1 ] || { usage >&2; exit 2; }
VERSION="$1"
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
cd "$ROOT"

export PUI_VERSION="$VERSION"

if [ -n "${GITHUB_OUTPUT:-}" ]; then
  echo "released_version=$VERSION" >> "$GITHUB_OUTPUT"
fi

PRE="${PREBUILT_DIR:-}"
DLL_X64=build/foo_ui_panels.dll
DLL_ARM64EC=build-arm64ec/foo_ui_panels.dll
DLL_X86=build-x86/foo_ui_panels.dll
MAC_ARGS=()

if [ -n "$PRE" ] && [ "$(cat "$PRE/stamp/version" 2>/dev/null)" = "$VERSION" ] \
   && [ -f "$PRE/x64/foo_ui_panels.dll" ] && [ -f "$PRE/arm64ec/foo_ui_panels.dll" ] \
   && [ -f "$PRE/x86/foo_ui_panels.dll" ]; then
  echo "reusing the Build run's DLLs stamped $VERSION"
  DLL_X64="$PRE/x64/foo_ui_panels.dll"
  DLL_ARM64EC="$PRE/arm64ec/foo_ui_panels.dll"
  DLL_X86="$PRE/x86/foo_ui_panels.dll"
  if [ -f "$PRE/mac/foo_ui_panels-mac.zip" ]; then
    rm -rf "$PRE/mac/x"
    unzip -q "$PRE/mac/foo_ui_panels-mac.zip" -d "$PRE/mac/x"
    MAC_ARGS=(--mac "$PRE/mac/x/foo_ui_panels.component")
    if [ -n "${GITHUB_OUTPUT:-}" ]; then
      echo "has_mac=true" >> "$GITHUB_OUTPUT"
    fi
  fi
else
  [ -z "$PRE" ] || echo "no prebuilt DLLs stamped $VERSION in $PRE, building"
  WIN_ARCH=x64 ./build.sh
  WIN_ARCH=arm64ec ./build.sh
  WIN_ARCH=x86 ./build.sh
fi

ASSET="foo_ui_panels_${VERSION}.fb2k-component"
rm -f "$ASSET"
python3 scripts/package.py "$ASSET" --dll "$DLL_X64" \
  --arm64ec "$DLL_ARM64EC" --x86 "$DLL_X86" "${MAC_ARGS[@]}"
echo "packaged: $ASSET"
