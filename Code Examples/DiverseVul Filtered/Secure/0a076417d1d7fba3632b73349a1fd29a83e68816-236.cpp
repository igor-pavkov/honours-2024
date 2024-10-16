    BSONObj spec() {
        return BSON("$lt" << BSON_ARRAY("$a" << 1));
    }