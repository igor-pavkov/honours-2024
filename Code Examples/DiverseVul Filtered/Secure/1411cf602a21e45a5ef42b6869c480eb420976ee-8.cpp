TEST(SnappyMessageCompressor, Overflow) {
    checkOverflow(std::make_unique<SnappyMessageCompressor>());
}