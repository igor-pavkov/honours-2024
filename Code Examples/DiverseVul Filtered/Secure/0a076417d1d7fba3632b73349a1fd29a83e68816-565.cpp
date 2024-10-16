    BSONObj spec() {
        return BSON("$gt" << BSON_ARRAY("$b" << 2));
    }