# Moving-delay detail verification

Test-only follow-up to 6.46. Evaluates the original Chorus and Flanger moving delay against a sinusoid evaluated at the same fractional position, so the reference includes the intended pitch sweep. Tests cover 1 kHz and 6.5 kHz at 44.1, 48 and 96 kHz, fully wet with feedback off. Cubic output must reduce squared error and bring RMS closer to the reference. Delay history and modulation phase must match both read modes exactly.

Local GCC passed all 12 cases. At 48 kHz and 6.5 kHz, Chorus RMS error against the reference was -0.523 dB with linear and -0.053 dB with Detail Read. Flanger measured -0.520 and -0.053 dB. This measures level and interpolation error for specific signals and settings. Broader listening and sideband isolation remain open. Windows verification pending. The current delivered app is 6.46.
