# 6.25 routing and effects quality — development candidate

New Custom row mixer routing complements the unchanged Parallel, Series and Dual mono modes. Click a row's ROUTE button or choose Custom row mixer in the routing selector.

Each row receives any combination of the app input and earlier row outputs. Multiple consumers split a signal; multiple sources merge. Feed-forward order rejects self/backward connections. Each enabled row processes exactly once per sample, including stereo effects. Disabled rows are silent and excluded from averaging. A row with no sources still processes silence, so existing tails can run. No selected outputs means silence.

Controls: stereo/left/right/mono source selection, -60 to +12 dB row output gain, stereo balance, polarity inversion, final output inclusion, and average versus unity summing. Gain, balance and polarity transitions are smoothed over 10 ms. Route control snapshots are applied at block boundaries without blocking audio when the editor holds the configuration mutex. Structural rewiring itself is immediate, not a gapless crossfade.

Templates: parallel rows, split/rejoin (row 1 into rows 2 and 3 then row 4), and two chains. Templates preserve existing devices and row enable states. Activate the required rows for the template. Global input/post effects still apply to Custom mode; use the existing Dual mono mode for isolated grid-only chains.

Routing persists in presets, stays attached to physical rows when effects move, and defaults safely when loading old presets. Routing does not switch with scenes and is not exposed as DAW automation. Connections are between whole rows, not arbitrary columns. Arbitrary multi-channel Anubis hardware routing is not implemented here.

Effects quality fix: switching Stereo Delay into tape mode during a clean delay-time crossfade now completes the 20 ms crossfade before starting tape glide. The new test fails against the old implementation and compares the corrected stereo audio with independent old/new fixed read heads at 44.1, 48, 96 and 192 kHz.

Local validation passed: custom split/merge signal oracle, isolated channels, unity and sum levels, invalid routes, smooth polarity, existing routing, advanced effect state moves, Stereo Delay, Stereo Reverb and the 137-parameter binding contract. The existing reverb decay test measured about 1.10–1.13 seconds for a 1.2-second target across those four rates. These are technical checks, not listening verification or a claim of proprietary effect equivalence.

Windows binary, native UI/preset tests, pluginval and installer verification are pending for this candidate. Anubis hardware and listening tests remain user-side.
