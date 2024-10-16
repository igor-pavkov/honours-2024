void MessageCompressorManager::serverNegotiate(const BSONObj& input, BSONObjBuilder* output) {
    LOG(3) << "Starting server-side compression negotiation";

    auto elem = input.getField("compression");
    // If the "compression" field is missing, then this isMaster request is requesting information
    // rather than doing a negotiation
    if (elem.eoo()) {
        // If we haven't negotiated any compressors yet, then don't append anything to the
        // output - this makes this compatible with older versions of MongoDB that don't
        // support compression.
        if (_negotiated.size() > 0) {
            BSONArrayBuilder sub(output->subarrayStart("compression"));
            for (const auto& algo : _negotiated) {
                sub.append(algo->getName());
            }
            sub.doneFast();
        } else {
            LOG(3) << "Compression negotiation not requested by client";
        }
        return;
    }

    // If compression has already been negotiated, then this is a renegotiation, so we should
    // reset the state of the manager.
    _negotiated.clear();

    // First we go through all the compressor names that the client has requested support for
    BSONObj theirObj = elem.Obj();

    if (!theirObj.nFields()) {
        LOG(3) << "No compressors provided";
        return;
    }

    for (const auto& elem : theirObj) {
        MessageCompressorBase* cur;
        auto curName = elem.checkAndGetStringData();
        // If the MessageCompressorRegistry knows about a compressor with that name, then it is
        // valid and we add it to our list of negotiated compressors.
        if ((cur = _registry->getCompressor(curName))) {
            LOG(3) << cur->getName() << " is supported";
            _negotiated.push_back(cur);
        } else {  // Otherwise the compressor is not supported and we skip over it.
            LOG(3) << curName << " is not supported";
        }
    }

    // If the number of compressors that were eventually negotiated is greater than 0, then
    // we should send that back to the client.
    if (_negotiated.size() > 0) {
        BSONArrayBuilder sub(output->subarrayStart("compression"));
        for (auto algo : _negotiated) {
            sub.append(algo->getName());
        }
        sub.doneFast();
    } else {
        LOG(3) << "Could not agree on compressor to use";
    }
}