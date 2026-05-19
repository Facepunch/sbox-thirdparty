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
| [GameNetworkingSockets](https://github.com/ValveSoftware/GameNetworkingSockets) | v1.5.1 | Valve's reliable UDP networking library |

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
