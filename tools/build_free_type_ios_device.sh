./configure --prefix=/Users/jukka/Desktop/freetype-2.6.2/build-armv7 --host=arm-apple-darwin --enable-static=yes --enable-shared=no \
CPPFLAGS="-arch armv7 -fpascal-strings -Os -fmessage-length=0 -fvisibility=hidden -miphoneos-version-min=4.3 -I/Applications/Xcode.app/Contents/Developer/Platforms/iPhoneOS.platform/Developer/SDKs/iPhoneOS.sdk/usr/includlibxml2 -isysroot /Applications/Xcode.app/Contents/Developer/Platforms/iPhoneOS.platform/Developer/SDKs/iPhoneOS.sdk" \
CC=`xcrun -sdk iphoneos -find clang` \
CFLAGS="-arch armv7 -fpascal-strings -Os -fmessage-length=0 -fvisibility=hidden -miphoneos-version-min=4.3 -isysroot /Applications/Xcode.app/Contents/Developer/Platforms/iPhoneOS.platform/Developer/SDKs/iPhoneOS.sdk" \
LD=`xcrun -sdk iphoneos -find ld` \
LDFLAGS="-arch armv7 -isysroot /Applications/Xcode.app/Contents/Developer/Platforms/iPhoneOS.platform/Developer/SDKs/iPhoneOS.sdk -miphoneos-version-min=4.3" \
AR=`xcrun -sdk iphoneos -find ar`

./configure --prefix=/Users/jukka/Desktop/freetype-2.6.2/build-armv7s --host=arm-apple-darwin --enable-static=yes --enable-shared=no \
CPPFLAGS="-arch armv7s -fpascal-strings -Os -fmessage-length=0 -fvisibility=hidden -miphoneos-version-min=4.3 -I/Applications/Xcode.app/Contents/Developer/Platforms/iPhoneOS.platform/Developer/SDKs/iPhoneOS.sdk/usr/includlibxml2 -isysroot /Applications/Xcode.app/Contents/Developer/Platforms/iPhoneOS.platform/Developer/SDKs/iPhoneOS.sdk" \
CC=`xcrun -sdk iphoneos -find clang` \
CFLAGS="-arch armv7s -fpascal-strings -Os -fmessage-length=0 -fvisibility=hidden -miphoneos-version-min=4.3 -isysroot /Applications/Xcode.app/Contents/Developer/Platforms/iPhoneOS.platform/Developer/SDKs/iPhoneOS.sdk" \
LD=`xcrun -sdk iphoneos -find ld` \
LDFLAGS="-arch armv7s -isysroot /Applications/Xcode.app/Contents/Developer/Platforms/iPhoneOS.platform/Developer/SDKs/iPhoneOS.sdk -miphoneos-version-min=4.3" \
AR=`xcrun -sdk iphoneos -find ar`

./configure --prefix=/Users/jukka/Desktop/freetype-2.6.2/build-arm64 --host=arm-apple-darwin --enable-static=yes --enable-shared=no \
CPPFLAGS="-arch arm64 -fpascal-strings -Os -fmessage-length=0 -fvisibility=hidden -miphoneos-version-min=4.3 -I/Applications/Xcode.app/Contents/Developer/Platforms/iPhoneOS.platform/Developer/SDKs/iPhoneOS.sdk/usr/includlibxml2 -isysroot /Applications/Xcode.app/Contents/Developer/Platforms/iPhoneOS.platform/Developer/SDKs/iPhoneOS.sdk" \
CC=`xcrun -sdk iphoneos -find clang` \
CFLAGS="-arch arm64 -fpascal-strings -Os -fmessage-length=0 -fvisibility=hidden -miphoneos-version-min=4.3 -isysroot /Applications/Xcode.app/Contents/Developer/Platforms/iPhoneOS.platform/Developer/SDKs/iPhoneOS.sdk" \
LD=`xcrun -sdk iphoneos -find ld` \
LDFLAGS="-arch arm64 -isysroot /Applications/Xcode.app/Contents/Developer/Platforms/iPhoneOS.platform/Developer/SDKs/iPhoneOS.sdk -miphoneos-version-min=4.3" \
AR=`xcrun -sdk iphoneos -find ar`


xcrun -sdk iphoneos lipo -create -arch armv7 build-armv7/lib/libfreetype.a -arch armv7s build-armv7s/lib/libfreetype.a -arch arm64 build-arm64/lib/libfreetype.a  -output libfreetype_universal.a
rm -rf build-armv7s
mv -f libfreetype_universal.a build-armv7/lib/libfreetype.a
mv build-armv7 build