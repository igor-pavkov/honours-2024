    Document getSpec() {
        return DOC("input" << DOC_ARRAY(DOC_ARRAY(1)) << "expected"
                           << DOC("$allElementsTrue" << true << "$anyElementTrue" << true));
    }