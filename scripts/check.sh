#!/usr/bin/env bash
set -euo pipefail

usage() {
  printf 'Usage: %s [Debug|Release] [--preset NAME]\n' "$0"
  printf 'Defaults: both configurations; LINSCALER_PRESET or msvc. Windows Git Bash only.\n'
}

configuration=Both
configuration_set=false
preset_set=false
ps_args=()
while [[ $# -gt 0 ]]; do
  case "$1" in
    Debug|Release)
      if [[ $configuration_set == true ]]; then
        usage >&2
        exit 2
      fi
      configuration=$1
      configuration_set=true
      shift
      ;;
    --preset)
      if [[ $preset_set == true || $# -lt 2 || ! $2 =~ ^[A-Za-z0-9][A-Za-z0-9_.-]*$ ]]; then
        usage >&2
        exit 2
      fi
      ps_args+=(-Preset "$2")
      preset_set=true
      shift 2
      ;;
    -h|--help)
      usage
      exit 0
      ;;
    *)
      usage >&2
      exit 2
      ;;
  esac
done

if [[ ${OS:-} != Windows_NT ]] || ! command -v cygpath >/dev/null 2>&1; then
  printf 'Use this script from Windows Git Bash. On other platforms use the CMake presets.\n' >&2
  exit 1
fi
if ! command -v powershell.exe >/dev/null 2>&1; then
  printf 'Windows PowerShell was not found on PATH.\n' >&2
  exit 1
fi

script_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
# Environment values are not automatically converted from Git Bash paths.
if [[ -n ${LINSCALER_CMAKE:-} && $LINSCALER_CMAKE == /* ]]; then
  export LINSCALER_CMAKE="$(cygpath -w "$LINSCALER_CMAKE")"
fi
exec powershell.exe -NoProfile -ExecutionPolicy RemoteSigned \
  -File "$(cygpath -w "$script_dir/check.ps1")" \
  -Configuration "$configuration" "${ps_args[@]}"
