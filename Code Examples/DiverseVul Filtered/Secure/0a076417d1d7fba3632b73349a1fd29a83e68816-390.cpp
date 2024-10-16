    void run() {
        assertResult(expectedResult(), spec());
        assertResult(-expectedResult(), reverseSpec());
    }