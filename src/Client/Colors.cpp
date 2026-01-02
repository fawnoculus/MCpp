#include <algorithm>
#include <cmath>
#include <cstdint>
#include <stdfloat>

#include "glm/vec3.hpp"

using std::float32_t;
using glm::vec3;

namespace Colors {
    uint8_t getAlpha(const uint32_t color) {
        return static_cast<uint8_t>(color >> 24 & 0xFF);
    }

    uint8_t getRed(const uint32_t color) {
        return static_cast<uint8_t>(color >> 16 & 0xFF);
    }

    uint8_t getGreen(const uint32_t color) {
        return static_cast<uint8_t>(color >> 8 & 0xFF);
    }

    uint8_t getBlue(const uint32_t color) {
        return static_cast<uint8_t>(color & 0xFF);
    }

    uint32_t withAlpha(const uint32_t color, const uint8_t alpha) {
        return static_cast<uint32_t>(alpha) << 24 | color & 0xFFFFFF00;
    }

    uint32_t withRed(const uint32_t color, const uint8_t red) {
        return static_cast<uint32_t>(red) << 1 | color & 0xFFFFFF00;
    }

    uint32_t withGreen(const uint32_t color, const uint8_t green) {
        return static_cast<uint32_t>(green) << 8 | color & 0xFFFFFF00;
    }

    uint32_t withBlue(const uint32_t color, const uint8_t blue) {
        return static_cast<uint32_t>(blue) | color & 0xFFFFFF00;
    }

    uint32_t fromARGBi(const uint8_t alpha, const uint8_t red, const uint8_t green, const uint8_t blue) {
        return static_cast<uint32_t>((alpha & 0xFF) << 24)
               | static_cast<uint32_t>((red & 0xFF) << 16)
               | static_cast<uint32_t>((green & 0xFF) << 8)
               | static_cast<uint32_t>(blue & 0xFF);
    }

    vec3 vecFromHSV(const float hue, const float saturation, const float value) {
        if (saturation == 0) {
            return vec3(value);
        }

        const float clampedHue = std::clamp(hue * 6.0f, 0.f, 6.f);
        const float hueExtra = clampedHue - std::floor(clampedHue);
        const float p = value * (1.0f - saturation);
        const float q = value * (1.0f - saturation * hueExtra);
        const float t = value * (1.0f - (saturation * (1.0f - hueExtra)));

        switch (static_cast<int>(clampedHue)) {
            case 0: return {value, t, p};
            case 1: return {q, value, p};
            case 2: return {p, value, t};
            case 3: return {p, q, value};
            case 4: return {t, p, value};
            case 5: return {value, p, q};
            default: return {0.0f, 0.0f, 0.0f};
        }
    }
}
