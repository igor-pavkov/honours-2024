    Document getSpec() {
        return DOC("input" << DOC_ARRAY(DOC_ARRAY(1 << 2 << 4) << DOC_ARRAY(1 << 2 << 2 << 4)
                                                               << DOC_ARRAY(4 << 1 << 2)
                                                               << DOC_ARRAY(2 << 1 << 1 << 4))
                           << "expected"
                           << DOC("$setIntersection" << DOC_ARRAY(1 << 2 << 4) << "$setEquals"
                                                     << true
                                                     << "$setUnion"
                                                     << DOC_ARRAY(1 << 2 << 4))
                           << "error"
                           << DOC_ARRAY("$setIsSubset"_sd
                                        << "$setDifference"_sd));
    }