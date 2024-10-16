    BSONObj spec() {
        return BSON("$and" << BSON_ARRAY("$a"
                                         << "$b"
                                         << 0));
    }