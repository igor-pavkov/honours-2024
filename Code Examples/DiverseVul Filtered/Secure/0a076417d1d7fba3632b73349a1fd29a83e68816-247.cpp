    BSONObj spec() {
        return BSON("$and" << BSON_ARRAY(false));
    }