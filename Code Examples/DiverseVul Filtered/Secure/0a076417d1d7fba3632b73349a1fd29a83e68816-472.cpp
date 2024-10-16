    BSONObj spec() {
        return BSON("$ne" << BSON_ARRAY("$a"
                                        << "$b"));
    }