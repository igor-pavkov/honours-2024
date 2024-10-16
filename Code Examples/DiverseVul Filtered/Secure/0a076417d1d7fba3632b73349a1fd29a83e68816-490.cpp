    BSONObj expectedOptimized() {
        return BSON("$and" << BSON_ARRAY("$a"
                                         << "$b"));
    }