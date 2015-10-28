CONFIG(debug, debug|release) {
    DEST_DIRECTORY = $$top_builddir/bin/debug
}
CONFIG(release, debug|release) {
    DEST_DIRECTORY = $$top_builddir/bin/release
}

