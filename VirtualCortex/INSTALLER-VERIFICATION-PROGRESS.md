# Installer verification follow-up

Based on verified 6.28 at da8dbf1ab064fb4a67afd8083148728b810d54fe. No active/queued Windows CI was present before editing.

The installer previously verified EXE byte identity but only VST3 presence. The packaging script now compares installed and built VST3 SHA256 hashes and fails on mismatch before uninstall/package upload. No application, DSP, preset or GUI changes; version remains 6.28.0 because this changes validation only.

Windows CI verification of this script change is pending. Do not replace the delivered verified 6.28 downloads or promote this commit until its CI succeeds. Next: inspect CI result and deployment logs, then continue the routing wire separation and inactive-grid display work in ROUTING-6.28.md. Actual native GUI inspection is required for the next GUI change. Anubis remains user-side.
