    BSONObj spec() {
        return BSON("$or" << BSON_ARRAY(true << true));
    }