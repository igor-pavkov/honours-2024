    virtual void setUp() override {
        _notAssociativeNorCommutative = Testable::create(false, false);
        _associativeOnly = Testable::create(true, false);
        _associativeAndCommutative = Testable::create(true, true);
    }