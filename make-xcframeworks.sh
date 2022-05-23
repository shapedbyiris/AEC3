# DEVELOPERID_CERTIFICATE_NAME="Developer ID Application: TGR1.618 limited (5RMZ3ZU3DS)"
# codesign --sign "${DEVELOPERID_CERTIFICATE_NAME}" static-libs/libapi.a --force --deep  --timestamp --options runtime
# codesign --sign "${DEVELOPERID_CERTIFICATE_NAME}" static-libs/libAEC3.a --force --deep  --timestamp --options runtime
# codesign --sign "${DEVELOPERID_CERTIFICATE_NAME}" static-libs/libbase.a --force --deep  --timestamp --options runtime

rm -rf macos-products/headers || true
rm -rf macos-products/base.xcframework || true
rm -rf macos-products/api.xcframework || true
rm -rf macos-products/AEC3.xcframework || true

mkdir ./macos-products/headers
rsync -avhm --include='api' --include='*.h' --exclude='*' ./ ./macos-products/headers/api/
rsync -avhm --include='*/' --include='*.h' --exclude='*' ./base/rtc_base ./macos-products/headers/api/
rsync -avhm --include='*/' --include='*.h' --exclude='*' ./base/abseil/absl ./macos-products/headers/api/

rsync -avhm --include='*/' --include='*.h' --exclude='*' ./audio_processing ./macos-products/headers/audio_processing

xcodebuild -create-xcframework \
    -library static-libs/libapi.a \
    -headers macos-products/headers/api \
    -output macos-products/api.xcframework

xcodebuild -create-xcframework \
    -library static-libs/libbase.a \
    -output macos-products/base.xcframework

xcodebuild -create-xcframework \
    -library static-libs/libAEC3.a \
    -headers macos-products/headers/audio_processing \
    -output macos-products/AEC3.xcframework


