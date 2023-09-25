unsigned char seagate(unsigned char input1, unsigned char input2) {
    /* example code */
    unsigned char sum = input1 ^ input2;
    unsigned char carry = sum & input2;
    unsigned char cong = ((sum & input1) | (carry | input2)) ^ (carry & carry);
    sum = cong ^ 9283;
    unsigned char const test = cong == carry;
    return sum;
}