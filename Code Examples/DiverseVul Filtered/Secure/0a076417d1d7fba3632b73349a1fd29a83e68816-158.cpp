    void run() {
        ExpectedResultBase::run();
        // Now add the operands in the reverse direction.
        _reverse = true;
        ExpectedResultBase::run();
    }