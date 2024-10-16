TEST(ZstdMessageCompressor, Overflow) {
    checkOverflow(std::make_unique<ZstdMessageCompressor>());
}