#!/usr/bin/env bash
usage() {
  cat <<'USAGE'
Usage: . scripts/toolchain.sh   (sourced by build.sh, scripts/mac-build.sh, scripts/run-unit-tests.sh)
       scripts/toolchain.sh     (prints what it resolved)
Resolves the build inputs once, so every build and test entry point finds the same ones:
  SDK_ROOT  parent of foobar2000/ + pfc/: the checkout's parent when it has them, else
            ~/.local/share/foo_navidrome-sdk (the SDK copy foo_navidrome uses on a Mac),
            else the checkout's parent (build.sh then fetches the SDK there)
  XWIN      xwin splat: ~/.xwin, else ~/.local/share/xwin/sdk (foo_navidrome's Mac layout)
  PATH      on macOS, Homebrew's llvm (clang-cl, llvm-rc, llvm-mt) and lld (lld-link) first
USAGE
}

_pui_root="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
if [ -z "${SDK_ROOT:-}" ]; then
  _pui_parent="$(cd "$_pui_root/.." && pwd)"
  _pui_shared="$HOME/.local/share/foo_navidrome-sdk"
  if [ -d "$_pui_parent/foobar2000/SDK" ] && [ -d "$_pui_parent/pfc" ]; then SDK_ROOT="$_pui_parent"
  elif [ -d "$_pui_shared/foobar2000/SDK" ] && [ -d "$_pui_shared/pfc" ]; then SDK_ROOT="$_pui_shared"
  else SDK_ROOT="$_pui_parent"; fi
fi
if [ -z "${XWIN:-}" ]; then
  if [ -d "$HOME/.xwin/crt" ] || [ ! -d "$HOME/.local/share/xwin/sdk/crt" ]; then XWIN="$HOME/.xwin"
  else XWIN="$HOME/.local/share/xwin/sdk"; fi
fi
if [ "$(uname)" = Darwin ] && command -v brew >/dev/null; then
  for _pui_f in llvm lld; do
    _pui_p="$(brew --prefix "$_pui_f" 2>/dev/null)/bin"
    [ -d "$_pui_p" ] && case ":$PATH:" in *":$_pui_p:"*) ;; *) PATH="$_pui_p:$PATH" ;; esac
  done
fi
export SDK_ROOT XWIN PATH
unset _pui_root _pui_parent _pui_shared _pui_f _pui_p

if [ "${BASH_SOURCE[0]}" = "$0" ]; then
  case "${1:-}" in -h|--help) usage; exit 0 ;; esac
  echo "SDK_ROOT=$SDK_ROOT"
  echo "XWIN=$XWIN"
  echo "clang-cl=$(command -v clang-cl || echo missing)"
  echo "lld-link=$(command -v lld-link || echo missing)"
fi
