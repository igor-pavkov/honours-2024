TEST(NoopMessageCompressor, Fidelity) {
    auto testMessage = buildMessage();
    checkFidelity(testMessage, std::make_unique<NoopMessageCompressor>());
}