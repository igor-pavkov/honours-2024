    static intrusive_ptr<Testable> create(bool associative, bool commutative) {
        return new Testable(associative, commutative);
    }