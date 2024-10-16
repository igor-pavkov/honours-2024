    BSONObj spec() {
        return BSON("$gte" << BSON_ARRAY(2 << "$b"));
    }