#!/bin/bash

set -e

SCRIPT_DIR="$(cd "$(dirname "$0")"; pwd)"
. "$SCRIPT_DIR"/config.sh

cat - > "$INSTALL_PREFIX/bin/straining-coasters-launcher" <<'EOF'
#!/bin/sh

SCRIPT_DIR="$(cd "$(dirname "$0")"; pwd)"
"$SCRIPT_DIR"/straining-coasters \
    --data-path="$SCRIPT_DIR"/../share/straining-coasters
EOF

chmod a+x "$INSTALL_PREFIX/bin/straining-coasters-launcher"
