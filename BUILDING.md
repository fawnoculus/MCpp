# Dependencies
[VulkanSDK](https://vulkan.lunarg.com/sdk/home), that's it!
Everything else is (should be) automatically downloaded by CMake.

for Debug builds specifically you'll also need "vulkan validation layers",
though you can build without them by building RelWithDebInfo, Release or anything that isn't Debug

Debian based:
```shell
sudo apt install vulkan-validationlayers
```

Arch based:
```shell
sudo pacman -S vulkan-validation-layers
```
