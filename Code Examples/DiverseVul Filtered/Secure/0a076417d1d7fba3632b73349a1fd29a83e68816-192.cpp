    BSONObj spec() {
        return BSON("$lte" << BSON_ARRAY(1 << 0));
    }