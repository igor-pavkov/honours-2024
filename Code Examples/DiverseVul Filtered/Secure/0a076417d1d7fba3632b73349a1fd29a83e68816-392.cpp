    BSONObj spec() {
        return BSON("$toLower" << BSON_ARRAY(str()));
    }