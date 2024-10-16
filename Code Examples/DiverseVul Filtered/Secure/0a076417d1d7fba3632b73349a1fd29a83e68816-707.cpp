    BSONObj spec() {
        return BSON("$cmp" << BSON_ARRAY("z"
                                         << "a"));
    }