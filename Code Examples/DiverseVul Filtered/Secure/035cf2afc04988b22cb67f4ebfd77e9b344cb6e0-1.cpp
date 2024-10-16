    void kill(const UChar32* text, int textLen, bool forward) {
        if (textLen == 0) {
            return;
        }
        Utf32String killedText(text, textLen);
        if (lastAction == actionKill && size > 0) {
            int slot = indexToSlot[0];
            int currentLen = theRing[slot].length();
            int resultLen = currentLen + textLen;
            Utf32String temp(resultLen + 1);
            if (forward) {
                memcpy(temp.get(), theRing[slot].get(), currentLen * sizeof(UChar32));
                memcpy(&temp[currentLen], killedText.get(), textLen * sizeof(UChar32));
            } else {
                memcpy(temp.get(), killedText.get(), textLen * sizeof(UChar32));
                memcpy(&temp[textLen], theRing[slot].get(), currentLen * sizeof(UChar32));
            }
            temp[resultLen] = 0;
            temp.initFromBuffer();
            theRing[slot] = temp;
        } else {
            if (size < capacity) {
                if (size > 0) {
                    memmove(&indexToSlot[1], &indexToSlot[0], size);
                }
                indexToSlot[0] = size;
                size++;
                theRing.push_back(killedText);
            } else {
                int slot = indexToSlot[capacity - 1];
                theRing[slot] = killedText;
                memmove(&indexToSlot[1], &indexToSlot[0], capacity - 1);
                indexToSlot[0] = slot;
            }
            index = 0;
        }
    }