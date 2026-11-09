# Live NAM integration — development only

ModelBlockAdapter bridges the sample graph to a two-channel block processor.
It uses fixed buffers, sanitizes nonfinite input/output, and introduces exactly
one block of latency. Its tests cover block sizes 1, 32, 128 and 512, stereo
isolation, startup/reset silence and stream continuity.

This is not a NAM implementation or an enabled app feature. Remaining work:
- Wrap two prepared mono NAM engines with the adapter's process interface.
- Attach the existing ownership exchange and handle failed loads and restarts.
- Compensate dry and parallel paths for introduced latency before enabling it.
- Add model file selection and persistent paths without changing existing IDs.
- Verify real models, callback allocations, CPU load and Windows runtime.

The verified user-facing release remains 6.20.0.
