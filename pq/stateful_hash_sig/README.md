# wolfSSL Post-Quantum Cryptography Stateful Hash-based Signatures Example

This directory contains:

- A simple example that uses wolfCrypt LMS/HSS hooks to sign and verify a message
  with configurable LMS/HSS parameters. Requires wolfssl with `--enable-lms=yes`
  and `--with-liblms=<path to hash-sigs install>`.

- An example that uses wolfCrypt XMSS/XMSS^MT hooks to sign and verify a message
  with a configurable XMSS/XMSS^MT parameter string. Requires wolfssl with `--enable-xmss=yes`
  and `--with-libxmss=<path to patched xmss-reference install>`.

# Prerequisites

The LMS/HSS sign verify example requires that hash-sigs has been built, and
wolfSSL has been built with LMS/HSS support enabled.  Please see Item 17
in the wolfSSL repo's INSTALL file.

https://github.com/wolfSSL/wolfssl/blob/master/INSTALL

The XMSS/XMSS^MT example requires that the xmss-reference repository has been
cloned, patched, and built. Please see item 20 in the wolfSSL repo's INSTALL file.

The patch to use is `0001-Patch-to-support-wolfSSL-xmss-reference-integration.patch` from this XMSS/XMSS^MT example.
This patch includes an addendum readme, `patch_readme.md`, that lists all changes made and explains their rationale.

# Building the LMS/HSS example

Configure the Makefile to point to your hash-sigs install:

```
HSS_INC  = <path to hss install>
```

```
HSS_LIB         = <path to hss_lib_thread.a>
```

Then build:

```
$ make lms_example
```

## Signing and Verifying a Message with LMS/HSS

This example will generate an LMS/HSS key pair with L=levels, H=height, and
W=Winternitz parameters, then sign and verify a given number of signatures.
It will also print the signature size, the total number of signatures, and
the public and private key lengths.

While LMS/HSS have small public and private keys, and fast signing and
verifying, the initial key generation can be quite slow and intensive,
especially for larger heights and Winternitz parameters.

LMS/HSS signature systems have a finite number of one-time signatures (OTS).
The number of available signatures is
  N = 2 ** (levels * height)

The supported parameter values are those in RFC8554:
- levels = {1..8}
- height = {5, 10, 15, 20, 25}
- Winternitz = {1, 2, 4, 8}

To see the help and usage, run the program without options:
```sh
$./lms_example
usage:
  ./lms_example <levels> <height> <winternitz> [num signatures]

examples:
  ./lms_example 1 5 1
  ./lms_example 3 5 4 100
  ./lms_example 2 10 2 0

description:
...
```

# Building the XMSS/XMSS^MT example

Configure the Makefile to point to your xmss install:

```
XMSS_INC = <path to patched xmss install>
```

```
XMSS_LIB = <path to xmss_lib.a or xmss_verify_lib.a>
```

Then build:

```
$ make xmss_example
```

Build the verify-only example with
```
$ make xmss_example_verifyonly
```

## Signing and Verifying a Message with XMSS/XMSS^MT

To see the help and usage, run the program without options:
```sh
$ ./xmss_example
usage:
  ./xmss_example <param string> [num signatures]

examples:
  ./xmss_example XMSSMT-SHA2_20/4_256 5
  ./xmss_example XMSSMT-SHA2_60/6_256 100
  ./xmss_example XMSS-SHA2_10_256 1023
```

The param string may be any name from this table:

```
         ----------------------------------------------------------
         | Name                     Oid         n   w  len  h   d  |
 XMSS:   | "XMSS-SHA2_10_256"       0x00000001  32  16  67  10  1  |
         | "XMSS-SHA2_16_256"       0x00000002  32  16  67  16  1  |
         | "XMSS-SHA2_20_256"       0x00000003  32  16  67  20  1  |
         |                                                         |
 XMSSMT: | "XMSSMT-SHA2_20/2_256"   0x00000001  32  16  67  20  2  |
         | "XMSSMT-SHA2_20/4_256"   0x00000002  32  16  67  20  4  |
         | "XMSSMT-SHA2_40/2_256"   0x00000003  32  16  67  40  2  |
         | "XMSSMT-SHA2_40/4_256"   0x00000004  32  16  67  40  4  |
         | "XMSSMT-SHA2_40/8_256"   0x00000005  32  16  67  40  8  |
         | "XMSSMT-SHA2_60/3_256"   0x00000006  32  16  67  60  3  |
         | "XMSSMT-SHA2_60/6_256"   0x00000007  32  16  67  60  6  |
         | "XMSSMT-SHA2_60/12_256"  0x00000008  32  16  67  60  12 |
         ----------------------------------------------------------
```

