    BSONObj spec() {
        return BSON("$ne" << BSON_ARRAY(1 << 0));
    }