#include "seagateType.h"

/* basically this function does all the actual creating of the gates */
/* this one function is longer than the entire seagate.c file, which does all the file io and parsing */
/* mostly due to a lot of duplicate code, which i dont really care about, let me do my over 1000+ line switch statement */

void recordPaddedReg(seagate *selfp, int reference1, int reference2, int operation) { // creates a new register from one or two reference handle and an operation
    if (reference1 == -1) {
        printf("This should never happen.\n");
        return;
    }
    seagate self = *selfp;
    char *ref1name = self.registers -> data[reference1].s;
    int size1 = self.registers -> data[reference1 + 1].i; // size is the same as reference1 size
    char *ref2name = self.registers -> data[reference2].s;
    int size2 = self.registers -> data[reference2 + 1].i; // size is the same as reference2 size
    char *newName;
    int scaleFactor;
    int shrinkSize;
    int savedComp;
    /* all of this garbage is so these values can be publically available */
    int maxUp;
    int preMoveHeight;
    int smallerReg = reference1;
    int biggerReg = reference2;
    int sizeDifference = size2 - size1;
    int smallerSize = size1;
    int biggerSize = size2;
    if (size1 > size2) {
        smallerSize = size2;
        biggerSize = size1;
        smallerReg = reference2;
        biggerReg = reference1;
        sizeDifference = size1 - size2;
    }
    /* operation codes:
    0 - rename register (reference1 becomes reference2, assignment)
    1 - bitwise NOT (reference1)
    2 - bitwise AND
    3 - bitwise OR
    4 - bitwise XOR
    5 - logical NOT (reference1)
    6 - logical AND
    7 - logical OR
    8 - logical XOR
    9 - shift left (unsigned SHL only)
    10 - shift right (unsigned SHR only)
    11 - increment (reference1)
    12 - decrement (reference1)
    13 - negative (reference1)
    14 - logical equals
    15 - pipe output from register2 to register1
    21 - add
    22 - subtract
    23 - multiply
    24 - divide
    25 - modulo
    */
    /* temporary copy constructor if the register is operated on itself */
    if (reference1 == reference2) { // this doesn't check if it's an operation on two registers, but it shouldn't matter
        char tempLookup[strlen(ref1name) + 5];
        memcpy(tempLookup, ref1name, strlen(ref1name));
        memcpy(tempLookup + strlen(ref1name), "COPY", 5);
        int checkForCopy = checkNamespace(&self, tempLookup);
        if (checkForCopy == -1) {
            /* create a copy and assign reference2 to it */
            self.registers -> data[reference1 + 2].i += 1;
            list_append(self.registers, (unitype) strdup(tempLookup), 's');
            list_append(self.registers, (unitype) size1, 'i');
            list_append(self.registers, (unitype) 0, 'i'); // 0 references to this register so far
            list_append(self.registers, (unitype) list_init(), 'r'); // list of handles for registers
            for (int i = 0; i < size1; i++) {
                list_append(self.registers -> data[self.registers -> length - 1].r, (unitype) (int) self.components -> length, 'i');
                list_append(self.components, (unitype) "POWER", 's');
                list_append(self.positions, (unitype) (self.positions -> data[self.registers -> data[reference1 + 3].r -> data[0].i * 3 - 2].d - self.compDistX * i), 'd'); // same x position as reference register
                list_append(self.positions, (unitype) (self.positions -> data[self.registers -> data[reference1 + 3].r -> data[0].i * 3 - 1].d + self.compDistY * self.registers -> data[reference1 + 2].i), 'd'); // y position of reference + compDist * number of references
                list_append(self.positions, (unitype) 0, 'd'); // facing upwards
                list_append(self.io, (unitype) 0, 'i');
                list_append(self.io, (unitype) 0, 'i');
                list_append(self.io, (unitype) 0, 'i');
                list_append(self.inpComp, self.compSlots -> data[list_find(self.compSlots, (unitype) "POWER", 's') + 1], 'i');
                list_append(self.inpComp, self.registers -> data[reference1 + 3].r -> data[i], 'i');
                list_append(self.inpComp, (unitype) 0, 'i');

                /* add wiring from reference register, in this case it's a 1 to 1 bit to bit */
                list_append(self.wiring, self.registers -> data[reference1 + 3].r -> data[i], 'i');
                list_append(self.wiring, (unitype) (int) (self.components -> length - 1), 'i');
                list_append(self.wiring, (unitype) 0, 'i');
            }
            reference2 = self.registers -> length - 4;
        } else {
            /* copy already exists */
            reference2 = checkForCopy;
        }

    }
    list_t *wireTemp = list_init();

    switch (operation) {
    case 0: ; // assignment/copying
    if (size1 != size2) {
        printf("Warning: bit mismatch assigning %s (%d bits) to %s (%d bits)\n", self.registers -> data[reference1].s, self.registers -> data[reference1 + 1].i, self.registers -> data[reference2].s, self.registers -> data[reference2 + 1].i);
    }
    self.registers -> data[reference1 + 2] = self.registers -> data[reference2 + 2]; // currently there's no way to keep these in sync... this may become a problem. I'm choosing to ignore it for now
    for (int i = 0; i < smallerSize; i++) {
        self.registers -> data[reference1 + 3].r -> data[i] = self.registers -> data[reference2 + 3].r -> data[i]; // simply redefines the bits to those of reference2
    }
    break;





    case 1: ; // this semicolon is because gcc is as bad at parsing C code as I am, bitwise NOT
    self.registers -> data[reference1 + 2].i += 1;
    newName = malloc(strlen(ref1name) + 4);
    memcpy(newName, ref1name, strlen(ref1name)); // create a new register called {reference1.name}NOT
    memcpy(newName + strlen(ref1name), "NOT", 4);
    list_append(self.registers, (unitype) newName, 's');
    self.opResult = newName;
    list_append(self.registers, (unitype) size1, 'i');
    list_append(self.registers, (unitype) 0, 'i'); // 0 references to this register so far
    list_append(self.registers, (unitype) list_init(), 'r'); // list of handles for registers
    for (int i = 0; i < size1; i++) {
        list_append(self.registers -> data[self.registers -> length - 1].r, (unitype) (int) self.components -> length, 'i');
        list_append(self.components, (unitype) "NOT", 's');
        list_append(self.positions, (unitype) (self.positions -> data[self.registers -> data[reference1 + 3].r -> data[0].i * 3 - 2].d - self.compDistX * i), 'd'); // same x position as reference register
        list_append(self.positions, (unitype) (self.positions -> data[self.registers -> data[reference1 + 3].r -> data[0].i * 3 - 1].d + self.compDistY * self.registers -> data[reference1 + 2].i), 'd'); // y position of reference + compDist * number of references
        list_append(self.positions, (unitype) 0, 'd'); // facing upwards
        list_append(self.io, (unitype) 0, 'i');
        list_append(self.io, (unitype) 0, 'i');
        list_append(self.io, (unitype) 0, 'i');
        list_append(self.inpComp, self.compSlots -> data[list_find(self.compSlots, (unitype) "NOT", 's') + 1], 'i');
        list_append(self.inpComp, self.registers -> data[reference1 + 3].r -> data[i], 'i');
        list_append(self.inpComp, (unitype) 0, 'i');

        /* add wiring from reference register, in this case it's a 1 to 1 bit to bit */
        list_append(self.wiring, self.registers -> data[reference1 + 3].r -> data[i], 'i');
        list_append(self.wiring, (unitype) (int) (self.components -> length - 1), 'i');
        list_append(self.wiring, (unitype) 0, 'i');
    }
    break;





    case 2: // bitwise AND
    self.registers -> data[reference1 + 2].i += 1; // increment source register reference count (just the first one since the new gates are going on top of that)
    newName = malloc(strlen(ref1name) + strlen(ref2name) + 4);
    memcpy(newName, ref1name, strlen(ref1name)); // create a new register called {reference1.name}AND{reference2.name}
    memcpy(newName + strlen(ref1name), "AND", 3);
    memcpy(newName + strlen(ref1name) + 3, ref2name, strlen(ref2name) + 1);
    self.opResult = newName;
    list_append(self.registers, (unitype) newName, 's');
    list_append(self.registers, (unitype) smallerSize, 'i');
    list_append(self.registers, (unitype) 0, 'i'); // 0 references to this register so far
    list_append(self.registers, (unitype) list_init(), 'r'); // list of handles for registers
    for (int i = 0; i < smallerSize; i++) {
        list_append(self.registers -> data[self.registers -> length - 1].r, (unitype) (int) self.components -> length, 'i');
        list_append(self.components, (unitype) "AND", 's');
        list_append(self.positions, (unitype) (self.positions -> data[self.registers -> data[reference1 + 3].r -> data[0].i * 3 - 2].d - self.compDistX * i), 'd'); // same x position as reference1 register
        list_append(self.positions, (unitype) (self.positions -> data[self.registers -> data[reference1 + 3].r -> data[0].i * 3 - 1].d + self.compDistY * self.registers -> data[reference1 + 2].i), 'd'); // y position of reference1 + compDist * number of references
        list_append(self.positions, (unitype) 0, 'd'); // facing upwards
        list_append(self.io, (unitype) 0, 'i');
        list_append(self.io, (unitype) 0, 'i');
        list_append(self.io, (unitype) 0, 'i');
        list_append(self.inpComp, self.compSlots -> data[list_find(self.compSlots, (unitype) "AND", 's') + 1], 'i');
        list_append(self.inpComp, self.registers -> data[reference1 + 3].r -> data[i], 'i');
        list_append(self.inpComp, self.registers -> data[reference2 + 3].r -> data[i], 'i');

        /* add wiring from reference register, in this case it's a 1 to 1 bit to bit */
        // printf("ref1 %d\n", reference1);
        list_append(self.wiring, self.registers -> data[reference1 + 3].r -> data[i], 'i');
        list_append(self.wiring, (unitype) (int) (self.components -> length - 1), 'i');
        list_append(self.wiring, (unitype) 0, 'i');
        list_append(self.wiring, self.registers -> data[reference2 + 3].r -> data[i], 'i');
        list_append(self.wiring, (unitype) (int) (self.components -> length - 1), 'i');
        list_append(self.wiring, (unitype) 0, 'i');
    }
    break;





    case 3: ; // bitwise OR
    self.registers -> data[reference1 + 2].i += 1; // increment source register reference count
    newName = malloc(strlen(ref1name) + strlen(ref2name) + 3);
    memcpy(newName, ref1name, strlen(ref1name)); // create a new register called {reference1.name}OR{reference2.name}
    memcpy(newName + strlen(ref1name), "OR", 2);
    memcpy(newName + strlen(ref1name) + 2, ref2name, strlen(ref2name) + 1);
    self.opResult = newName;
    list_append(self.registers, (unitype) newName, 's');
    list_append(self.registers, (unitype) smallerSize, 'i');
    list_append(self.registers, (unitype) 0, 'i'); // 0 references to this register so far
    list_append(self.registers, (unitype) list_init(), 'r'); // list of handles for registers
    for (int i = 0; i < smallerSize; i++) {
        list_append(self.registers -> data[self.registers -> length - 1].r, (unitype) (int) self.components -> length, 'i');
        list_append(self.components, (unitype) "OR", 's');
        list_append(self.positions, (unitype) (self.positions -> data[self.registers -> data[reference1 + 3].r -> data[0].i * 3 - 2].d - self.compDistX * i), 'd'); // same x position as reference1 register
        list_append(self.positions, (unitype) (self.positions -> data[self.registers -> data[reference1 + 3].r -> data[0].i * 3 - 1].d + self.compDistY * self.registers -> data[reference1 + 2].i), 'd'); // y position of reference1 + compDist * number of references
        list_append(self.positions, (unitype) 0, 'd'); // facing upwards
        list_append(self.io, (unitype) 0, 'i');
        list_append(self.io, (unitype) 0, 'i');
        list_append(self.io, (unitype) 0, 'i');
        list_append(self.inpComp, self.compSlots -> data[list_find(self.compSlots, (unitype) "OR", 's') + 1], 'i');
        list_append(self.inpComp, self.registers -> data[reference1 + 3].r -> data[i], 'i');
        list_append(self.inpComp, self.registers -> data[reference2 + 3].r -> data[i], 'i');

        /* add wiring from reference register, in this case it's a 1 to 1 bit to bit */
        list_append(self.wiring, self.registers -> data[reference1 + 3].r -> data[i], 'i');
        list_append(self.wiring, (unitype) (int) (self.components -> length - 1), 'i');
        list_append(self.wiring, (unitype) 0, 'i');
        list_append(self.wiring, self.registers -> data[reference2 + 3].r -> data[i], 'i');
        list_append(self.wiring, (unitype) (int) (self.components -> length - 1), 'i');
        list_append(self.wiring, (unitype) 0, 'i');
    }
    break;





    case 4: ; // bitwise XOR
    self.registers -> data[reference1 + 2].i += 1; // increment source register reference count
    newName = malloc(strlen(ref1name) + strlen(ref2name) + 4);
    memcpy(newName, ref1name, strlen(ref1name)); // create a new register called {reference1.name}XOR{reference2.name}
    memcpy(newName + strlen(ref1name), "XOR", 3);
    memcpy(newName + strlen(ref1name) + 3, ref2name, strlen(ref2name) + 1);
    self.opResult = newName;
    list_append(self.registers, (unitype) newName, 's');
    list_append(self.registers, (unitype) smallerSize, 'i');
    list_append(self.registers, (unitype) 0, 'i'); // 0 references to this register so far
    list_append(self.registers, (unitype) list_init(), 'r'); // list of handles for registers
    for (int i = 0; i < smallerSize; i++) {
        list_append(self.registers -> data[self.registers -> length - 1].r, (unitype) (int) self.components -> length, 'i');
        list_append(self.components, (unitype) "XOR", 's');
        list_append(self.positions, (unitype) (self.positions -> data[self.registers -> data[reference1 + 3].r -> data[0].i * 3 - 2].d - self.compDistX * i), 'd'); // same x position as reference1 register
        list_append(self.positions, (unitype) (self.positions -> data[self.registers -> data[reference1 + 3].r -> data[0].i * 3 - 1].d + self.compDistY * self.registers -> data[reference1 + 2].i), 'd'); // y position of reference1 + compDist * number of references
        list_append(self.positions, (unitype) 0, 'd'); // facing upwards
        list_append(self.io, (unitype) 0, 'i');
        list_append(self.io, (unitype) 0, 'i');
        list_append(self.io, (unitype) 0, 'i');
        list_append(self.inpComp, self.compSlots -> data[list_find(self.compSlots, (unitype) "XOR", 's') + 1], 'i');
        list_append(self.inpComp, self.registers -> data[reference1 + 3].r -> data[i], 'i');
        list_append(self.inpComp, self.registers -> data[reference2 + 3].r -> data[i], 'i');

        /* add wiring from reference register, in this case it's a 1 to 1 bit to bit */
        list_append(self.wiring, self.registers -> data[reference1 + 3].r -> data[i], 'i');
        list_append(self.wiring, (unitype) (int) (self.components -> length - 1), 'i');
        list_append(self.wiring, (unitype) 0, 'i');
        list_append(self.wiring, (unitype) self.registers -> data[reference2 + 3].r -> data[i], 'i');
        list_append(self.wiring, (unitype) (int) (self.components -> length - 1), 'i');
        list_append(self.wiring, (unitype) 0, 'i');
    }
    break;





    case 5: ; // logical NOT
    /* for each register we create a tower of ORs where each next layer of the tower contains half as many ORs as the last */
    newName = malloc(strlen(ref1name) + 2);
    memcpy(newName, ref1name, strlen(ref1name)); // create a new register called {reference1.name}N
    memcpy(newName + strlen(ref1name), "N", 2);
    list_append(self.registers, (unitype) newName, 's');
    self.opResult = newName;
    list_append(self.registers, (unitype) size1, 'i');
    list_append(self.registers, (unitype) 0, 'i'); // 0 references to this register so far
    list_append(self.registers, (unitype) list_init(), 'r'); // list of handles for registers

    /* initialise wireTemp */
    list_clear(wireTemp);
    for (int i = 0; i < size1; i++) {
        list_append(wireTemp, self.registers -> data[reference1 + 3].r -> data[i], 'i');
    }

    self.registers -> data[reference1 + 2].i += 1; // increment source register reference count by number of layers (incrementally)
    scaleFactor = 2;
    while (scaleFactor < size1 + 1) {
        for (int i = 0; i < size1; i += scaleFactor) {
            list_append(self.components, (unitype) "OR", 's');
            list_append(self.positions, (unitype) (self.positions -> data[self.registers -> data[reference1 + 3].r -> data[0].i * 3 - 2].d - self.compDistX * i), 'd'); // same x position as reference1 register
            list_append(self.positions, (unitype) (self.positions -> data[self.registers -> data[reference1 + 3].r -> data[0].i * 3 - 1].d + self.compDistY * self.registers -> data[reference1 + 2].i), 'd'); // y position of reference1 + compDist * number of references
            list_append(self.positions, (unitype) 0, 'd'); // facing upwards
            list_append(self.io, (unitype) 0, 'i');
            list_append(self.io, (unitype) 0, 'i');
            list_append(self.io, (unitype) 0, 'i');
            list_append(self.inpComp, self.compSlots -> data[list_find(self.compSlots, (unitype) "OR", 's') + 1], 'i');
            list_append(self.inpComp, wireTemp -> data[i], 'i');
            list_append(self.inpComp, wireTemp -> data[i + scaleFactor / 2], 'i');

            /* add wiring from reference register */
            list_append(self.wiring, wireTemp -> data[i], 'i');
            list_append(self.wiring, (unitype) (int) (self.components -> length - 1), 'i');
            list_append(self.wiring, (unitype) 0, 'i');

            list_append(self.wiring, wireTemp -> data[i + scaleFactor / 2], 'i');
            list_append(self.wiring, (unitype) (int) (self.components -> length - 1), 'i');
            list_append(self.wiring, (unitype) 0, 'i');

            wireTemp -> data[i].i = (int) (self.components -> length - 1);
            
        }
        self.registers -> data[reference1 + 2].i += 1;
        scaleFactor *= 2;
    }
    list_append(self.registers -> data[self.registers -> length - 1].r, (unitype) (int) (self.components -> length - 1), 'i');
    for (int i = 1; i < size1; i++) { // create n - 1 useless POWER blocks to act as other bits
        list_append(self.components, (unitype) "POWER", 's');
        list_append(self.positions, (unitype) (self.positions -> data[self.registers -> data[reference1 + 3].r -> data[0].i * 3 - 2].d - self.compDistX * i), 'd'); // same x position as reference register
        list_append(self.positions, (unitype) (self.positions -> data[self.registers -> data[reference1 + 3].r -> data[0].i * 3 - 1].d + self.compDistY * (self.registers -> data[reference1 + 2].i - 1)), 'd'); // y position of reference + compDist * number of references
        list_append(self.positions, (unitype) 0, 'd'); // facing upwards
        list_append(self.io, (unitype) 0, 'i');
        list_append(self.io, (unitype) 0, 'i');
        list_append(self.io, (unitype) 0, 'i');
        list_append(self.inpComp, self.compSlots -> data[list_find(self.compSlots, (unitype) "POWER", 's') + 1], 'i');
        list_append(self.inpComp, (unitype) 0, 'i');
        list_append(self.inpComp, (unitype) 0, 'i');
        list_append(self.registers -> data[self.registers -> length - 1].r, (unitype) (int) (self.components -> length - 1), 'i');
    }
    break;




    case 6: ; // logical AND
    newName = malloc(strlen(ref1name) + strlen(ref2name) + 2);
    memcpy(newName, ref1name, strlen(ref1name)); // create a new register called {reference1.name}A{reference2.name}
    memcpy(newName + strlen(ref1name), "A", 2);
    memcpy(newName + strlen(ref1name) + 1, ref2name, strlen(ref2name) + 1);
    self.opResult = newName;
    list_append(self.registers, (unitype) newName, 's');
    list_append(self.registers, (unitype) smallerSize, 'i');
    list_append(self.registers, (unitype) 0, 'i'); // 0 references to this register so far
    list_append(self.registers, (unitype) list_init(), 'r'); // list of handles for registers

    /* create ORs for register1 */
    /* initialise wireTemp */
    list_clear(wireTemp);
    for (int i = 0; i < size1; i++) {
        list_append(wireTemp, self.registers -> data[reference1 + 3].r -> data[i], 'i');
    }

    self.registers -> data[reference1 + 2].i += 1; // increment source register reference count by number of layers (incrementally)
    scaleFactor = 2;
    while (scaleFactor < size1 + 1) {
        for (int i = 0; i < size1; i += scaleFactor) {
            list_append(self.components, (unitype) "OR", 's');
            list_append(self.positions, (unitype) (self.positions -> data[self.registers -> data[reference1 + 3].r -> data[0].i * 3 - 2].d - self.compDistX * i), 'd'); // same x position as reference1 register
            list_append(self.positions, (unitype) (self.positions -> data[self.registers -> data[reference1 + 3].r -> data[0].i * 3 - 1].d + self.compDistY * self.registers -> data[reference1 + 2].i), 'd'); // y position of reference1 + compDist * number of references
            list_append(self.positions, (unitype) 0, 'd'); // facing upwards
            list_append(self.io, (unitype) 0, 'i');
            list_append(self.io, (unitype) 0, 'i');
            list_append(self.io, (unitype) 0, 'i');
            list_append(self.inpComp, self.compSlots -> data[list_find(self.compSlots, (unitype) "OR", 's') + 1], 'i');
            list_append(self.inpComp, wireTemp -> data[i], 'i');
            list_append(self.inpComp, wireTemp -> data[i + scaleFactor / 2], 'i');

            /* add wiring from reference register */
            list_append(self.wiring, wireTemp -> data[i], 'i');
            list_append(self.wiring, (unitype) (int) (self.components -> length - 1), 'i');
            list_append(self.wiring, (unitype) 0, 'i');

            list_append(self.wiring, wireTemp -> data[i + scaleFactor / 2], 'i');
            list_append(self.wiring, (unitype) (int) (self.components -> length - 1), 'i');
            list_append(self.wiring, (unitype) 0, 'i');

            wireTemp -> data[i].i = (int) (self.components -> length - 1);
            
        }
        self.registers -> data[reference1 + 2].i += 1;
        scaleFactor *= 2;
    }

    savedComp = self.components -> length - 1; // save this component for input 1 to the AND gate that will be the output of the logical AND

    /* create ORs for register2 */
    /* initialise wireTemp */
    list_clear(wireTemp);
    for (int i = 0; i < size2; i++) {
        list_append(wireTemp, self.registers -> data[reference2 + 3].r -> data[i], 'i');
    }

    self.registers -> data[reference2 + 2].i += 1; // increment source register reference count by number of layers (incrementally)
    scaleFactor = 2;
    while (scaleFactor < size2 + 1) {
        for (int i = 0; i < size2; i += scaleFactor) {
            list_append(self.components, (unitype) "OR", 's');
            list_append(self.positions, (unitype) (self.positions -> data[self.registers -> data[reference2 + 3].r -> data[0].i * 3 - 2].d - self.compDistX * i), 'd'); // same x position as reference1 register
            list_append(self.positions, (unitype) (self.positions -> data[self.registers -> data[reference2 + 3].r -> data[0].i * 3 - 1].d + self.compDistY * self.registers -> data[reference2 + 2].i), 'd'); // y position of reference1 + compDist * number of references
            list_append(self.positions, (unitype) 0, 'd'); // facing upwards
            list_append(self.io, (unitype) 0, 'i');
            list_append(self.io, (unitype) 0, 'i');
            list_append(self.io, (unitype) 0, 'i');
            list_append(self.inpComp, self.compSlots -> data[list_find(self.compSlots, (unitype) "OR", 's') + 1], 'i');
            list_append(self.inpComp, wireTemp -> data[i], 'i');
            list_append(self.inpComp, wireTemp -> data[i + scaleFactor / 2], 'i');

            /* add wiring from reference register */
            list_append(self.wiring, wireTemp -> data[i], 'i');
            list_append(self.wiring, (unitype) (int) (self.components -> length - 1), 'i');
            list_append(self.wiring, (unitype) 0, 'i');

            list_append(self.wiring, wireTemp -> data[i + scaleFactor / 2], 'i');
            list_append(self.wiring, (unitype) (int) (self.components -> length - 1), 'i');
            list_append(self.wiring, (unitype) 0, 'i');

            wireTemp -> data[i].i = (int) (self.components -> length - 1);
            
        }
        self.registers -> data[reference2 + 2].i += 1;
        scaleFactor *= 2;
    }


    list_append(self.components, (unitype) "AND", 's');
    list_append(self.positions, (unitype) (self.positions -> data[self.registers -> data[reference1 + 3].r -> data[0].i * 3 - 2].d - self.compDistX * 0), 'd'); // same x position as reference1 register
    list_append(self.positions, (unitype) (self.positions -> data[self.registers -> data[reference1 + 3].r -> data[0].i * 3 - 1].d + self.compDistY * self.registers -> data[reference1 + 2].i), 'd'); // y position of reference1 + compDist * number of references
    list_append(self.positions, (unitype) 0, 'd'); // facing upwards
    list_append(self.io, (unitype) 0, 'i');
    list_append(self.io, (unitype) 0, 'i');
    list_append(self.io, (unitype) 0, 'i');
    list_append(self.inpComp, self.compSlots -> data[list_find(self.compSlots, (unitype) "AND", 's') + 1], 'i');
    list_append(self.inpComp, (unitype) savedComp, 'i');
    list_append(self.inpComp, (unitype) (int) (self.components -> length - 2), 'i'); // -2 refers to the last OR created, where -1 refers to this very AND component

    /* add wiring from reference register */
    list_append(self.wiring, (unitype) savedComp, 'i'); // create wire from savedComp to the AND
    list_append(self.wiring, (unitype) (int) (self.components -> length - 1), 'i');
    list_append(self.wiring, (unitype) 0, 'i');

    list_append(self.wiring, (unitype) (int) (self.components -> length - 2), 'i'); // create wire from last OR to the AND
    list_append(self.wiring, (unitype) (int) (self.components -> length - 1), 'i');
    list_append(self.wiring, (unitype) 0, 'i');

    list_append(self.registers -> data[self.registers -> length - 1].r, (unitype) (int) (self.components -> length - 1), 'i'); // add this as the first bit of the output
    self.registers -> data[reference1 + 2].i += 1;

    for (int i = 1; i < smallerSize; i++) { // create n - 1 useless POWER blocks to act as other bits
        list_append(self.components, (unitype) "POWER", 's');
        list_append(self.positions, (unitype) (self.positions -> data[self.registers -> data[reference1 + 3].r -> data[0].i * 3 - 2].d - self.compDistX * i), 'd'); // same x position as reference register
        list_append(self.positions, (unitype) (self.positions -> data[self.registers -> data[reference1 + 3].r -> data[0].i * 3 - 1].d + self.compDistY * (self.registers -> data[reference1 + 2].i - 1)), 'd'); // y position of reference + compDist * number of references
        list_append(self.positions, (unitype) 0, 'd'); // facing upwards
        list_append(self.io, (unitype) 0, 'i');
        list_append(self.io, (unitype) 0, 'i');
        list_append(self.io, (unitype) 0, 'i');
        list_append(self.inpComp, self.compSlots -> data[list_find(self.compSlots, (unitype) "POWER", 's') + 1], 'i');
        list_append(self.inpComp, (unitype) 0, 'i');
        list_append(self.inpComp, (unitype) 0, 'i');
        list_append(self.registers -> data[self.registers -> length - 1].r, (unitype) (int) (self.components -> length - 1), 'i');
    }
    break;




    case 7:; // logical OR
    newName = malloc(strlen(ref1name) + strlen(ref2name) + 2);
    memcpy(newName, ref1name, strlen(ref1name)); // create a new register called {reference1.name}O{reference2.name}
    memcpy(newName + strlen(ref1name), "O", 2);
    memcpy(newName + strlen(ref1name) + 1, ref2name, strlen(ref2name) + 1);
    self.opResult = newName;
    list_append(self.registers, (unitype) newName, 's');
    list_append(self.registers, (unitype) smallerSize, 'i');
    list_append(self.registers, (unitype) 0, 'i'); // 0 references to this register so far
    list_append(self.registers, (unitype) list_init(), 'r'); // list of handles for registers

    /* create ORs for register1 */
    /* initialise wireTemp */
    list_clear(wireTemp);
    for (int i = 0; i < size1; i++) {
        list_append(wireTemp, self.registers -> data[reference1 + 3].r -> data[i], 'i');
    }

    self.registers -> data[reference1 + 2].i += 1; // increment source register reference count by number of layers (incrementally)
    scaleFactor = 2;
    while (scaleFactor < size1 + 1) {
        for (int i = 0; i < size1; i += scaleFactor) {
            list_append(self.components, (unitype) "OR", 's');
            list_append(self.positions, (unitype) (self.positions -> data[self.registers -> data[reference1 + 3].r -> data[0].i * 3 - 2].d - self.compDistX * i), 'd'); // same x position as reference1 register
            list_append(self.positions, (unitype) (self.positions -> data[self.registers -> data[reference1 + 3].r -> data[0].i * 3 - 1].d + self.compDistY * self.registers -> data[reference1 + 2].i), 'd'); // y position of reference1 + compDist * number of references
            list_append(self.positions, (unitype) 0, 'd'); // facing upwards
            list_append(self.io, (unitype) 0, 'i');
            list_append(self.io, (unitype) 0, 'i');
            list_append(self.io, (unitype) 0, 'i');
            list_append(self.inpComp, self.compSlots -> data[list_find(self.compSlots, (unitype) "OR", 's') + 1], 'i');
            list_append(self.inpComp, wireTemp -> data[i], 'i');
            list_append(self.inpComp, wireTemp -> data[i + scaleFactor / 2], 'i');

            /* add wiring from reference register */
            list_append(self.wiring, wireTemp -> data[i], 'i');
            list_append(self.wiring, (unitype) (int) (self.components -> length - 1), 'i');
            list_append(self.wiring, (unitype) 0, 'i');

            list_append(self.wiring, wireTemp -> data[i + scaleFactor / 2], 'i');
            list_append(self.wiring, (unitype) (int) (self.components -> length - 1), 'i');
            list_append(self.wiring, (unitype) 0, 'i');

            wireTemp -> data[i].i = (int) (self.components -> length - 1);
            
        }
        self.registers -> data[reference1 + 2].i += 1;
        scaleFactor *= 2;
    }

    savedComp = self.components -> length - 1; // save this component for input 1 to the AND gate that will be the output of the logical OR

    /* create ORs for register2 */
    /* initialise wireTemp */
    list_clear(wireTemp);
    for (int i = 0; i < size2; i++) {
        list_append(wireTemp, self.registers -> data[reference2 + 3].r -> data[i], 'i');
    }

    self.registers -> data[reference2 + 2].i += 1; // increment source register reference count by number of layers (incrementally)
    scaleFactor = 2;
    while (scaleFactor < size2 + 1) {
        for (int i = 0; i < size2; i += scaleFactor) {
            list_append(self.components, (unitype) "OR", 's');
            list_append(self.positions, (unitype) (self.positions -> data[self.registers -> data[reference2 + 3].r -> data[0].i * 3 - 2].d - self.compDistX * i), 'd'); // same x position as reference1 register
            list_append(self.positions, (unitype) (self.positions -> data[self.registers -> data[reference2 + 3].r -> data[0].i * 3 - 1].d + self.compDistY * self.registers -> data[reference2 + 2].i), 'd'); // y position of reference1 + compDist * number of references
            list_append(self.positions, (unitype) 0, 'd'); // facing upwards
            list_append(self.io, (unitype) 0, 'i');
            list_append(self.io, (unitype) 0, 'i');
            list_append(self.io, (unitype) 0, 'i');
            list_append(self.inpComp, self.compSlots -> data[list_find(self.compSlots, (unitype) "OR", 's') + 1], 'i');
            list_append(self.inpComp, wireTemp -> data[i], 'i');
            list_append(self.inpComp, wireTemp -> data[i + scaleFactor / 2], 'i');

            /* add wiring from reference register */
            list_append(self.wiring, wireTemp -> data[i], 'i');
            list_append(self.wiring, (unitype) (int) (self.components -> length - 1), 'i');
            list_append(self.wiring, (unitype) 0, 'i');

            list_append(self.wiring, wireTemp -> data[i + scaleFactor / 2], 'i');
            list_append(self.wiring, (unitype) (int) (self.components -> length - 1), 'i');
            list_append(self.wiring, (unitype) 0, 'i');

            wireTemp -> data[i].i = (int) (self.components -> length - 1);
            
        }
        self.registers -> data[reference2 + 2].i += 1;
        scaleFactor *= 2;
    }


    list_append(self.components, (unitype) "OR", 's');
    list_append(self.positions, (unitype) (self.positions -> data[self.registers -> data[reference1 + 3].r -> data[0].i * 3 - 2].d - self.compDistX * 0), 'd'); // same x position as reference1 register
    list_append(self.positions, (unitype) (self.positions -> data[self.registers -> data[reference1 + 3].r -> data[0].i * 3 - 1].d + self.compDistY * self.registers -> data[reference1 + 2].i), 'd'); // y position of reference1 + compDist * number of references
    list_append(self.positions, (unitype) 0, 'd'); // facing upwards
    list_append(self.io, (unitype) 0, 'i');
    list_append(self.io, (unitype) 0, 'i');
    list_append(self.io, (unitype) 0, 'i');
    list_append(self.inpComp, self.compSlots -> data[list_find(self.compSlots, (unitype) "OR", 's') + 1], 'i');
    list_append(self.inpComp, (unitype) savedComp, 'i');
    list_append(self.inpComp, (unitype) (int) (self.components -> length - 2), 'i'); // -2 refers to the last OR created for reg2, where -1 refers to this very OR component

    /* add wiring from reference register */
    list_append(self.wiring, (unitype) savedComp, 'i'); // create wire from savedComp to the OR
    list_append(self.wiring, (unitype) (int) (self.components -> length - 1), 'i');
    list_append(self.wiring, (unitype) 0, 'i');

    list_append(self.wiring, (unitype) (int) (self.components -> length - 2), 'i'); // create wire from last OR to this OR
    list_append(self.wiring, (unitype) (int) (self.components -> length - 1), 'i');
    list_append(self.wiring, (unitype) 0, 'i');

    list_append(self.registers -> data[self.registers -> length - 1].r, (unitype) (int) (self.components -> length - 1), 'i'); // add this as the first bit of the output
    self.registers -> data[reference1 + 2].i += 1;

    for (int i = 1; i < smallerSize; i++) { // create n - 1 useless POWER blocks to act as other bits
        list_append(self.components, (unitype) "POWER", 's');
        list_append(self.positions, (unitype) (self.positions -> data[self.registers -> data[reference1 + 3].r -> data[0].i * 3 - 2].d - self.compDistX * i), 'd'); // same x position as reference register
        list_append(self.positions, (unitype) (self.positions -> data[self.registers -> data[reference1 + 3].r -> data[0].i * 3 - 1].d + self.compDistY * (self.registers -> data[reference1 + 2].i - 1)), 'd'); // y position of reference + compDist * number of references
        list_append(self.positions, (unitype) 0, 'd'); // facing upwards
        list_append(self.io, (unitype) 0, 'i');
        list_append(self.io, (unitype) 0, 'i');
        list_append(self.io, (unitype) 0, 'i');
        list_append(self.inpComp, self.compSlots -> data[list_find(self.compSlots, (unitype) "POWER", 's') + 1], 'i');
        list_append(self.inpComp, (unitype) 0, 'i');
        list_append(self.inpComp, (unitype) 0, 'i');
        list_append(self.registers -> data[self.registers -> length - 1].r, (unitype) (int) (self.components -> length - 1), 'i');
    }
    break;




    case 8: ; // logical XOR (not equals), if one register is bigger in size it can't have bits set in the upper section for this to return false
    /* we XOR the bits together, then we OR the result of the XOR along with any bits that one register has that the other doesn't (if one register is larger than the other)
    */
    preMoveHeight = self.registers -> data[reference1 + 2].i;
    self.registers -> data[reference1 + 2].i += 1; // increment source register reference count
    newName = malloc(strlen(ref1name) + strlen(ref2name) + 2);
    memcpy(newName, ref1name, strlen(ref1name)); // create a new register called {reference1.name}X{reference2.name}
    memcpy(newName + strlen(ref1name), "X", 1);
    memcpy(newName + strlen(ref1name) + 1, ref2name, strlen(ref2name) + 1);
    self.opResult = newName;
    list_append(self.registers, (unitype) newName, 's');
    list_append(self.registers, (unitype) smallerSize, 'i');
    list_append(self.registers, (unitype) 0, 'i'); // 0 references to this register so far
    list_append(self.registers, (unitype) list_init(), 'r'); // list of handles for registers

    list_clear(wireTemp);
    for (int i = 0; i < smallerSize; i++) {
        list_append(self.components, (unitype) "XOR", 's');
        list_append(self.positions, (unitype) (self.positions -> data[self.registers -> data[reference1 + 3].r -> data[0].i * 3 - 2].d - self.compDistX * i), 'd'); // same x position as reference1 register
        list_append(self.positions, (unitype) (self.positions -> data[self.registers -> data[reference1 + 3].r -> data[0].i * 3 - 1].d + self.compDistY * self.registers -> data[reference1 + 2].i), 'd'); // y position of reference1 + compDist * number of references
        list_append(self.positions, (unitype) 0, 'd'); // facing upwards
        list_append(self.io, (unitype) 0, 'i');
        list_append(self.io, (unitype) 0, 'i');
        list_append(self.io, (unitype) 0, 'i');
        list_append(self.inpComp, self.compSlots -> data[list_find(self.compSlots, (unitype) "XOR", 's') + 1], 'i');
        list_append(self.inpComp, self.registers -> data[reference1 + 3].r -> data[i], 'i');
        list_append(self.inpComp, self.registers -> data[reference2 + 3].r -> data[i], 'i');

        /* add wiring from reference register, in this case it's a 1 to 1 bit to bit */
        list_append(self.wiring, self.registers -> data[reference1 + 3].r -> data[i], 'i');
        list_append(self.wiring, (unitype) (int) (self.components -> length - 1), 'i');
        list_append(self.wiring, (unitype) 0, 'i');
        list_append(self.wiring, (unitype) self.registers -> data[reference2 + 3].r -> data[i], 'i');
        list_append(self.wiring, (unitype) (int) (self.components -> length - 1), 'i');
        list_append(self.wiring, (unitype) 0, 'i');

        list_append(wireTemp, (unitype) (int) (self.components -> length - 1), 'i'); // keep track of these in wireTemp
    }

    /* create ORs for the output XOR */
    self.registers -> data[reference1 + 2].i += 1; // increment source register reference count by number of layers (incrementally)
    scaleFactor = 2;
    while (scaleFactor < smallerSize + 1) {
        for (int i = 0; i < smallerSize; i += scaleFactor) {
            list_append(self.components, (unitype) "OR", 's');
            list_append(self.positions, (unitype) (self.positions -> data[self.registers -> data[reference1 + 3].r -> data[0].i * 3 - 2].d - self.compDistX * i), 'd'); // same x position as reference1 register
            list_append(self.positions, (unitype) (self.positions -> data[self.registers -> data[reference1 + 3].r -> data[0].i * 3 - 1].d + self.compDistY * self.registers -> data[reference1 + 2].i), 'd'); // y position of reference1 + compDist * number of references
            list_append(self.positions, (unitype) 0, 'd'); // facing upwards
            list_append(self.io, (unitype) 0, 'i');
            list_append(self.io, (unitype) 0, 'i');
            list_append(self.io, (unitype) 0, 'i');
            list_append(self.inpComp, self.compSlots -> data[list_find(self.compSlots, (unitype) "OR", 's') + 1], 'i');
            list_append(self.inpComp, wireTemp -> data[i], 'i');
            list_append(self.inpComp, wireTemp -> data[i + scaleFactor / 2], 'i');

            /* add wiring from reference register */
            list_append(self.wiring, wireTemp -> data[i], 'i');
            list_append(self.wiring, (unitype) (int) (self.components -> length - 1), 'i');
            list_append(self.wiring, (unitype) 0, 'i');

            list_append(self.wiring, wireTemp -> data[i + scaleFactor / 2], 'i');
            list_append(self.wiring, (unitype) (int) (self.components -> length - 1), 'i');
            list_append(self.wiring, (unitype) 0, 'i');

            wireTemp -> data[i].i = (int) (self.components -> length - 1);
            
        }
        self.registers -> data[reference1 + 2].i += 1;
        scaleFactor *= 2;
    }

    if (size1 != size2) { // tangent: one register is larger than the other
    maxUp = self.registers -> data[reference1 + 2].i;
    self.registers -> data[reference1 + 2].i = preMoveHeight;
    savedComp = self.components -> length - 1; // save this component for master OR
    /* OR the remaining bits */
    
    list_clear(wireTemp);
    for (int i = 0; i < smallerSize; i++) { // fill the first smallerSize of wireTemp stuff (we won't need this)
        list_append(wireTemp, self.registers -> data[biggerReg + 3].r -> data[i], 'i');
    }
    for (int i = smallerSize; i < biggerSize; i++) { // consider only bits that are above the ones XOR'd
        list_append(wireTemp, self.registers -> data[biggerReg + 3].r -> data[i], 'i');
    }
    self.registers -> data[biggerReg + 2].i += 1; // increment source register reference count by number of layers (incrementally)
    shrinkSize = biggerSize - smallerSize;
    scaleFactor = 2;
    while (shrinkSize > 0) {
        for (int i = smallerSize; i < biggerSize; i += scaleFactor) {
            list_append(self.components, (unitype) "OR", 's');
            list_append(self.positions, (unitype) (self.positions -> data[self.registers -> data[biggerReg + 3].r -> data[0].i * 3 - 2].d - self.compDistX * i), 'd'); // same x position as reference1 register
            list_append(self.positions, (unitype) (self.positions -> data[self.registers -> data[biggerReg + 3].r -> data[0].i * 3 - 1].d + self.compDistY * self.registers -> data[biggerReg + 2].i), 'd'); // y position of reference1 + compDist * number of references
            list_append(self.positions, (unitype) 0, 'd'); // facing upwards
            list_append(self.io, (unitype) 0, 'i');
            list_append(self.io, (unitype) 0, 'i');
            list_append(self.io, (unitype) 0, 'i');
            list_append(self.inpComp, self.compSlots -> data[list_find(self.compSlots, (unitype) "OR", 's') + 1], 'i');
            list_append(self.inpComp, wireTemp -> data[i], 'i');
            if (i + scaleFactor / 2 < wireTemp -> length)
                list_append(self.inpComp, wireTemp -> data[i + scaleFactor / 2], 'i');
            else
                list_append(self.inpComp, (unitype) 0, 'i');

            /* add wiring from reference register */
            list_append(self.wiring, wireTemp -> data[i], 'i');
            list_append(self.wiring, (unitype) (int) (self.components -> length - 1), 'i');
            list_append(self.wiring, (unitype) 0, 'i');

            if (i + scaleFactor / 2 < wireTemp -> length) {
                list_append(self.wiring, wireTemp -> data[i + scaleFactor / 2], 'i');
                list_append(self.wiring, (unitype) (int) (self.components -> length - 1), 'i');
                list_append(self.wiring, (unitype) 0, 'i');
            }

            wireTemp -> data[i].i = (int) (self.components -> length - 1);
            
        }
        self.registers -> data[biggerReg + 2].i += 1;
        scaleFactor *= 2;
        shrinkSize /= 2;
    }

    /* last OR for the XOR'd and extra bits */
    list_append(self.components, (unitype) "OR", 's');
    list_append(self.positions, (unitype) (self.positions -> data[self.registers -> data[reference1 + 3].r -> data[0].i * 3 - 2].d - self.compDistX * 0), 'd'); // same x position as reference1 register
    list_append(self.positions, (unitype) (self.positions -> data[self.registers -> data[reference1 + 3].r -> data[0].i * 3 - 1].d + self.compDistY * self.registers -> data[reference1 + 2].i), 'd'); // y position of reference1 + compDist * number of references
    list_append(self.positions, (unitype) 0, 'd'); // facing upwards
    list_append(self.io, (unitype) 0, 'i');
    list_append(self.io, (unitype) 0, 'i');
    list_append(self.io, (unitype) 0, 'i');
    list_append(self.inpComp, self.compSlots -> data[list_find(self.compSlots, (unitype) "OR", 's') + 1], 'i');
    list_append(self.inpComp, (unitype) savedComp, 'i');
    list_append(self.inpComp, (unitype) (int) (self.components -> length - 2), 'i'); // -2 refers to the last OR created for reg2, where -1 refers to this very OR component

    /* add wiring from reference register */
    list_append(self.wiring, (unitype) savedComp, 'i'); // create wire from savedComp to the OR
    list_append(self.wiring, (unitype) (int) (self.components -> length - 1), 'i');
    list_append(self.wiring, (unitype) 0, 'i');

    list_append(self.wiring, (unitype) (int) (self.components -> length - 2), 'i'); // create wire from last OR to this OR
    list_append(self.wiring, (unitype) (int) (self.components -> length - 1), 'i');
    list_append(self.wiring, (unitype) 0, 'i');

    self.registers -> data[reference1 + 2].i += 1;
    
    }

    /* padding */

    list_append(self.registers -> data[self.registers -> length - 1].r, (unitype) (int) (self.components -> length - 1), 'i'); // add this as the first bit of the output

    for (int i = 1; i < smallerSize; i++) { // create n - 1 useless POWER blocks to act as other bits
        list_append(self.components, (unitype) "POWER", 's');
        list_append(self.positions, (unitype) (self.positions -> data[self.registers -> data[reference1 + 3].r -> data[0].i * 3 - 2].d - self.compDistX * i), 'd'); // same x position as reference register
        list_append(self.positions, (unitype) (self.positions -> data[self.registers -> data[reference1 + 3].r -> data[0].i * 3 - 1].d + self.compDistY * (self.registers -> data[reference1 + 2].i - 1)), 'd'); // y position of reference + compDist * number of references
        list_append(self.positions, (unitype) 0, 'd'); // facing upwards
        list_append(self.io, (unitype) 0, 'i');
        list_append(self.io, (unitype) 0, 'i');
        list_append(self.io, (unitype) 0, 'i');
        list_append(self.inpComp, self.compSlots -> data[list_find(self.compSlots, (unitype) "POWER", 's') + 1], 'i');
        list_append(self.inpComp, (unitype) 0, 'i');
        list_append(self.inpComp, (unitype) 0, 'i');
        list_append(self.registers -> data[self.registers -> length - 1].r, (unitype) (int) (self.components -> length - 1), 'i');
    }
    if (maxUp > self.registers -> data[reference1 + 2].i) {
        self.registers -> data[reference1 + 2].i = maxUp;
    }
    break;





    case 9:
    break;




    case 10:
    break;




    case 11:
    break;




    case 12:
    break;



    case 13:
    break;



    case 14: ; // logical equals, if one register is bigger in size it can't have bits set in the upper section for this to return true
    /* we XOR the bits together, then we OR the result of the XOR along with any bits that one register has that the other doesn't (if one register is larger than the other)
    then we take a NOT of the result of that and pad to the smaller type
    */
    preMoveHeight = self.registers -> data[reference1 + 2].i;
    self.registers -> data[reference1 + 2].i += 1; // increment source register reference count
    newName = malloc(strlen(ref1name) + strlen(ref2name) + 2);
    memcpy(newName, ref1name, strlen(ref1name)); // create a new register called {reference1.name}XN{reference2.name}
    memcpy(newName + strlen(ref1name), "XN", 2);
    memcpy(newName + strlen(ref1name) + 2, ref2name, strlen(ref2name) + 1);
    self.opResult = newName;
    list_append(self.registers, (unitype) newName, 's');
    list_append(self.registers, (unitype) smallerSize, 'i');
    list_append(self.registers, (unitype) 0, 'i'); // 0 references to this register so far
    list_append(self.registers, (unitype) list_init(), 'r'); // list of handles for registers

    list_clear(wireTemp);
    for (int i = 0; i < smallerSize; i++) {
        list_append(self.components, (unitype) "XOR", 's');
        list_append(self.positions, (unitype) (self.positions -> data[self.registers -> data[reference1 + 3].r -> data[0].i * 3 - 2].d - self.compDistX * i), 'd'); // same x position as reference1 register
        list_append(self.positions, (unitype) (self.positions -> data[self.registers -> data[reference1 + 3].r -> data[0].i * 3 - 1].d + self.compDistY * self.registers -> data[reference1 + 2].i), 'd'); // y position of reference1 + compDist * number of references
        list_append(self.positions, (unitype) 0, 'd'); // facing upwards
        list_append(self.io, (unitype) 0, 'i');
        list_append(self.io, (unitype) 0, 'i');
        list_append(self.io, (unitype) 0, 'i');
        list_append(self.inpComp, self.compSlots -> data[list_find(self.compSlots, (unitype) "XOR", 's') + 1], 'i');
        list_append(self.inpComp, self.registers -> data[reference1 + 3].r -> data[i], 'i');
        list_append(self.inpComp, self.registers -> data[reference2 + 3].r -> data[i], 'i');

        /* add wiring from reference register, in this case it's a 1 to 1 bit to bit */
        list_append(self.wiring, self.registers -> data[reference1 + 3].r -> data[i], 'i');
        list_append(self.wiring, (unitype) (int) (self.components -> length - 1), 'i');
        list_append(self.wiring, (unitype) 0, 'i');
        list_append(self.wiring, (unitype) self.registers -> data[reference2 + 3].r -> data[i], 'i');
        list_append(self.wiring, (unitype) (int) (self.components -> length - 1), 'i');
        list_append(self.wiring, (unitype) 0, 'i');

        list_append(wireTemp, (unitype) (int) (self.components -> length - 1), 'i'); // keep track of these in wireTemp
    }

    /* create ORs for the output XOR */
    self.registers -> data[reference1 + 2].i += 1; // increment source register reference count by number of layers (incrementally)
    scaleFactor = 2;
    while (scaleFactor < smallerSize + 1) {
        for (int i = 0; i < smallerSize; i += scaleFactor) {
            list_append(self.components, (unitype) "OR", 's');
            list_append(self.positions, (unitype) (self.positions -> data[self.registers -> data[reference1 + 3].r -> data[0].i * 3 - 2].d - self.compDistX * i), 'd'); // same x position as reference1 register
            list_append(self.positions, (unitype) (self.positions -> data[self.registers -> data[reference1 + 3].r -> data[0].i * 3 - 1].d + self.compDistY * self.registers -> data[reference1 + 2].i), 'd'); // y position of reference1 + compDist * number of references
            list_append(self.positions, (unitype) 0, 'd'); // facing upwards
            list_append(self.io, (unitype) 0, 'i');
            list_append(self.io, (unitype) 0, 'i');
            list_append(self.io, (unitype) 0, 'i');
            list_append(self.inpComp, self.compSlots -> data[list_find(self.compSlots, (unitype) "OR", 's') + 1], 'i');
            list_append(self.inpComp, wireTemp -> data[i], 'i');
            list_append(self.inpComp, wireTemp -> data[i + scaleFactor / 2], 'i');

            /* add wiring from reference register */
            list_append(self.wiring, wireTemp -> data[i], 'i');
            list_append(self.wiring, (unitype) (int) (self.components -> length - 1), 'i');
            list_append(self.wiring, (unitype) 0, 'i');

            list_append(self.wiring, wireTemp -> data[i + scaleFactor / 2], 'i');
            list_append(self.wiring, (unitype) (int) (self.components -> length - 1), 'i');
            list_append(self.wiring, (unitype) 0, 'i');

            wireTemp -> data[i].i = (int) (self.components -> length - 1);
            
        }
        self.registers -> data[reference1 + 2].i += 1;
        scaleFactor *= 2;
    }

    if (size1 != size2) { // tangent: one register is larger than the other
    maxUp = self.registers -> data[reference1 + 2].i;
    self.registers -> data[reference1 + 2].i = preMoveHeight;
    savedComp = self.components -> length - 1; // save this component for master OR
    /* OR the remaining bits */
    
    list_clear(wireTemp);
    for (int i = 0; i < smallerSize; i++) { // fill the first smallerSize of wireTemp stuff (we won't need this)
        list_append(wireTemp, self.registers -> data[biggerReg + 3].r -> data[i], 'i');
    }
    for (int i = smallerSize; i < biggerSize; i++) { // consider only bits that are above the ones XOR'd
        list_append(wireTemp, self.registers -> data[biggerReg + 3].r -> data[i], 'i');
    }
    self.registers -> data[biggerReg + 2].i += 1; // increment source register reference count by number of layers (incrementally)
    shrinkSize = biggerSize - smallerSize;
    scaleFactor = 2;
    while (shrinkSize > 0) {
        for (int i = smallerSize; i < biggerSize; i += scaleFactor) {
            list_append(self.components, (unitype) "OR", 's');
            list_append(self.positions, (unitype) (self.positions -> data[self.registers -> data[biggerReg + 3].r -> data[0].i * 3 - 2].d - self.compDistX * i), 'd'); // same x position as reference1 register
            list_append(self.positions, (unitype) (self.positions -> data[self.registers -> data[biggerReg + 3].r -> data[0].i * 3 - 1].d + self.compDistY * self.registers -> data[biggerReg + 2].i), 'd'); // y position of reference1 + compDist * number of references
            list_append(self.positions, (unitype) 0, 'd'); // facing upwards
            list_append(self.io, (unitype) 0, 'i');
            list_append(self.io, (unitype) 0, 'i');
            list_append(self.io, (unitype) 0, 'i');
            list_append(self.inpComp, self.compSlots -> data[list_find(self.compSlots, (unitype) "OR", 's') + 1], 'i');
            list_append(self.inpComp, wireTemp -> data[i], 'i');
            if (i + scaleFactor / 2 < wireTemp -> length)
                list_append(self.inpComp, wireTemp -> data[i + scaleFactor / 2], 'i');
            else
                list_append(self.inpComp, (unitype) 0, 'i');

            /* add wiring from reference register */
            list_append(self.wiring, wireTemp -> data[i], 'i');
            list_append(self.wiring, (unitype) (int) (self.components -> length - 1), 'i');
            list_append(self.wiring, (unitype) 0, 'i');

            if (i + scaleFactor / 2 < wireTemp -> length) {
                list_append(self.wiring, wireTemp -> data[i + scaleFactor / 2], 'i');
                list_append(self.wiring, (unitype) (int) (self.components -> length - 1), 'i');
                list_append(self.wiring, (unitype) 0, 'i');
            }

            wireTemp -> data[i].i = (int) (self.components -> length - 1);
            
        }
        self.registers -> data[biggerReg + 2].i += 1;
        scaleFactor *= 2;
        shrinkSize /= 2;
    }

    /* last OR for the XOR'd and extra bits */
    list_append(self.components, (unitype) "OR", 's');
    list_append(self.positions, (unitype) (self.positions -> data[self.registers -> data[reference1 + 3].r -> data[0].i * 3 - 2].d - self.compDistX * 0), 'd'); // same x position as reference1 register
    list_append(self.positions, (unitype) (self.positions -> data[self.registers -> data[reference1 + 3].r -> data[0].i * 3 - 1].d + self.compDistY * self.registers -> data[reference1 + 2].i), 'd'); // y position of reference1 + compDist * number of references
    list_append(self.positions, (unitype) 0, 'd'); // facing upwards
    list_append(self.io, (unitype) 0, 'i');
    list_append(self.io, (unitype) 0, 'i');
    list_append(self.io, (unitype) 0, 'i');
    list_append(self.inpComp, self.compSlots -> data[list_find(self.compSlots, (unitype) "OR", 's') + 1], 'i');
    list_append(self.inpComp, (unitype) savedComp, 'i');
    list_append(self.inpComp, (unitype) (int) (self.components -> length - 2), 'i'); // -2 refers to the last OR created for reg2, where -1 refers to this very OR component

    /* add wiring from reference register */
    list_append(self.wiring, (unitype) savedComp, 'i'); // create wire from savedComp to the OR
    list_append(self.wiring, (unitype) (int) (self.components -> length - 1), 'i');
    list_append(self.wiring, (unitype) 0, 'i');

    list_append(self.wiring, (unitype) (int) (self.components -> length - 2), 'i'); // create wire from last OR to this OR
    list_append(self.wiring, (unitype) (int) (self.components -> length - 1), 'i');
    list_append(self.wiring, (unitype) 0, 'i');

    self.registers -> data[reference1 + 2].i += 1;

    }

    /* master NOT and padding */
    list_append(self.components, (unitype) "NOT", 's');
    list_append(self.positions, (unitype) (self.positions -> data[self.registers -> data[reference1 + 3].r -> data[0].i * 3 - 2].d - self.compDistX * 0), 'd'); // same x position as reference1 register
    list_append(self.positions, (unitype) (self.positions -> data[self.registers -> data[reference1 + 3].r -> data[0].i * 3 - 1].d + self.compDistY * self.registers -> data[reference1 + 2].i), 'd'); // y position of reference1 + compDist * number of references
    list_append(self.positions, (unitype) 0, 'd'); // facing upwards
    list_append(self.io, (unitype) 0, 'i');
    list_append(self.io, (unitype) 0, 'i');
    list_append(self.io, (unitype) 0, 'i');
    list_append(self.inpComp, self.compSlots -> data[list_find(self.compSlots, (unitype) "NOT", 's') + 1], 'i');
    list_append(self.inpComp, (unitype) (int) (self.components -> length - 2), 'i');
    list_append(self.inpComp, (unitype) 0, 'i'); // -2 refers to the last OR created for reg2, where -1 refers to this very OR component

    /* add wiring from reference register */
    list_append(self.wiring, (unitype) (int) (self.components -> length - 2), 'i'); // create wire from savedComp to the OR
    list_append(self.wiring, (unitype) (int) (self.components -> length - 1), 'i');
    list_append(self.wiring, (unitype) 0, 'i');

    list_append(self.registers -> data[self.registers -> length - 1].r, (unitype) (int) (self.components -> length - 1), 'i'); // add this as the first bit of the output
    self.registers -> data[reference1 + 2].i += 1;

    for (int i = 1; i < smallerSize; i++) { // create n - 1 useless POWER blocks to act as other bits
        list_append(self.components, (unitype) "POWER", 's');
        list_append(self.positions, (unitype) (self.positions -> data[self.registers -> data[reference1 + 3].r -> data[0].i * 3 - 2].d - self.compDistX * i), 'd'); // same x position as reference register
        list_append(self.positions, (unitype) (self.positions -> data[self.registers -> data[reference1 + 3].r -> data[0].i * 3 - 1].d + self.compDistY * (self.registers -> data[reference1 + 2].i - 1)), 'd'); // y position of reference + compDist * number of references
        list_append(self.positions, (unitype) 0, 'd'); // facing upwards
        list_append(self.io, (unitype) 0, 'i');
        list_append(self.io, (unitype) 0, 'i');
        list_append(self.io, (unitype) 0, 'i');
        list_append(self.inpComp, self.compSlots -> data[list_find(self.compSlots, (unitype) "POWER", 's') + 1], 'i');
        list_append(self.inpComp, (unitype) 0, 'i');
        list_append(self.inpComp, (unitype) 0, 'i');
        list_append(self.registers -> data[self.registers -> length - 1].r, (unitype) (int) (self.components -> length - 1), 'i');
    }

    if (maxUp > self.registers -> data[reference1 + 2].i) {
        self.registers -> data[reference1 + 2].i = maxUp;
    }
    break;



    case 15: ; // piping
    if (size1 != size2) {
        printf("Warning: bit mismatch piping %s (%d bits) to %s (%d bits)\n", self.registers -> data[reference1].s, self.registers -> data[reference1 + 1].i, self.registers -> data[reference2].s, self.registers -> data[reference2 + 1].i);
    }
    for (int i = 0; i < smallerSize; i++) {
        self.inpComp -> data[self.registers -> data[reference1 + 3].r -> data[i].i * 3 - 1].i = self.registers -> data[reference2 + 3].r -> data[i].i; // for some reason power components accept inputs into their second input (?) (probably to do with the special case for power blocks)
        list_append(self.wiring, (unitype) self.registers -> data[reference2 + 3].r -> data[i], 'i');
        list_append(self.wiring, (unitype) self.registers -> data[reference1 + 3].r -> data[i], 'i');
        list_append(self.wiring, (unitype) 0, 'i');
    }
    break;




    case 21: ; // addition
    break;




    case 22: ; // subtraction
    break;




    case 23: ; // multiplication
    break;



    case 24: ; // division
    break;



    case 25: ; // modulo
    break;
    }
    list_free(wireTemp);
    *selfp = self;
}