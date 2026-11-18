// Offline source-tap transition feasibility test. Build with C++17.
#include <algorithm>
#include <array>
#include <cassert>
#include <cmath>
#include <iostream>

struct TapBlend {
    int current = 0, next = 0, remaining = 0;
    float read(const std::array<float, 9>& taps, int requested, int length) {
        if (!remaining && requested != current) {
            next = requested;
            remaining = length;
        }
        if (!remaining) return taps[current];
        const float weight = float(length - remaining + 1) / length;
        if (--remaining == 0) {
            current = next;
            return taps[current];
        }
        return taps[current] + weight * (taps[next] - taps[current]);
    }
};

int main() {
    for (int rate : {44100, 48000, 96000}) {
        const int length = std::lround(rate * .005);
        TapBlend fade;
        std::array<float, 9> taps{};
        std::array<float, 127> echo{};
        float previousHard = 0, previousSmooth = 0;
        double hardStep = 0, smoothStep = 0, tailEnergy = 0;
        for (int i = 0; i < rate; ++i) {
            const float input = i < rate/2 ? .25f * std::cos(2*3.141592653589793*200*(i-rate/4)/rate) : 0.f;
            auto& memory = echo[i % echo.size()];
            const float delayed = memory;
            memory = input + .65f * delayed;
            taps[0] = input;
            taps[8] = -input + .1f * delayed;
            const int requested = i >= rate/4 ? 8 : 0;
            const float hard = taps[requested];
            const float smooth = fade.read(taps, requested, length);
            if (i == rate/4) {
                hardStep = std::abs(hard - previousHard);
                smoothStep = std::abs(smooth - previousSmooth);
            }
            if (i < rate/4 || i >= rate/4 + length-1) assert(hard == smooth);
            if (i >= rate/2) tailEnergy += smooth*smooth;
            previousSmooth = smooth;
            previousHard = hard;
        }
        assert(hardStep > .4 && smoothStep < .005 && tailEnergy > .001);
        std::cout << rate << " Hz, immediate step " << hardStep << ", 5 ms blend step " << smoothStep << ", tail energy " << tailEnergy << '\n';
    }
}
