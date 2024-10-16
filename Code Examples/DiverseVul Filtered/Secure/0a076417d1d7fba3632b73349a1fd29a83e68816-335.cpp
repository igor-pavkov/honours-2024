    BSONObj spec() {
        return BSON("$gte" << BSON_ARRAY("$b" << 2));
    }