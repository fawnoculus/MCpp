#pragma once

#include <algorithm>
#include <cmath>
#include <stdfloat>

#include "glm/vec3.hpp"
#include "glm/vec4.hpp"

using std::float32_t;
using glm::vec3, glm::vec4;

namespace Colors {
    uint8_t getAlpha(uint32_t color);

    uint8_t getRed(uint32_t color);

    uint8_t getGreen(uint32_t color);

    uint8_t getBlue(uint32_t color);

    uint32_t withAlpha(uint32_t color, uint8_t alpha);

    uint32_t withRed(uint32_t color, uint8_t red);

    uint32_t withGreen(uint32_t color, uint8_t green);

    uint32_t withBlue(uint32_t color, uint8_t blue);

    uint32_t fromARGBi(uint8_t alpha, uint8_t red, uint8_t green, uint8_t blue);

    constexpr uint32_t fromRGBi(const uint8_t red, const uint8_t green, const uint8_t blue) {
        return fromARGBi(255, red, green, blue);
    }

    constexpr uint32_t fromARGBf(const float32_t alpha, const float32_t red, const float32_t green,
                                 const float32_t blue) {
        return fromARGBi(
            std::clamp(static_cast<int32_t>(alpha * 255), 0, 255),
            std::clamp(static_cast<int32_t>(red * 255), 0, 255),
            std::clamp(static_cast<int32_t>(green * 255), 0, 255),
            std::clamp(static_cast<int32_t>(blue * 255), 0, 255)
        );
    }

    constexpr uint32_t fromRGBf(const float32_t red, const float32_t green, const float32_t blue) {
        return fromARGBf(1.f, red, green, blue);
    }

    constexpr uint32_t fromRGBv(const vec3 color) {
        return fromRGBf(color.r, color.g, color.b);
    }

    constexpr uint32_t fromARGBv(const vec4 color) {
        return fromARGBf(color.a, color.r, color.g, color.b);
    }

    constexpr vec4 toARGBv(const uint32_t color) {
        return {
            static_cast<float32_t>(getAlpha(color)) / 255.f,
            static_cast<float32_t>(getRed(color)) / 255.f,
            static_cast<float32_t>(getGreen(color)) / 255.f,
            static_cast<float32_t>(getBlue(color)) / 255.f
        };
    }

    constexpr vec3 toRGBv(const uint32_t color) {
        return {
            static_cast<float32_t>(getRed(color)) / 255.f,
            static_cast<float32_t>(getGreen(color)) / 255.f,
            static_cast<float32_t>(getBlue(color)) / 255.f
        };
    }

    vec3 vecFromHSV(float hue, float saturation, float value);

    constexpr uint32_t fromHSV(const float hue, const float saturation, const float value) {
        return fromRGBv(vecFromHSV(hue, saturation, value));
    }
}
