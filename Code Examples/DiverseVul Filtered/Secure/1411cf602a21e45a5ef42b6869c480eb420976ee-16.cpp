TEST(SnappyMessageCompressor, Fidelity) {
    auto testMessage = buildMessage();
    checkFidelity(testMessage, std::make_unique<SnappyMessageCompressor>());
}