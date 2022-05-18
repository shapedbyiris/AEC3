rm -rf macos-products/base.xcframework || true
rm -rf macos-products/api.xcframework || true
rm -rf macos-products/AEC3.xcframework || true

xcodebuild -create-xcframework \
    -library static-libs/libapi.a \
    -headers macos-products/api-headers/ \
    -output macos-products/api.xcframework

# xcodebuild -create-xcframework \
#     -library static-libs/libAEC3.a \
#     -headers audio_processing \
#     -output macos-products/AEC3.xcframework

# xcodebuild -create-xcframework \
#     -library static-libs/libbase.a \
#     -headers base \
#     -output macos-products/base.xcframework

