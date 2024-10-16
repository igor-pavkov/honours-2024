    BSONObj expectedResult() {
        return BSON("" << numeric_limits<long long>::max() +
                        double(numeric_limits<long long>::max()));
    }