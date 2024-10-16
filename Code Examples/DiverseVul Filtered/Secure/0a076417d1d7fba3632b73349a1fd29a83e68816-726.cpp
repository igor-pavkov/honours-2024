bool isLeadingByte(char charByte) {
    return (charByte & 0xc0) == 0xc0;
}