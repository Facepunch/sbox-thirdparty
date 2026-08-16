# sbox-thirdparty

Native dependencies for [sbox](https://github.com/Facepunch/sbox), built from source by
GitHub Actions and published as releases. sbox downloads them during its build.

## Libraries

Versions below are each workflow's default; what sbox actually consumes is pinned in its
`RemoteDeps.cs`.

| Library | Default | Purpose |
|---------|---------|---------|
| [dav1d](https://code.videolan.org/videolan/dav1d) | 1.5.3 | AV1 decoder |
| [SVT-AV1](https://gitlab.com/AOMediaCodec/SVT-AV1) | v4.1.0 | AV1 encoder |
| [libvpx](https://github.com/webmproject/libvpx) | v1.16.0 | VP9 codec |
| [libopus](https://github.com/xiph/opus) | v1.5.2 | Opus codec |
| [libwebp](https://chromium.googlesource.com/webm/libwebp) | v1.5.0 | WebP decode/encode |
| [libwebm](https://github.com/webmproject/libwebm) | 1.0.0.32 | WebM/MKV demuxer |
| [libcurl](https://curl.se/libcurl/) | 8.12.1 | HTTP/HTTPS streaming |
| [libyuv](https://chromium.googlesource.com/libyuv/libyuv/) | 4afb965 | RGBA/I420 conversion |
| [lame](https://lame.sourceforge.io/) | 3.100 | MP3 encoding (libmp3lame) |
| [SDL3](https://github.com/libsdl-org/SDL) | 2a623fd2 | Windowing, input, audio |
| [embree](https://github.com/embree/embree) | v3.13.5 | CPU raytracing |
| [etc2comp](https://github.com/google/etc2comp) | master | ETC2 compression |
| [ispc_texcomp](https://github.com/GameTechDev/ISPCTextureCompressor) | master | BC6H/ASTC/BC1/BC3 compression |
| [bc7enc](https://github.com/BinomialLLC/bc7e) | main | BC7 compression |
| [spirv-tools](https://github.com/KhronosGroup/glslang) | 14.3.0 | SPIR-V opt and remap, built via glslang |
| [slang](https://github.com/shader-slang/slang) | v2026.14 | Slang shader compiler |
| [dxc](https://github.com/microsoft/DirectXShaderCompiler) | v1.8.2505 | HLSL preprocessing |
| [openexr](https://github.com/AcademySoftwareFoundation/openexr) | v2.5.8 | IlmBase types for modeldoc |
| [alembic](https://github.com/alembic/alembic) | 1.7.16 | Alembic cache import |
| [oidn](https://github.com/RenderKit/oidn) | v1.4.3 | Lightmap denoising |
| [fidelityfx-fsr3](https://github.com/GPUOpen-LibrariesAndSDKs/FidelityFX-SDK) | v1.1.4 | FSR3 upscaling |
| [sentry-native](https://github.com/getsentry/sentry-native) | 0.11.3 | Crash reporting |
| [openxr-loader](https://github.com/KhronosGroup/OpenXR-SDK) | release-1.1.43 | OpenXR loader |
| [qt5](https://github.com/Facepunch/qt) | master | Tools UI, our fork |

`compressonator` also builds here (win64, linux64) but is not consumed by sbox yet.

Not built here: `fbx`, `nvidia`, `ovrlipsync` and `superluminal`. Vendor SDKs with no
buildable source, so they stay vendored in sbox.

## Platforms

| Identifier | Runner | Toolchain |
|------------|--------|-----------|
| `win64` | `windows-latest` | MSVC |
| `linuxsteamrt64` | `ubuntu-24.04` | GCC |
| `linuxsteamrtarm64` | `ubuntu-24.04-arm` | GCC |
| `osxarm64` | `macos-26` | Clang |

Most libraries build all four. The exceptions, and why:

| Library | Platforms | Blocker |
|---------|-----------|---------|
| `embree` | win64, linux64 | No aarch64 support before embree 4, which is a different API. |
| `ispc_texcomp`, `bc7enc` | win64, linux64 | The ISPC kernels have no NEON target. |
| `oidn` | win64 | Same, and 2.x reworks the device API `vrad3` uses. |
| `fidelityfx-fsr3` | win64 | AMD's SDK and shader compiler are Windows-only. |
| `sentry-native` | win64 | sbox only builds the crash handler on Windows. |
| `qt5` | win64, linux64 | The editor only ships on these two. |

## Triggering a build

Tag push or the Actions UI:

```bash
git tag dav1d-1.5.3 && git push origin dav1d-1.5.3
```

Each workflow publishes a release with `.zip` (Windows) and `.tar.gz` (Unix) archives
containing `include/` and `lib/`. Re-running an existing version replaces its assets rather
than failing.

## Notes

- **MSVC runtime.** Source 2 sets `RuntimeLibrary=MultiThreaded`, so static libraries build
  `/MT`. Libraries loaded as DLLs use `/MD` and own their own CRT.
- **MSVC version.** Artifacts use whatever MSVC the runner has, so linking them needs a
  local toolset at least as new. sbox pins one in `src/Directory.Build.props`.
- **Version floors.** `openexr`, `alembic` and `oidn` are held on their current major
  versions; the next one is a source change in sbox, not a dependency bump.
- **qt5** builds [Facepunch/qt](https://github.com/Facepunch/qt), never upstream Qt. Our
  patches are marked `s&box:`. It needs an LFS checkout and packages by hand, since Qt has
  to keep its `bin/` host tools and `plugins/` tree as a directory layout.
- **bc7enc** is bc7e plus a wrapper in `shims/` providing the entry points sbox resolves.

Anything more specific is commented in the workflow that needs it.
