# Subharmonicon contributor notes

This repository builds an unofficial Subharmonicon firmware extension. C and
assembly under `src/` own the extension, hooks, stock ABI, and linker layout.
`firmware_sysex.py`, `patcher.py`, and `tools/` own firmware validation, patch
generation, application, and release packaging. Never add Moog firmware or
other vendor binaries to the repository.

Documentation contains current facts and contracts. History lives in Git.

## Workflow

Carry implementation requests through edits and relevant verification. Resolve
routine choices from the task and current code; ask only when a missing decision
materially affects the result. Read-only reviews and diagnoses end with findings
unless implementation is requested.

Validate external data at its entry point and trust internal callers. Keep the
stock-firmware hash, patch address bounds, segment overlap checks, and SysEx
container validation intact. Delete dead code with its build entries. Do not add
durable tests for new work; use existing checks and focused temporary experiments.

For code changes, run `make check`. Run `make release` when patch generation or
packaging changes. When an official 1.1.1 SysEx is available, validate that path
with `SUBH_STOCK_SYSEX=/path/to/firmware.syx make check`. Documentation or agent
configuration changes require only diff and syntax review.

After completing requested changes and verification, commit them unless the user
asks to leave them uncommitted. Stage only the task's changes. Pushing requires
separate user direction. Report the result, verification, and remaining limits
in concise prose.
