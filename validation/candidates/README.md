# Reverb coefficient cache candidate

Prototype only; not integrated into the released application. Skips recalculation of the reverb feedback/filter coefficients when all eight coefficient inputs are exactly unchanged. Invalidates on prepare. Smoothing and the existing 32-sample update cadence are retained.

An optimized Linux build produced byte-identical stereo float output against the current engine for 512000 frames with periodic decay, crossover and predelay changes. SHA256 for both renders: ae86c877121d3c20dceb62f3e1584406c40578b486c40b7c93072e326ed2d028.

This single comparison is not exhaustive. Windows compilation, integration, sample-rate/reset coverage and native processor tests remain required before release. Version 6.11 remains the released binary.
