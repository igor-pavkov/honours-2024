    Document getSpec() {
        return DOC("input" << DOC_ARRAY(DOC_ARRAY(1) << DOC_ARRAY(1 << 2)) << "expected"
                           << DOC("$setIsSubset" << true << "$setEquals" << false
                                                 << "$setIntersection"
                                                 << DOC_ARRAY(1)
                                                 << "$setUnion"
                                                 << DOC_ARRAY(1 << 2)
                                                 << "$setDifference"
                                                 << vector<Value>()));
    }