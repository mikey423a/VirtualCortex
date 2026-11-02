"""Static contract check: JUCE editor bindings must reference declared processor IDs."""
from pathlib import Path
import re

root = Path(__file__).resolve().parents[1]
processor = (root / "Source/PluginProcessor.cpp").read_text()
editor = (root / "Source/PluginEditor.cpp").read_text()

# Restrict the parameter scan to the actual declaration function.
layout = processor.split("VirtualCortexProcessor::makeLayout(){", 1)[1].split("\nbool VirtualCortexProcessor::isBusesLayoutSupported", 1)[0]
declared = re.findall(r'add(?:Float|Bool)\("([^"]+)"', layout)
assert len(declared) == len(set(declared)), "Duplicate processor parameter IDs"
assert declared, "No parameters found"

# Both editor arrays are part of the plugin's user-facing control contract.
arrays = re.findall(r'const std::array<const char\*,\s*\d+>\s+(?:ids|switchesIds)\s*=\s*\{([^}]+)\}', editor)
assert len(arrays) == 2, "Could not locate both editor parameter ID arrays"
used = [item for array in arrays for item in re.findall(r'"([^"]+)"', array)]
assert len(used) == len(set(used)), "Editor binds the same parameter more than once"
missing = sorted(set(used) - set(declared))
assert not missing, f"Editor references missing processor parameters: {missing}"
# Prevent an accidental silent loss of a control from the editor.
assert set(used) == set(declared), f"Processor parameters without editor controls: {sorted(set(declared)-set(used))}"
print(f"Verified {len(declared)} unique processor parameters and editor bindings")

legacy = {"gate","drive","ampBias","inputHP","tone","level","cab","cabCut","delayTime","delayFeedback","delayMix","reverbMix","tremRate","tremDepth","outputTrim","eqLow","eqMid","eqHigh","gateOn","ampOn","cabOn","delayOn","reverbOn","tremOn","bypass","eqOn","irOn","gridMode"}
assert legacy <= set(declared), "An existing automation parameter was removed"

# New automation parameters must append to the established 6.9 order.
expected = ["gate","drive","ampBias","inputHP","tone","level","cab","cabCut","delayTime","delayFeedback","delayMix","reverbMix","tremRate","tremDepth","outputTrim","eqLow","eqMid","eqHigh","gateOn","ampOn","cabOn","delayOn","reverbOn","tremOn","eqOn","bypass","irOn","gridMode","sdTimeL","sdTimeR","sdFeedback","sdCross","sdLowCut","sdHighCut","sdMix","sdModRate","sdModDepth","sdDucking","sdDuckRelease","sdWidth","sdWetLevel","sdGlide","sdBpm","sdBeatsL","sdBeatsR","sdFilterMix","stereoDelayOn","sdPingPong","sdTape","sdSync"]
assert declared[:50] == expected, "Existing host parameter order changed"

assert len(declared) == 71, "Unexpected automation parameter count"
assert declared[50:69] == ["srDecay","srSize","srPreDelay","srDiffusion","srEarly","srMix","srLowDecay","srHighDecay","srLowFreq","srHighFreq","srModRate","srModDepth","srWidth","srDucking","srRelease","srWetLevel","srInputHP","srInputLP","stereoReverbOn"], "Existing reverb parameter order changed"
assert declared[69] == "srPreGlide", "Pre-delay mode must append to the existing host order"

assert declared[70] == "sdDiffusion", "Diffusion must append without reordering automation"
