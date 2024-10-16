    BSONObj spec() {
        return BSON("$eq" << BSON_ARRAY("$a" << 1));
    }