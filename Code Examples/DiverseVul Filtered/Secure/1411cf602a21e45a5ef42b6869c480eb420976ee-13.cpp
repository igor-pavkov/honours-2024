TEST(ZstdMessageCompressor, Fidelity) {
    auto testMessage = buildMessage();
    checkFidelity(testMessage, std::make_unique<ZstdMessageCompressor>());
}