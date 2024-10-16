    Document getSpec() {
        return DOC("input" << DOC_ARRAY(vector<Value>() << DOC_ARRAY(1 << 2)) << "expected"
                           << DOC("$setIntersection" << vector<Value>() << "$setUnion"
                                                     << DOC_ARRAY(1 << 2)
                                                     << "$setIsSubset"
                                                     << true
                                                     << "$setEquals"
                                                     << false
                                                     << "$setDifference"
                                                     << vector<Value>()));
    }