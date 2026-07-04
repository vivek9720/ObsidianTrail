# ObsidianTrail

ObsidianTrail is an offline C++17 library and command-line tool for turning
incident-response evidence bundles into normalized timelines, actor summaries,
network pivots, policy findings, and redacted reports. It is designed for local
case folders where logs, endpoint exports, identity snapshots, and analyst notes
must be reviewed without sending evidence to an external service.

## Capabilities

- Parse line-oriented evidence records, key/value notes, compact manifest files,
  and small binary ledger pages.
- Normalize principals, sessions, hosts, IP ranges, process names, artifact paths,
  timestamps, and confidence labels.
- Build an append-only case ledger with deterministic checksums and replayable
  timeline state.
- Evaluate local policy queries over the timeline and evidence graph.
- Detect useful investigation signals such as impossible travel, stale sessions,
  privilege changes, suspicious network pivots, excessive redaction loss, and
  contradictory source records.
- Produce text and JSON-style summaries for review packets.

The project uses only the C++ standard library and performs deterministic local
file processing. The `fuzz/` directory contains developer robustness harnesses
for the same parsers and state machines used by the CLI.

## Library Areas

- `core`: text normalization, byte readers, timestamps, diagnostics, and a compact JSON parser.
- `evidence`: line records, manifests, artifact paths, bundles, and append-only ledgers.
- `identity` and `network`: principal/session and flow normalization.
- `timeline`: ordering, correlation, and investigation windows.
- `analysis`: case graph construction, statistics, scoring, and playbook rules.
- `storage`: simple local tabular exports used by tools and tests.
