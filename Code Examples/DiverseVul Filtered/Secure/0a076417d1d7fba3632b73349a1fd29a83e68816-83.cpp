    BSONObj expectedOptimized() {
        return BSON("$or" << BSON_ARRAY("$a"
                                        << "$b"));
    }