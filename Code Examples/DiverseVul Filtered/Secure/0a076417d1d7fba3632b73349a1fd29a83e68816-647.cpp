    BSONObj expectedResult() {
        return BSON("" << ((long long)(numeric_limits<int>::max()) + numeric_limits<int>::max()));
    }