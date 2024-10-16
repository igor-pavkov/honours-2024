    BSONObj spec() {
        return BSON("$lte" << BSON_ARRAY(2 << "$b"));
    }