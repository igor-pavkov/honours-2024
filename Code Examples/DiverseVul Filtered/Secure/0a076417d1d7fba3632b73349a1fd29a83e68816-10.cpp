    virtual BSONObj expectedOptimized() {
        return BSON("$const" << expectedResult().firstElement());
    }