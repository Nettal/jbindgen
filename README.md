jbindgen
====

### A project for generating Java bindings from c code

Usage
----
See the `add_jbindgen_test` function in [CMakeLists.txt](./CMakeLists.txt)

Dependencies
----

- `libclang` 17 or above
- `cmake`
- `g++` or `clang++`

```shell
sudo apt install libclang-17-dev cmake g++
```

Test & Examples
----
Generate the vulkan bindings, you need to install the `libvulkan-dev` package first

```shell
cmake -B build
cmake --build build -t jbindgen -j
```

Generate the [assimp](https://github.com/assimp/assimp) bindings (need network to download assimp source form GitHub)

```shell
cmake -B build
cmake --build build -t assimp -j 
```

Generate the [miniaudio](https://github.com/mackron/miniaudio) bindings (need network to download assimp source form
GitHub)

```shell
cmake -B build
cmake --build build -t miniaudio -j 
```