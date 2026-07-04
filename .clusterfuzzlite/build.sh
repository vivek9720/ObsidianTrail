#!/bin/bash -eu
PROJECT="${SRC:-$(pwd)}"
if [ -d "$PROJECT/ObsidianTrail" ]; then PROJECT="$PROJECT/ObsidianTrail"; fi
cd "$PROJECT"
: "${OUT:?OUT must be set}"
mkdir -p "$OUT"
COMMON_FLAGS="${CXXFLAGS:-} -std=c++17 -I$PROJECT/include"
SOURCES=$(find "$PROJECT/src" -name '*.cpp' | sort)
for target in record_fuzzer manifest_fuzzer ledger_fuzzer timeline_fuzzer identity_fuzzer network_fuzzer policy_fuzzer redaction_fuzzer report_fuzzer bundle_fuzzer; do
  $CXX $COMMON_FLAGS "$PROJECT/fuzz/${target}.cc" $SOURCES ${LIB_FUZZING_ENGINE:-} -o "$OUT/$target"
done
