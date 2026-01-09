# MCpp
A simple Minecraft Clone in C++ using Vulkan.

## Building
You'll need:
1. [VulkanSDK](https://vulkan.lunarg.com/sdk/home)
2. Anything that can build a Cmake Project

that's it! Everything else is (should be) automatically downloaded by CMake.

for Debug builds of the Client you'll also need "vulkan validation layers",
though you can build without them by building RelWithDebInfo, Release or anything else that isn't Debug.

### Download "vulkan validation layers"
(On Windows these are included in the VulkanSDK)

Debian based:
```shell
sudo apt install vulkan-validationlayers
```

Arch based:
```shell
sudo pacman -S vulkan-validation-layers
```

Fedora based:
```shell
sudo dnf install vulkan-validation-layers
```

## Licence
This software is licensed under the GNU Public License version 3. In short: This software is free, you may run the software freely, create modified versions,
distribute this software and distribute modified versions, as long as the modified software too has a free software license. The full license can be found in the `LICENSE.txt` file.

### Dependency Licences
- spdlog: [MIT License](https://github.com/gabime/spdlog/blob/v1.x/LICENSE)
- glm: [The Happy Bunny License (Modified MIT License)](https://github.com/g-truc/glm/blob/master/copying.txt)
- asio: [Boost Software License, Version 1.0](https://github.com/chriskohlhoff/asio/blob/master/LICENSE_1_0.txt)
- glfw: [zlib License](https://github.com/glfw/glfw/blob/master/LICENSE.md)
- stb: [MIT License or Public Domain (aka: unlicense)](https://github.com/nothings/stb/blob/master/LICENSE)
- imgui: [MIT License](https://github.com/ocornut/imgui/blob/master/LICENSE.txt)
- shaderc: [Apache-2.0 License](https://github.com/google/shaderc/blob/main/LICENSE)
- Vulkan Specifications: [Apache-2.0 and MIT and CC-BY-4.0 and LicenseRef-MPLUS](https://github.com/KhronosGroup/Vulkan-Docs/blob/main/LICENSE.adoc)
