# Third-Party Licenses

This file aggregates the third-party licenses and copyright notices that apply to
the **McNopper/OpenVG examples** specifically - i.e. what this repository adds on
top of its underlying libraries.

The examples themselves are distributed under the **MIT License** - see
[`LICENSE`](LICENSE).

## Underlying libraries

### GLUS

These examples are built on top of
[GLUS](https://github.com/McNopper/GLUS) (MIT), by Norbert Nopper, in its
OpenVG (`OpenGL=VG`) flavor. The full license texts and copyright notices are
maintained in the GLUS repository:

https://github.com/McNopper/GLUS/blob/main/THIRD-PARTY.md

### OpenVG_Library (based on the Khronos OpenVG 1.1 sources)

When the platform does not provide its own EGL/OpenVG pair, the examples build
and link [OpenVG_Library](https://github.com/McNopper/OpenVG_Library), a
software OpenVG 1.1 implementation whose code derives from the
**OpenVG 1.1 Reference Implementation** by **The Khronos Group**
(https://www.khronos.org/openvg/).

- **Upstream (implementation):** https://www.khronos.org/openvg/
- **Upstream (library):** https://github.com/McNopper/OpenVG_Library
- **License:** the Khronos permissive license shipped with the reference
  implementation, plus the MIT license for the surrounding library and glue.

The complete Khronos license text, copyright notices and provenance are
maintained in the OpenVG_Library repository:

https://github.com/McNopper/OpenVG_Library/blob/main/THIRD-PARTY.md

Comply with them as described in that file.

## Platform-provided EGL and OpenVG

On platforms that provide an EGL with an OpenVG client API and an OpenVG
implementation (for example embedded GPU drivers), the examples link those
system libraries instead and OpenVG_Library is not used.

## Bundled example data

### Khronos tiger path data (`Example03_VG`)

The path data in `Example03_VG/src/tiger.c` originates from the tiger sample of
the **OpenVG 1.1 Reference Implementation** by The Khronos Group
(https://www.khronos.org/openvg/). Per the reference implementation's notes,
the tiger path data was extracted from the `tiger.eps` example file distributed
with **GPL Ghostscript**; see the license notes in
OpenVG_Library's `THIRD-PARTY.md`.