Here `n=32` is the number of bytes in the SHA256 has function, `w=16`
is the Winternitz parameter, `len=67` is the length of Winternitz chains,
`h` is the is the total height of the tree or hyper-tree, and `d` is the
number of levels in the hyper-tree. The number of signatures available
is `N = 2 ** (h)`.

The main contributor to key generation time is the ratio `h/d`.
Not surprisingly, be aware that `XMSS-SHA2_20_256`, and `XMSSMT-SHA2_60/3_256`, are particularly
CPU intensive because of the large number of hash operations involved, and
may take a long time. E.g. on an Intel i7 linux system these examples took
approximately 24 min, and 1 hour, respectively.

The other examples will be much faster.

An interesting facet of XMSS/XMSS^MT is that the private key format
and size is implementation specific. The wolfSSL XMSS/XMSS^MT hooks
feature uses the "fast" implementation from xmss-reference, which
has larger private key sizes.

For example:

```
$ ./xmss_example  "XMSSMT-SHA2_20/2_256" 200
using parameters: XMSSMT-SHA2_20/2_256
signature length: 4963
priv key length: 6002
pub key length: 68
...done!
signing and verifying 200 signatures...
...done!
finished

```

```
$ ./xmss_example  "XMSSMT-SHA2_40/4_256" 200
using parameters: XMSSMT-SHA2_40/4_256
signature length: 9893
priv key length: 15256
pub key length: 68
...done!
signing and verifying 200 signatures...
...done!
finished
```

## Using the verify-only XMSS/XMSS^MT example

The usage for the verify-only example is:
```
$ ./xmss_example_verifyonly
usage:
  ./xmss_example_verifyonly <param string> <pub file> <sig file> <msg file>

For simplicity message is assumed to be 32 bytes in size.

examples:
  ./xmss_example_verifyonly XMSSMT-SHA2_20/4_256 xmss_pub.key xmss_sig.bin msg.bin
  ./xmss_example_verifyonly XMSSMT-SHA2_60/6_256 xmss_pub.key xmss_sig.bin msg.bin
  ./xmss_example_verifyonly XMSS-SHA2_10_256 xmss_pub.key xmss_sig.bin msg.bin
```

An example:
```
$./xmss_example_verifyonly XMSSMT-SHA2_20/2_256 pk.bin sig.bin msg.bin
using parameters: XMSSMT-SHA2_20/2_256
pub:
0x00 0x00 0x00 0x01 0x2B 0xC1 0xA4 0x8D
0x32 0x4B 0x15 0xA8 0xF6 0xEA 0x04 0xC0
0x96 0x52 0x70 0x4D 0x4C 0x19 0x94 0xA4
0x2D 0x2E 0xDA 0x52 0xA4 0x5F 0xF7 0xA8
0x50 0x0A 0xE1 0xB3 0x77 0xD6 0x85 0x33
0xB0 0x8A 0x4F 0x9D 0x54 0x6B 0xE9 0xFE
0x4B 0x3B 0xCB 0x5A 0x7A 0x92 0x86 0x0C
0x4B 0x7F 0xBF 0x2E 0xA0 0x1F 0x47 0x6F
0xDB 0x35 0x1C 0x61
msg:
0xF1 0x91 0x9C 0xC0 0xA7 0xDC 0xEB 0xCE
0x38 0x96 0xEC 0x28 0x3A 0x9E 0xE1 0xA0
0xA3 0x2F 0x94 0x1E 0xE7 0xB0 0x56 0x15
0x54 0x8C 0x17 0xF4 0x65 0xFF 0xCB 0x08
signature length: 4963
pub key length: 68
Verify good!
finished
```
