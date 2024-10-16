    Document getSpec() {
        return DOC("input" << DOC_ARRAY(Value(BSONNULL) << DOC_ARRAY(1 << 2)) << "expected"
                           << DOC("$setIntersection" << BSONNULL << "$setUnion" << BSONNULL
                                                     << "$setDifference"
                                                     << BSONNULL)
                           << "error"
                           << DOC_ARRAY("$setEquals"_sd
                                        << "$setIsSubset"_sd));
    }