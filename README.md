# sbox-thirdparty

Prebuilt static libraries for [sbox](https://github.com/Facepunch/sbox), built via GitHub Actions.

## Libraries

| Library | Version | Purpose |
|---------|---------|---------|
| [dav1d](https://code.videolan.org/videolan/dav1d) | 1.5.3 | AV1 decoder |
| [SVT-AV1](https://gitlab.com/AOMediaCodec/SVT-AV1) | v4.1.0 | AV1 encoder |
| [libvpx](https://github.com/webmproject/libvpx) | v1.16.0 | VP9 codec |
| [libopus](https://github.com/xiph/opus) | v1.5.2 | Opus codec |
| [libwebp](https://chromium.googlesource.com/webm/libwebp) | v1.5.0 | WebP decoder/encoder |
| [libwebm](https://github.com/webmproject/libwebm) | 1.0.0.32 | WebM/MKV demuxer (mkvparser) |
| [libcurl](https://curl.se/libcurl/) | 8.12.1 | HTTP/HTTPS streaming |
| [libyuv](https://chromium.googlesource.com/libyuv/libyuv/) | main | RGBA↔I420 color conversion (SIMD) |
| [SDL3](https://github.com/libsdl-org/SDL) | release-3.5.0 | Windowing / input / audio backend |
| [embree](https://github.com/embree/embree) | v3.13.5 | CPU raytracing (lightmap bakes, mapdoc picking) |
| [etc2comp](https://github.com/google/etc2comp) | master | ETC2 texture compression |
| [spirv-tools](https://github.com/KhronosGroup/glslang) | 14.3.0 | SPIR-V opt / remap (built via glslang) |
| [slang](https://github.com/shader-slang/slang) | v2025.6.3 | Slang shader compiler |
| [dxc](https://github.com/microsoft/DirectXShaderCompiler) | v1.8.2505 | HLSL → SPIR-V compiler |
| [ispc_texcomp](https://github.com/GameTechDev/ISPCTextureCompressor) | master | BC6H/BC7/ASTC/BC1/BC3 compression |
| [openexr](https://github.com/AcademySoftwareFoundation/openexr) | v2.5.8 | IlmBase/Imath types for modeldoc |
| [alembic](https://github.com/alembic/alembic) | 1.7.16 | Alembic cache import |
| [oidn](https://github.com/RenderKit/oidn) | v1.4.3 | Lightmap denoising (vrad3) |
| [qt5](https://github.com/Facepunch/qt) | master (Qt 5.15.2) | Tools UI — **our fork**, not upstream Qt |
| [vulkan-loader](https://github.com/KhronosGroup/Vulkan-Loader) | v1.4.313 | Vulkan loader (`vulkan-1` / `libvulkan`) |
| [openxr-loader](https://github.com/KhronosGroup/OpenXR-SDK) | release-1.1.43 | OpenXR loader |
| [uvatlas](https://github.com/microsoft/UVAtlas) | jun2025 | UV atlas generation |

Most libraries build all four platforms. The exceptions are not preferences — each is
blocked on something concrete:

| Library | Platforms | Why not all four |
|---------|-----------|------------------|
| `embree` | win64, linuxsteamrt64 | 3.13.x's vendored `sse2neon.h` does not compile on aarch64 with GCC 13+. Needs embree 4, which is a different API. |
| `oidn` | win64 | 1.x has no NEON path (ISPC targets are SSE/AVX only). Needs oidn 2.x, which reworks the device API `vrad3` uses. |
| `ispc_texcomp` | win64, linuxsteamrt64 | The vendored kernels are `sse2,avx` only; that snapshot has no NEON support to compile. |
| `qt5` | win64 | Our fork builds through `valve/fp_config.bat` + `jom`, which is Windows-only tooling. A Unix build means driving its `configure` instead. |
| `fidelityfx-fsr3` | win64 | AMD's SDK build and its shader-compiler tool are Windows-centric. |

Unblocking the first three means a major-version bump on the library, which changes sbox
source — so they are decisions, not matrix edits.

Deliberately **not** built here: `fbx` (Autodesk), `nvidia` (DLSS/Aftermath/NVAPI),
`ovrlipsync` and `superluminal`. These are vendor SDKs shipped as binaries with no
buildable source.

## Platforms

| Identifier | Runner | Toolchain |
|------------|--------|-----------|
| `win64` | `windows-latest` | MSVC |
| `linuxsteamrt64` | `ubuntu-24.04` | GCC |
| `linuxsteamrtarm64` | `ubuntu-24.04-arm` | GCC |
| `osxarm64` | `macos-26` | Clang |

## Triggering a build

Each library has its own workflow. Trigger via tag push or the Actions UI:

```bash
git tag dav1d-1.5.1 && git push origin dav1d-1.5.1
```

Or go to **Actions → Build \<library\> → Run workflow** and enter a version.

Each workflow publishes a GitHub Release with `.tar.gz` (Unix) / `.zip` (Windows) archives containing `include/` and `lib/`.

## Notes

- **libvpx on win64** uses MSYS2 (bash/make/perl) to drive the configure script but compiles with MSVC via `--target=x86_64-win64-vs17`, producing a `.lib`.
- **MSVC runtime.** Source 2 builds with `RuntimeLibrary=MultiThreaded` (see `engine/Tools/SboxBuild/Native/Defaults.cs`), so static libraries are built `/MT`. Libraries consumed as DLLs (`slang`, `ispc_texcomp`) use `/MD` instead — they own their own CRT.
- **Version floors that matter.** `openexr` must stay on the 2.5.x line and `alembic` on 1.7.x: 3.x replaces IlmBase with Imath and moves headers to `<Imath/...>`, which is a source change in `modeldoc_utils`, not a dependency bump. Likewise `oidn` must stay on 1.x, since 2.x reworks the device API that `vrad3` uses.
- **Versions bumped during the move.** sbox vendored `embree` 3.6.1 (2019), `openexr` 2.2.1 (2014) and `oidn` 1.1.0 (2019). None compile on current runners, so the defaults are the newest API-compatible release on the same line.
- **ispc_texcomp** has no CMake upstream; the workflow reimplements `Makefile.linux` / `ispc_texcomp.vcxproj` directly. The `crtoverride.cpp` that sbox carries is a Valve addition, inert behind `#if defined( USE_VCRT )`, and is deliberately not built here.
- **etc2comp** declares `cmake_minimum_required(VERSION 2.8)`, which CMake 4.x rejects, hence `-DCMAKE_POLICY_VERSION_MINIMUM=3.5`.
- **qt5 builds our fork**, [Facepunch/qt](https://github.com/Facepunch/qt) (Qt 5.15.2, based on Valve's build), never upstream Qt — the fork carries QDockWidget behaviour the tools UI relies on. It needs an LFS checkout, since the fork stores its build tooling (`jom`, `gnuwin32`) in LFS. It is also the one workflow that packages by hand rather than through `collect-package-upload`, because Qt has to keep its `bin/` host tools (`moc`, `rcc`, `uic`) and `plugins/` tree as a directory layout.
- **Loaders vs headers.** For `vulkan-loader` and `openxr-loader` only the loader binary is built here; the Vulkan/OpenXR headers, plus VMA and volk, are header-only and stay vendored in sbox.
