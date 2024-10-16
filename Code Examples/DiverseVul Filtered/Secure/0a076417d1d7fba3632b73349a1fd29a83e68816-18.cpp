    BSONObj spec() {
        return BSON("$or" << BSON_ARRAY(0 << BSON("$or" << BSON_ARRAY(BSON("$or" << BSON_ARRAY(1))))
                                          << "$a"
                                          << "$b"));
    }