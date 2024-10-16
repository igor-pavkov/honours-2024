    BSONObj operand2() {
        return BSON("" << double(numeric_limits<long long>::max()));
    }