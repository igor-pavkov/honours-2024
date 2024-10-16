    Document getSpec() {
        return DOC("input" << DOC_ARRAY(DOC_ARRAY(1 << 2) << DOC_ARRAY(1 << 2)) << "expected"
                           << DOC("$setIsSubset" << true << "$setEquals" << true
                                                 << "$setIntersection"
                                                 << DOC_ARRAY(1 << 2)
                                                 << "$setUnion"
                                                 << DOC_ARRAY(1 << 2)
                                                 << "$setDifference"
                                                 << vector<Value>()));
    }