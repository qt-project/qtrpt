CONFIG(debug, debug|release) {
    DEST_DIRECTORY = $$PWD/../bin/debug
}
CONFIG(release, debug|release) {
    DEST_DIRECTORY = $$PWD/../bin/release
}

