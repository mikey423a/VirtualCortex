# Virtual Cortex feature target and current limits

User target: Quad Cortex-like Windows standalone and VST3 operation, excluding
creation/training of amp captures. Keep existing DSP, four-row square-block GUI,
2560x1440/fullscreen support, and Anubis ASIO input/output.

A complete reference inventory is not a complete implementation. No proprietary
Neural DSP DSP algorithms, factory model weights or capture engine have been
obtained. Exact sonic parity cannot be claimed or verified from screenshots and
the device list. Independent NAM playback is not Neural Capture compatibility.

## Work remaining

- Live NAM: off-thread load/prewarm, ownership transfer, sample-rate handling,
  click-controlled switching, state restore, model UI and callback CPU/allocation tests.
- Routing: explicit input selection, split/merge buses and output assignments;
  the present four enabled rows share input and average their outputs.
- Performance controls: actual scene state, stomp assignments, MIDI control and
  expression mapping, with host automation/preset compatibility tests.
- Missing processing: pitch/harmony, synth, looper and other catalog functions;
  implement and validate actual audio before exposing entries as playable.
- Model-specific amp/cab/effect behavior: acquire suitable licensed models or
  develop independently and test against available references. Do not map every
  device name to a generic algorithm and label that a recreation.
- System completion: tuner, tempo/control integration, comprehensive I/O workflow,
  Windows 11/Anubis device tests, listening validation, performance and recovery.
- Deliberately out of scope: user amp profiling/capture creation. Existing code is
  preserved; no new capture-creation workflow is planned.

This is a development gap list, not an exhaustive verified hardware specification.
Hardware/cloud account management functions need a separate compatibility decision.

## Ownership checkpoint

`RealtimeModelExchange.h` adds a bounded SPSC ownership bridge for heap-owned DSP
models. A loader prepares models and submits ownership; audio switches at block
boundaries and retires old objects; only the loader destroys retired objects.
Full queues apply backpressure, preserving the current model and caller ownership.
Null submissions explicitly unload. Both threads must stop before destruction.
This class is not yet wired into the processor, and does not add a playable model.

Local validation: optimized C++17 test and AddressSanitizer/UndefinedBehaviorSanitizer
with leak scanning disabled passed. The test covers full queues, unload, pending
shutdown cleanup and 100,000 concurrent ordered switches, with audio-thread
allocation/deletion assertions. LeakSanitizer cannot scan threads in this runtime;
object counts independently reach zero. Local CMake is unavailable. Windows CI
is the authoritative full-project build; no new release version is declared here.
