    BSONObj spec() {
        return BSON("$or" << BSON_ARRAY("$a"
                                        << "$b"
                                        << 0));
    }