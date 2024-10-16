    BSONObj spec() {
        return BSON("$eq" << BSON_ARRAY(1 << BSON("$and" << BSON_ARRAY("$a"))));
    }