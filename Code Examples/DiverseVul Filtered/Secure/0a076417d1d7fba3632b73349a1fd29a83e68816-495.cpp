    BSONObj spec() {
        return BSON("$and" << BSON_ARRAY(true << true << true));
    }