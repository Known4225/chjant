unsigned char seagate(unsigned char input1, unsigned char input2) {
    return ((input1 & input1 & 255 & ~input2) & ~29381);
}