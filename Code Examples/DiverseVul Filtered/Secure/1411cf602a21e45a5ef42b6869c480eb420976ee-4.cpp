void MessageCompressorManager::clientFinish(const BSONObj& input) {
    auto elem = input.getField("compression");
    LOG(3) << "Finishing client-side compression negotiation";

    // We've just called clientBegin, so the list of compressors should be empty.
    invariant(_negotiated.empty());

    // If the server didn't send back a "compression" array, then we assume compression is not
    // supported by this server and just return. We've already disabled compression by clearing
    // out the _negotiated array above.
    if (elem.eoo()) {
        LOG(3) << "No compression algorithms were sent from the server. "
               << "This connection will be uncompressed";
        return;
    }

    LOG(3) << "Received message compressors from server";
    for (const auto& e : elem.Obj()) {
        auto algoName = e.checkAndGetStringData();
        auto ret = _registry->getCompressor(algoName);
        LOG(3) << "Adding compressor " << ret->getName();
        _negotiated.push_back(ret);
    }
}