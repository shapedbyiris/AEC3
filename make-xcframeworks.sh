set -xeuo pipefail
rm -rf macos-products output || true
rm -rf macOS/build UninstalledProducts || true

sh generate-macos-project.sh

# copy headers:
mkdir -p ./macos-products/headers
rsync -avhm --include='api' --include='*.h' --exclude='*' ./ ./macos-products/headers/api/
rsync -avhm --include='*/' --include='*.h' --exclude='*' ./base/rtc_base ./macos-products/headers/api/
rsync -avhm --include='*/' --include='*.h' --exclude='*' ./base/abseil/absl ./macos-products/headers/api/
rsync -avhm --include='*/' --include='*.h' --exclude='*' ./audio_processing ./macos-products/headers/audio_processing

# Make debug versions:
xcodebuild archive \
    -project macOS/AEC3.xcodeproj \
    -target api \
    -sdk macosx \
    -configuration Debug \
    -destination "generic/platform=macOS" \
    GCC_OPTIMIZATION_LEVEL=3 \
    OTHER_CPLUSPLUSFLAGS="-DNDEBUG '-std=gnu++14'"

xcodebuild -create-xcframework \
    -library output/Darwin/Debug_x64/libapi.a \
    -headers macos-products/headers/api \
    -output macos-products/debug/api.xcframework

xcodebuild -create-xcframework \
    -library output/Darwin/Debug_x64/libbase.a \
    -output macos-products/debug/base.xcframework

xcodebuild -create-xcframework \
    -library output/Darwin/Debug_x64/libAEC3.a \
    -headers macos-products/headers/audio_processing \
    -output macos-products/debug/AEC3.xcframework


# Make release versions:
xcodebuild archive \
    -project macOS/AEC3.xcodeproj \
    -target api \
    -sdk macosx \
    -configuration Release \
    -destination "generic/platform=macOS" \
    GCC_OPTIMIZATION_LEVEL=3 \
    OTHER_CPLUSPLUSFLAGS="-DNDEBUG '-std=gnu++14'"


xcodebuild -create-xcframework \
    -library output/Darwin/Release_x64/libapi.a \
    -headers macos-products/headers/api \
    -output macos-products/release/api.xcframework

xcodebuild -create-xcframework \
    -library output/Darwin/Release_x64/libbase.a \
    -output macos-products/release/base.xcframework

xcodebuild -create-xcframework \
    -library output/Darwin/Release_x64/libAEC3.a \
    -headers macos-products/headers/audio_processing \
    -output macos-products/release/AEC3.xcframework