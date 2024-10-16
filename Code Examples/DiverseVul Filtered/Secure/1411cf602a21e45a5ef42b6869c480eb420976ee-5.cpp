void checkOverflow(std::unique_ptr<MessageCompressorBase> compressor) {
    // This is our test data that we're going to try to compress/decompress into a buffer that's
    // way too small.
    const std::string data =
        "We embrace reality. We apply high-quality thinking and rigor."
        "We have courage in our convictions but work hard to ensure biases "
        "or personal beliefs do not get in the way of finding the best solution.";
    ConstDataRange input(data.data(), data.size());

    // This is our tiny buffer that should cause an error.
    std::array<char, 16> smallBuffer;
    DataRange smallOutput(smallBuffer.data(), smallBuffer.size());

    // This is a normal sized buffer that we can store a compressed version of our test data safely
    std::vector<char> normalBuffer;
    normalBuffer.resize(compressor->getMaxCompressedSize(data.size()));
    auto sws = compressor->compressData(input, DataRange(normalBuffer.data(), normalBuffer.size()));
    ASSERT_OK(sws);
    DataRange normalRange = DataRange(normalBuffer.data(), sws.getValue());

    // Check that compressing the test data into a small buffer fails
    ASSERT_NOT_OK(compressor->compressData(input, smallOutput));

    // Check that decompressing compressed test data into a small buffer fails
    ASSERT_NOT_OK(compressor->decompressData(normalRange, smallOutput));

    // Check that decompressing a valid buffer that's missing data doesn't overflow the
    // source buffer.
    std::vector<char> scratch;
    scratch.resize(data.size());
    ConstDataRange tooSmallRange(normalBuffer.data(), normalBuffer.size() / 2);
    ASSERT_NOT_OK(
        compressor->decompressData(tooSmallRange, DataRange(scratch.data(), scratch.size())));
}