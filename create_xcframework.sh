##
##  create_xcframework.sh.sh
##  AEC#
##
##  Created by Ariel Elkin on 26/04/2022.
##  Copyright © 2022 TGR1.618 Ltd. All rights reserved.
##

set -xeo pipefail

rm -rf macOS || true
rm -rf output || true
rm -rf AEC3.xcframework || true

sh generate-macos-project.sh

xcodebuild clean build \
    -project "macOS/AEC3.xcodeproj" \
    -target "AEC3" \
    | xcpretty

xcodebuild clean archive \
    -project "Framework/AEC3.xcodeproj" \
    -configuration "Release" \
    | xcpretty

xcodebuild -create-xcframework \
    -framework Framework/build/UninstalledProducts/macosx/AEC3.framework \
    -output AEC3.xcframework