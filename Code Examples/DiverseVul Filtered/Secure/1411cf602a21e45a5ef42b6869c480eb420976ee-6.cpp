TEST(ZlibMessageCompressor, Overflow) {
    checkOverflow(std::make_unique<ZlibMessageCompressor>());
}