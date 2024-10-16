MessageCompressorRegistry buildRegistry() {
    MessageCompressorRegistry ret;
    auto compressor = std::make_unique<NoopMessageCompressor>();

    std::vector<std::string> compressorList = {compressor->getName()};
    ret.setSupportedCompressors(std::move(compressorList));
    ret.registerImplementation(std::move(compressor));
    ret.finalizeSupportedCompressors().transitional_ignore();

    return ret;
}