    BSONObj spec() {
        return BSON("$toUpper" << BSON_ARRAY(str()));
    }