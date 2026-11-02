# Virtual Cortex Windows build setup

Status: prepared only. No Windows build or installer has run.
GitHub refused branch creation in mikey423a/RhythmFXSync1 with HTTP 403:
Resource not accessible by integration.

The workflow builds only VirtualCortex/ and leaves RhythmFXSync/ untouched.
It checks out JUCE 8.0.10, downloads Steinberg ASIO SDK, uses MSVC on windows-2022,
runs CTest, validates VST3 with pluginval 1.0.4 at level 5, tests standalone
startup, and compiles an Inno Setup installer. It then tests installer deployment
and uninstall before uploading the binaries as an Actions artifact.

The workflow and installer scripts are unexecuted and may need corrections.
A successful run still does not verify Anubis hardware or interactive GUI behavior.

With repository write/workflow access, commit the VirtualCortex folder and
.github/workflows/virtualcortex-windows.yml. The push starts the build.
Do not upload this ZIP as a single file: the workflow needs the extracted paths.
No changes to the existing RhythmFXSync source are necessary.
