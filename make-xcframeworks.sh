set -xeuo pipefail
rm -rf macos-products output || true
rm -rf macOS/build UninstalledProducts || true

sh generate-macos-project.sh

# Make debug versions:

xcodebuild build \
    -project macOS/AEC3.xcodeproj \
    -target api \
    -sdk macosx \
    -configuration Debug \
    -destination "generic/platform=macOS" \
    GCC_OPTIMIZATION_LEVEL=3 \
    OTHER_CPLUSPLUSFLAGS="-DNDEBUG '-std=gnu++14'" \
    | xcpretty

mkdir -p ./macos-products/headers
rsync -avhm --include='api' --include='*.h' --exclude='*' ./ ./macos-products/headers/api/
rsync -avhm --include='*/' --include='*.h' --exclude='*' ./base/rtc_base ./macos-products/headers/api/
rsync -avhm --include='*/' --include='*.h' --exclude='*' ./base/abseil/absl ./macos-products/headers/api/
rsync -avhm --include='*/' --include='*.h' --exclude='*' ./audio_processing ./macos-products/headers/audio_processing

xcodebuild -create-xcframework \
    -library output/Darwin/Debug_x64/libapi.a \
    -headers macos-products/headers/api \
    -output macos-products/api.xcframework

xcodebuild -create-xcframework \
    -library output/Darwin/Debug_x64/libbase.a \
    -output macos-products/base.xcframework

xcodebuild -create-xcframework \
    -library output/Darwin/Debug_x64/libAEC3.a \
    -headers macos-products/headers/audio_processing \
    -output macos-products/AEC3.xcframework


# Make release versions:

xcodebuild build \
    -project macOS/AEC3.xcodeproj \
    -target api \
    -sdk macosx \
    -configuration Release \
    -destination "generic/platform=macOS" \
    GCC_OPTIMIZATION_LEVEL=3 \
    OTHER_CPLUSPLUSFLAGS="-DNDEBUG '-std=gnu++14'" \
    | xcpretty


xcodebuild -create-xcframework \
    -library output/Darwin/Release_x64/libapi.a \
    -headers macos-products/headers/api \
    -output macos-products/api.xcframework

xcodebuild -create-xcframework \
    -library output/Darwin/Release_x64/libbase.a \
    -output macos-products/base.xcframework

xcodebuild -create-xcframework \
    -library output/Darwin/Release_x64/libAEC3.a \
    -headers macos-products/headers/audio_processing \
    -output macos-products/AEC3.xcframework