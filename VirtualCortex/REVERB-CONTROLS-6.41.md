# 6.41 reverb control efficiency — verified

Stereo Reverb skips already-settled smoothers and reuses unchanged duck envelope/wet-gain coefficients across configure calls. Preparation invalidates the cache. No parameter IDs, preset schema, routing, tank interpolation, decay design or GUI layout changes. Existing model and cabinet IR transition fixes remain.

Compiled code e9fbbcb64fdf8b6ee931c3504dde8da46d930d98; Windows CI https://github.com/mikey423a/VirtualCortex/actions/runs/44402148003 (job 135147751312), verified November 18, 2026. Actual EXE/VST3 compilation, all 48 regressions, pluginval strictness 5, standalone 10-second startup, installer install/uninstall passed. Separate Steinberg validator not configured.

Frozen 6.40 renderer comparisons passed bit-for-bit for stereo samples across preparation rates 8/44.1/48/96/192kHz and repeated restarts, repeated/automated controls, malformed release/wet values, bypass, sleep/wake and predelay changes. Existing reverb regressions retained. Local GCC -O2 passed; ASan/UBSan completed with final PASS (leak detection disabled).

Paired six-pass Windows microbenchmark: 480000 frames at48kHz, configure every64, alternating implementation order; median steady optimized190.664ms versus legacy194.308ms (1.88% lower); automated197.243ms versus201.323ms (2.03% lower). Checksums match. Local medians153.066/164.981ms steady and158.252/162.545ms automated. These are single-run microbenchmark results, not app-wide CPU or sonic-quality guarantees.

Downloaded Windows archive SHA256 1db3d4e1022dfe234a8615d21e5e1ef6a4196ddfd398dc8950435f8d5f2cd090 and logs archive c0ff671864663beb64b7d013d17e5ecc01b7b0837567e7072c29ab62a684575b matched GitHub digests. All four package hashes verified.
Installer SHA256 27dee8d451412a71c0d00b7a5f5bee5bff94e1f456c7d8619f13017c0926f3c2.
VST3 ZIP SHA256 bd64a9e4ce89bfd2021a9d71cf0f7ca8d1b46e7b6f83020ef93dffe1d592f0ba.

Native entry-H screenshots reviewed at2560x1440 and822x600; four rounded-square rows, whole-row entry caption, skipped dimming and full-width layout retained. Footer now6.41. No new physical OS interaction claim.

Next: optional compatible sonic improvements, measured audio topology-transition handling and physical OS menus/file dialogs. Do not repeat these completed control-efficiency checks. No proprietary equivalence, click-free or whole-app completion claim. Installer unsigned; Anubis/Windows11 hardware and listening remain user-side.
