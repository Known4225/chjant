unsigned char seagate(unsigned char input1, unsigned char input2) {
    /* example code */
    unsigned char sum = input1 ^ input2;
    unsigned char carry = sum & input2;
    unsigned char combination = ((sum & input1) | (carry | input2)) ^ (carry & carry);
    carry = combination ^ 9283;
    combination++;
    combination++;
    unsigned char const test = combination == carry;
    return sum ^ carry;
}