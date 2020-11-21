## Setup

* git clone
* git submodule --init --update
* export PATH=$PATH:`pwd`/building/tools/gn
* gn gen out/Default
* ninja -C out/Default


### Codesign

* Get apple developer certificate
* Follow the steps below to create a keychain and install the cert

```sh
$ security create-keychain -p pass `pwd`/uv_test.keychain
$ security unlock-keychain -p pass `pwd`/uv_test.keychain
$ security import <developer-certificate.p12> -k `pwd`/uv_test.keychain -P <developer-certificate-key> -T /usr/bin/codesign
$ security set-key-partition-list -S apple-tool:,apple:,codesign: -s -k pass `pwd`/uv_test.keychain
```

* Codesign the generated uv_spawn executable

```sh
$ security find-identity -v `pwd`/uv_test.keychain
$ codesign --sign <identity-from-previous-step>  --force --keychain `pwd`/uv_test.keychain --entitlements `pwd`/app-entitlements.plist `pwd`/out/Default/uv_spawn
```

### Results

* Without MAP_JIT : `./out/Default/uv_spawn`

```
ls process launched with ID : 6115 and in : 1.44147 ms
ls process exited with status : 0 and in : 5.09804 ms

ls process launched with ID : 6116 and in : 1.321 ms
ls process exited with status : 0 and in : 4.88865 ms

ls process launched with ID : 6117 and in : 1.18874 ms
ls process exited with status : 0 and in : 4.4697 ms

ls process launched with ID : 6118 and in : 1.16868 ms
ls process exited with status : 0 and in : 4.45456 ms

ls process launched with ID : 6119 and in : 1.14751 ms
ls process exited with status : 0 and in : 4.61297 ms

ls process launched with ID : 6120 and in : 1.1754 ms
ls process exited with status : 0 and in : 4.50136 ms

ls process launched with ID : 6121 and in : 1.13239 ms
ls process exited with status : 0 and in : 4.40179 ms

ls process launched with ID : 6122 and in : 1.12352 ms
ls process exited with status : 0 and in : 4.73885 ms

ls process launched with ID : 6123 and in : 1.70062 ms
ls process exited with status : 0 and in : 5.73448 ms

ls process launched with ID : 6124 and in : 1.19278 ms
ls process exited with status : 0 and in : 4.56495 ms
```

* With MAP_JIT : `./out/Default/uv_spawn --use-jit`

```
ls process launched with ID : 6061 and in : 63.0682 ms
ls process exited with status : 0 and in : 66.821 ms

ls process launched with ID : 6062 and in : 62.8761 ms
ls process exited with status : 0 and in : 66.656 ms

ls process launched with ID : 6063 and in : 64.1878 ms
ls process exited with status : 0 and in : 67.8507 ms

ls process launched with ID : 6064 and in : 67.6894 ms
ls process exited with status : 0 and in : 71.3869 ms

ls process launched with ID : 6065 and in : 60.7432 ms
ls process exited with status : 0 and in : 64.5483 ms

ls process launched with ID : 6066 and in : 74.2739 ms
ls process exited with status : 0 and in : 78.0864 ms

ls process launched with ID : 6067 and in : 61.8672 ms
ls process exited with status : 0 and in : 65.5697 ms

ls process launched with ID : 6068 and in : 63.2874 ms
ls process exited with status : 0 and in : 66.9381 ms

ls process launched with ID : 6069 and in : 64.5125 ms
ls process exited with status : 0 and in : 68.6403 ms

ls process launched with ID : 6070 and in : 65.2823 ms
ls process exited with status : 0 and in : 69.0946 ms
```