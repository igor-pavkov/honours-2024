TEST(ZlibMessageCompressor, Fidelity) {
    auto testMessage = buildMessage();
    checkFidelity(testMessage, std::make_unique<ZlibMessageCompressor>());
}