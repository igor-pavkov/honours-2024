    BSONObj spec() {
        return BSON("$eq" << BSON_ARRAY(BSON("$and" << BSON_ARRAY("$a")) << 1));
    }