    BSONObj spec() {
        return BSON("$gt" << BSON_ARRAY(2 << "$b"));
    }