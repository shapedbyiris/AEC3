// swift-tools-version:5.5
import PackageDescription

let package = Package(
    name: "AEC3",
    platforms: [.macOS(.v10_10)],
    products: [
        .library(
            name: "AEC3",
            targets: ["AEC3"]
        )
    ],
    targets: [
        .binaryTarget(
            name: "AEC3",
            path: "AEC3.xcframework"
        )
    ]
)
