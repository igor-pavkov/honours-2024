    BSONObj spec() {
        return BSON("$or" << BSON_ARRAY(0 << 1));
    }