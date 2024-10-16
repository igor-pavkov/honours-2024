    Document getSpec() {
        return DOC("input" << DOC_ARRAY(DOC_ARRAY(1 << 2) << DOC_ARRAY(8 << 2 << 4)) << "expected"
                           << DOC("$setIsSubset" << false << "$setEquals" << false
                                                 << "$setIntersection"
                                                 << DOC_ARRAY(2)
                                                 << "$setUnion"
                                                 << DOC_ARRAY(1 << 2 << 4 << 8)
                                                 << "$setDifference"
                                                 << DOC_ARRAY(1)));
    }