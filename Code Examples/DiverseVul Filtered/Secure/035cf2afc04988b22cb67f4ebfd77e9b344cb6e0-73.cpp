    Utf32String* yankPop() {
        if (size == 0) {
            return 0;
        }
        ++index;
        if (index == size) {
            index = 0;
        }
        return &theRing[indexToSlot[index]];
    }