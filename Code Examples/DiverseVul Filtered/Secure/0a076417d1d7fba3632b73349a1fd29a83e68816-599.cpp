    Document getSpec() {
        return DOC("input" << DOC_ARRAY(BSONNULL) << "error" << DOC_ARRAY("$allElementsTrue"_sd
                                                                          << "$anyElementTrue"_sd));
    }