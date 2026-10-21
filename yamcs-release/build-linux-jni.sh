#!/bin/bash
#
# Builds the RocksDB JNI library for Linux inside an Ubuntu 20.04 container:
#   x86_64  -> java/target/librocksdbjni-linux64.so
#   aarch64 -> java/target/librocksdbjni-linux-aarch64.so
#
# Building in the container keeps the library loadable on older systems (glibc 2.31 baseline)
# and avoids compiling RocksDB 9.4 with compilers newer than it supports (e.g. GCC 15).
#
# CPU compatibility:
#   x86_64  : -march=core2 (PORTABLE=core2), so it runs on old CPUs without SSE4/AVX.
#             zstd still contains BMI2 code paths, but it selects them at runtime based on cpuid.
#   aarch64 : no -march, i.e. the armv8-a baseline (PORTABLE unset adds nothing on aarch64).
#
# Used by .github/workflows/rocksdbjni.yml; can also be run locally on an x86_64 or aarch64
# machine with docker:
#   yamcs-release/build-linux-jni.sh [jobs]

set -euo pipefail

cd "$(dirname "$0")/.."

JOBS=${1:-$(nproc)}
IMAGE=${IMAGE:-ubuntu:20.04}

case "$(uname -m)" in
  x86_64) PORTABLE=core2 ;;
  aarch64) PORTABLE= ;;
  *) echo "unsupported architecture $(uname -m)" >&2; exit 1 ;;
esac

# the upstream download sites do not keep old versions (e.g. zlib.net); use the RocksDB mirror
DEPS=https://rocksdb-deps.s3.us-west-2.amazonaws.com/pkgs
: "${ZLIB_DOWNLOAD_BASE:=$DEPS/zlib}"
: "${BZIP2_DOWNLOAD_BASE:=$DEPS/bzip2}"
: "${SNAPPY_DOWNLOAD_BASE:=$DEPS/snappy}"
: "${LZ4_DOWNLOAD_BASE:=$DEPS/lz4}"
: "${ZSTD_DOWNLOAD_BASE:=$DEPS/zstd}"
export ZLIB_DOWNLOAD_BASE BZIP2_DOWNLOAD_BASE SNAPPY_DOWNLOAD_BASE LZ4_DOWNLOAD_BASE ZSTD_DOWNLOAD_BASE

docker run --rm -v "$PWD":/rocksdb -w /rocksdb \
  -e PORTABLE="$PORTABLE" -e JOBS="$JOBS" \
  -e ZLIB_DOWNLOAD_BASE -e BZIP2_DOWNLOAD_BASE -e SNAPPY_DOWNLOAD_BASE \
  -e LZ4_DOWNLOAD_BASE -e ZSTD_DOWNLOAD_BASE \
  -e HOST_UID="$(id -u)" -e HOST_GID="$(id -g)" \
  "$IMAGE" bash -c '
    set -euo pipefail
    export DEBIAN_FRONTEND=noninteractive
    apt-get update -q
    apt-get install -y -q --no-install-recommends build-essential cmake curl ca-certificates \
        openjdk-11-jdk-headless perl python3
    export JAVA_HOME=/usr/lib/jvm/java-11-openjdk-$(dpkg --print-architecture)
    gcc --version | head -1
    # build as the calling user so the checkout does not end up owned by root
    setpriv --reuid="$HOST_UID" --regid="$HOST_GID" --clear-groups env HOME=/tmp \
        make V=1 J="$JOBS" -j"$JOBS" rocksdbjavastatic
  '

ls -l java/target/librocksdbjni-linux*.so
