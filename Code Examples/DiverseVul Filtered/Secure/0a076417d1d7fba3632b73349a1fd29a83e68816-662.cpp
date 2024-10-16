    BSONObj spec() {
        return BSON("$lte" << BSON_ARRAY("$b" << 2));
    }