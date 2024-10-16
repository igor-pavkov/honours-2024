    Document getSpec() {
        return DOC("input" << DOC_ARRAY(DOC_ARRAY(1 << 2) << vector<Value>()) << "expected"
                           << DOC("$setIntersection" << vector<Value>() << "$setUnion"
                                                     << DOC_ARRAY(1 << 2)
                                                     << "$setIsSubset"
                                                     << false
                                                     << "$setEquals"
                                                     << false
                                                     << "$setDifference"
                                                     << DOC_ARRAY(1 << 2)));
    }