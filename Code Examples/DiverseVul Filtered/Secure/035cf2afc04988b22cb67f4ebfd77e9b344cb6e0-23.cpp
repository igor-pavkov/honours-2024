    Utf32String* yank() {
        return (size > 0) ? &theRing[indexToSlot[index]] : 0;
    }