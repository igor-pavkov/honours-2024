    TEST(BSONValidateFast, StringHasSomething) {
        BufBuilder bb;
        BSONObjBuilder ob(bb);
        bb.appendChar(String);
        bb.appendStr("x", /*withNUL*/true);
        bb.appendNum(0);
        const BSONObj x = ob.done();
        ASSERT_EQUALS(5 // overhead
                      + 1 // type
                      + 2 // name
                      + 4 // size
                      , x.objsize());
        ASSERT_NOT_OK(validateBSON(x.objdata(), x.objsize()));
    }