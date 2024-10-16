    Document getSpec() {
        return DOC("input" << DOC_ARRAY(DOC_ARRAY(true)) << "expected"
                           << DOC("$allElementsTrue" << true << "$anyElementTrue" << true));
    }