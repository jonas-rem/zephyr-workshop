#!/usr/bin/env bash

set -euo pipefail

WORKSHOP_ROOT=$(realpath "$(dirname "$0")/..")

cd "$WORKSHOP_ROOT"
exec west twister -T app -T samples --integration --exclude-tag workshop "$@"
