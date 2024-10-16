    Document getSpec() {
        return DOC(
            "input" << DOC_ARRAY(DOC_ARRAY(8 << 3) << DOC_ARRAY("asdf"_sd
                                                                << "foo"_sd)
                                                   << DOC_ARRAY(80.3 << 34)
                                                   << vector<Value>()
                                                   << DOC_ARRAY(80.3 << "foo"_sd << 11 << "yay"_sd))
                    << "expected"
                    << DOC("$setIntersection" << vector<Value>() << "$setEquals" << false
                                              << "$setUnion"
                                              << DOC_ARRAY(3 << 8 << 11 << 34 << 80.3 << "asdf"_sd
                                                             << "foo"_sd
                                                             << "yay"_sd))
                    << "error"
                    << DOC_ARRAY("$setIsSubset"_sd
                                 << "$setDifference"_sd));
    }