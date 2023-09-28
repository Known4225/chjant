#include "seagateType.h"
/* NOTE:
remember to always strdup() when adding strings to lists, if you just add the same pointer then
a list can free the string that is also inside another list, so make sure you always strdup so
each list gets a copy of the string. In theory this is memory inefficient and there could be
ways to make it so they can share a pointer without conflict, but in practice finding those cases
is not worth the time. It would be cool if there was a programatic approach though. */

char *list_toString(list_t *list) { // turns a list of chars into a malloc'd string, should be part of standard list tbh
    char *out = malloc(sizeof(char) * (list -> length + 1));
    for (int i = 0; i < list -> length; i++) {
        out[i] = list -> data[i].c;
    }
    out[list -> length] = '\0';
    return out;
}

static inline char spaceDeprivedChar(char t) {
    if (t == ' ' || t == ',' || t == '=' || t == ';' || t == '{' || t == '[' || t == '(' || t == ')' || t == ']' || t == '}' || t == '+' || t == '-' || t == '*' || t == '/' || 
    t == '&' || t == '|' || t == '~' || t == '!' || t == '^' || t == '<' || t == '>' || t == '%' || t == '"' || t == '#' || t == '\n')
        return 1;
    return 0;
}

void loadKeywords(seagate *selfp) { // most of these are not useful for this particular application, but I may as well include them all
    seagate self = *selfp;
    printf("unitype size: %li\n", sizeof(unitype));
    list_append(self.keywords, (unitype) "auto", 's');
    list_append(self.keywords, (unitype) "break", 's');
    list_append(self.keywords, (unitype) "case", 's');
    list_append(self.keywords, (unitype) "char", 's');

    list_append(self.keywords, (unitype) "const", 's');
    list_append(self.keywords, (unitype) "continue", 's');
    list_append(self.keywords, (unitype) "default", 's');
    list_append(self.keywords, (unitype) "do", 's');

    list_append(self.keywords, (unitype) "double", 's');
    list_append(self.keywords, (unitype) "else", 's');
    list_append(self.keywords, (unitype) "enum", 's');
    list_append(self.keywords, (unitype) "extern", 's');

    list_append(self.keywords, (unitype) "float", 's');
    list_append(self.keywords, (unitype) "for", 's');
    list_append(self.keywords, (unitype) "goto", 's');
    list_append(self.keywords, (unitype) "if", 's');

    list_append(self.keywords, (unitype) "int", 's');
    list_append(self.keywords, (unitype) "long", 's');
    list_append(self.keywords, (unitype) "register", 's');
    list_append(self.keywords, (unitype) "return", 's');

    list_append(self.keywords, (unitype) "short", 's');
    list_append(self.keywords, (unitype) "signed", 's');
    list_append(self.keywords, (unitype) "sizeof", 's');
    list_append(self.keywords, (unitype) "static", 's');

    list_append(self.keywords, (unitype) "struct", 's');
    list_append(self.keywords, (unitype) "switch", 's');
    list_append(self.keywords, (unitype) "typedef", 's');
    list_append(self.keywords, (unitype) "union", 's');

    list_append(self.keywords, (unitype) "unsigned", 's');
    list_append(self.keywords, (unitype) "void", 's');
    list_append(self.keywords, (unitype) "volatile", 's');
    list_append(self.keywords, (unitype) "while", 's');

    self.compSlots = list_init();
    list_append(self.compSlots, (unitype) "null", 's');
    list_append(self.compSlots, (unitype) "POWER", 's');
    list_append(self.compSlots, (unitype) 1, 'i');
    list_append(self.compSlots, (unitype) "NOT", 's');
    list_append(self.compSlots, (unitype) 1, 'i');
    list_append(self.compSlots, (unitype) "AND", 's');
    list_append(self.compSlots, (unitype) 2, 'i');
    list_append(self.compSlots, (unitype) "OR", 's');
    list_append(self.compSlots, (unitype) 2, 'i');
    list_append(self.compSlots, (unitype) "XOR", 's');
    list_append(self.compSlots, (unitype) 2, 'i');
    list_append(self.compSlots, (unitype) "NOR", 's');
    list_append(self.compSlots, (unitype) 2, 'i');
    list_append(self.compSlots, (unitype) "NAND", 's');
    list_append(self.compSlots, (unitype) 2, 'i');
    list_append(self.compSlots, (unitype) "BUFFER", 's');
    list_append(self.compSlots, (unitype) 1, 'i');

    self.components = list_init();
    list_append(self.components, (unitype) "null", 's'); // they start with an 'n' char or "null" string so they are 1-indexed not 0-indexed because I'm a bad coder
    self.inpComp = list_init();
    list_append(self.inpComp, (unitype) 'n', 'c');
    self.io = list_init();
    list_append(self.io, (unitype) 'n', 'c');
    self.positions = list_init();
    list_append(self.positions, (unitype) 'n', 'c');
    self.wiring = list_init();
    list_append(self.wiring, (unitype) 'n', 'c');
    self.compDistX = 32;
    self.compDistY = 40; // slightly more for Y to make wires look a little better
    self.gateLeftX = 0;
    self.gateRightX = 0;


    self.quantities = list_init();
    self.subsect = list_init();
    self.order = list_init();
    self.toDoStack = list_init();
    self.deleteStack = list_init();

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
    21 - add
    22 - subtract
    23 - multiply
    24 - divide
    25 - modulo
    */
    self.operatorPrecedence = list_init();
    list_append(self.operatorPrecedence, (unitype) "*", 's');
    list_append(self.operatorPrecedence, (unitype) 3, 'i');
    list_append(self.operatorPrecedence, (unitype) 23, 'i');
    list_append(self.operatorPrecedence, (unitype) "/", 's');
    list_append(self.operatorPrecedence, (unitype) 3, 'i');
    list_append(self.operatorPrecedence, (unitype) 24, 'i');
    list_append(self.operatorPrecedence, (unitype) "%", 's');
    list_append(self.operatorPrecedence, (unitype) 3, 'i');
    list_append(self.operatorPrecedence, (unitype) 25, 'i');
    list_append(self.operatorPrecedence, (unitype) "+", 's');
    list_append(self.operatorPrecedence, (unitype) 4, 'i');
    list_append(self.operatorPrecedence, (unitype) 21, 'i');
    list_append(self.operatorPrecedence, (unitype) "-", 's');
    list_append(self.operatorPrecedence, (unitype) 4, 'i');
    list_append(self.operatorPrecedence, (unitype) 22, 'i');
    list_append(self.operatorPrecedence, (unitype) "<<", 's');
    list_append(self.operatorPrecedence, (unitype) 5, 'i');
    list_append(self.operatorPrecedence, (unitype) 9, 'i');
    list_append(self.operatorPrecedence, (unitype) ">>", 's');
    list_append(self.operatorPrecedence, (unitype) 5, 'i');
    list_append(self.operatorPrecedence, (unitype) 10, 'i');
    list_append(self.operatorPrecedence, (unitype) "==", 's');
    list_append(self.operatorPrecedence, (unitype) 7, 'i');
    list_append(self.operatorPrecedence, (unitype) 14, 'i');
    list_append(self.operatorPrecedence, (unitype) "!=", 's');
    list_append(self.operatorPrecedence, (unitype) 7, 'i');
    list_append(self.operatorPrecedence, (unitype) 8, 'i');
    list_append(self.operatorPrecedence, (unitype) "&", 's');
    list_append(self.operatorPrecedence, (unitype) 8, 'i');
    list_append(self.operatorPrecedence, (unitype) 2, 'i');
    list_append(self.operatorPrecedence, (unitype) "^", 's');
    list_append(self.operatorPrecedence, (unitype) 9, 'i');
    list_append(self.operatorPrecedence, (unitype) 4, 'i');
    list_append(self.operatorPrecedence, (unitype) "|", 's');
    list_append(self.operatorPrecedence, (unitype) 10, 'i');
    list_append(self.operatorPrecedence, (unitype) 3, 'i');
    list_append(self.operatorPrecedence, (unitype) "&&", 's');
    list_append(self.operatorPrecedence, (unitype) 11, 'i');
    list_append(self.operatorPrecedence, (unitype) 6, 'i');
    list_append(self.operatorPrecedence, (unitype) "||", 's');
    list_append(self.operatorPrecedence, (unitype) 12, 'i');
    list_append(self.operatorPrecedence, (unitype) 7, 'i');

    list_append(self.operatorPrecedence, (unitype) "=", 's'); // added this type of assignment to do last, returns the second register, also does a check, opCode 30
    list_append(self.operatorPrecedence, (unitype) 13, 'i');
    list_append(self.operatorPrecedence, (unitype) 30, 'i');
    *selfp = self;
}

void exportGates(seagate *selfp, const char *filename) { // exports a logicgates file
    seagate self = *selfp;
    FILE *file = fopen(filename, "w+");
        for (int i = 1; i < self.components -> length; i++)
            fprintf(file, "%s ", self.components -> data[i].s);
        for (int i = 1; i < self.positions -> length; i++)
            fprintf(file, "%.0lf ", self.positions -> data[i].d);
        for (int i = 1; i < self.io -> length; i++)
            fprintf(file, "%d ", self.io -> data[i].i);
        for (int i = 1; i < self.inpComp -> length; i++)
            fprintf(file, "%d ", self.inpComp -> data[i].i);
        for (int i = 1; i < self.wiring -> length; i++)
            fprintf(file, "%d ", self.wiring -> data[i].i);
        printf("Successfully saved to %s\n", filename);
        fclose(file);
    *selfp = self;
}

void readString(seagate *selfp) { // parses pstr into strData and syntaxHighlight
    seagate self = *selfp;
    list_t *tempStr = list_init();
    self.charPtr = 0;
    while (self.charPtr < self.pstr -> length) {
        if (spaceDeprivedChar(self.pstr -> data[self.charPtr].c)) {
            if (tempStr -> length > 0)
                list_append(self.strData, (unitype) list_toString(tempStr), 's');
            list_clear(tempStr);
            list_append(self.syntaxic, (unitype) (int) self.strData -> length, 'i');
            list_append(self.syntaxic, self.pstr -> data[self.charPtr], 'c');
        } else {
            list_append(tempStr, self.pstr -> data[self.charPtr], 'c');
        }
        self.charPtr += 1;
    }
    list_print(self.strData);
    list_print(self.syntaxic);
    list_free(tempStr);
    *selfp = self;
}

char checkSyntax(seagate *selfp, int position, int column) { // returns the syntaxic divider between position - 1 and position in the code. Sometimes there are multiple dividers, column asks for the 1st, 2nd, 3rd, etc
    seagate self = *selfp;
    char keep = 0;
    for (int i = 0; i < self.syntaxic -> length; i += 2) {
        if (self.syntaxic -> data[i].i == position) {
            keep += 1;
            if (keep == column)
                return self.syntaxic -> data[i + 1].c;
        }
        if (self.syntaxic -> data[i].i > position)
            return '\0';
    }
    return '\0';
}

int setSyntax(seagate *selfp, int position, int column, char toSet) { // haha, getters and setters in C
    seagate self = *selfp;
    char keep = 0;
    for (int i = 0; i < self.syntaxic -> length; i += 2) {
        if (self.syntaxic -> data[i].i == position) {
            keep += 1;
            if (keep == column) {
                self.syntaxic -> data[i + 1].c = toSet;
                *selfp = self;
                return 0;
            }
        }
        if (self.syntaxic -> data[i].i > position)
            return -1;
    }
    return -1;
}

int checkType(seagate *selfp) { // returns the type specified at strPtr (in the form described in the type indicators section). Returns 0 if there is no type
    seagate self = *selfp;
    self.typeSize = 0;
    int out = 0x80000000;
    while (strcmp(self.strData -> data[self.strPtr].s, "register") == 0 || 
    strcmp(self.strData -> data[self.strPtr].s, "volatile") == 0 ||
    strcmp(self.strData -> data[self.strPtr].s, "auto") == 0 ||
    strcmp(self.strData -> data[self.strPtr].s, "extern") == 0 ||
    strcmp(self.strData -> data[self.strPtr].s, "static") == 0 ||
    strcmp(self.strData -> data[self.strPtr].s, "const") == 0) { // we pretty much ignore all of this
        self.strPtr += 1;
    }
    if (strcmp(self.strData -> data[self.strPtr].s, "signed") == 0) {
        self.strPtr += 1;
    }
    if (strcmp(self.strData -> data[self.strPtr].s, "unsigned") == 0) {
        out &= 0x7fffffff; // deleted the signed flag
        self.strPtr += 1;
    }
    if (strcmp(self.strData -> data[self.strPtr].s, "char") == 0) {
        out |= 0x1;
        self.strPtr += 1;
        self.typeSize = 8;
    } else {
    if (strcmp(self.strData -> data[self.strPtr].s, "short") == 0) {
        out |= 0x2;
        self.strPtr += 1;
        self.typeSize = 16;
        if (strcmp(self.strData -> data[self.strPtr].s, "int") == 0) {
            self.strPtr += 1;
        }
    } else {
    if (strcmp(self.strData -> data[self.strPtr].s, "int") == 0) {
        out |= 0x4;
        self.strPtr += 1;
        self.typeSize = 32;
    } else {
    if (strcmp(self.strData -> data[self.strPtr].s, "long") == 0) {
        self.strPtr += 1;
        if (strcmp(self.strData -> data[self.strPtr].s, "int") == 0) {
            out |= 0x4;
            self.strPtr += 1;
            self.typeSize = 32;
        } else {
        if (strcmp(self.strData -> data[self.strPtr].s, "long") == 0) {
            out |= 0x8;
            self.strPtr += 1;
            self.typeSize = 64;
            if (strcmp(self.strData -> data[self.strPtr].s, "int") == 0) {
                self.strPtr += 1;
            }
        } else {
        out |= 0x4;
        }    
        }
    } else {
    if (strcmp(self.strData -> data[self.strPtr].s, "float") == 0) {
        out |= 0x10;
        self.strPtr += 1;
        self.typeSize = 32;
    } else {
    if (strcmp(self.strData -> data[self.strPtr].s, "double") == 0) {
        out |= 0x20;
        self.strPtr += 1;
        self.typeSize = 64;
    } else {
    out = 0;
    } // clean coding
    } // good use of syntax
    } // maintainable code
    } // only triple nested !
    } // amazing
    } // yes
    if (out != 0) { // get rid of training "const"s and others
        while (strcmp(self.strData -> data[self.strPtr].s, "register") == 0 || 
        strcmp(self.strData -> data[self.strPtr].s, "volatile") == 0 ||
        strcmp(self.strData -> data[self.strPtr].s, "auto") == 0 ||
        strcmp(self.strData -> data[self.strPtr].s, "extern") == 0 ||
        strcmp(self.strData -> data[self.strPtr].s, "static") == 0 ||
        strcmp(self.strData -> data[self.strPtr].s, "const") == 0) { // we pretty much ignore all of this
            self.strPtr += 1;
        }
    }
    *selfp = self;
    return out;
}

int associatedOperation(seagate *selfp, list_t *check, int app) { // lookup for operations, set app to 1 to get the precendence, set app to 2 to get the operation code
    seagate self = *selfp;
    int out = 0;
    char *tstr = list_toString(check);
    for (int i = 0; i < self.operatorPrecedence -> length; i += 3) {
        if (strcmp(tstr, self.operatorPrecedence -> data[i].s) == 0) {
            out = self.operatorPrecedence -> data[i + app].i;
        }
    }
    free(tstr);
    return out;
}

int checkNamespace(seagate *selfp, char *findName) { // returns the register reference of the given namespace (pattern match namespace)
    seagate self = *selfp;
    for (int i = 0; i < self.registers -> length; i += 4) {
        if (strcmp(self.registers -> data[i].s, findName) == 0) {
            return i;
        }
    }
    return -1;
    *selfp = self;
}

int checkConstant(seagate *selfp, char *input) { // returns the best guess at a constant from a string (pattern match constants)
    seagate self = *selfp;
    self.constantType = 0;
    self.constant = (unitype) 0;
    char containsDot = 0;
    for (int i = 0; i < strlen(input); i++) {
        if (input[i] == '.') {
            containsDot = 1;
        }
        if (input[i] < 46 || input[i] > 58) {
            return -1;
        }
    }
    float dTest;
    int lTest;
    if (containsDot) {
        /* this is a float */
        sscanf(input, "%f", &dTest);
        self.constant = (unitype) dTest;
        self.constantType = 0x10;
    } else {
        /* this is an integer */
        sscanf(input, "%d", &lTest);
        self.constant = (unitype) lTest;
        self.constantType = 0;
        self.constantType |= 0x4;
        self.constantType |= 0x80000000;
    }
    *selfp = self;
    return 0;
}

void recordRegFromNamespace(seagate *selfp, int currentType, int varOrFunc, int sideOrDown) { // creates a register from the last added namespace
    seagate self = *selfp;
    list_append(self.userNamespace, (unitype) strdup(self.strData -> data[self.strPtr].s), 's'); // for lookups
    list_append(self.namespace, (unitype) strdup(self.strData -> data[self.strPtr].s), 's'); // input1
    self.strPtr += 1; // skip input1
    list_append(self.namespace, (unitype) currentType, 'i'); // add input1 type to namespaces
    list_append(self.namespace, (unitype) varOrFunc, 'i'); // add '0' to indicate variable
    int len = self.namespace -> length;
    list_append(self.registers, (unitype) strdup(self.namespace -> data[len - 3].s), 's');
    int type = self.namespace -> data[len - 2].i;
    int size = 0; // size of register in bits
    if (type & 0x1)
        size = 8;
    if (type & 0x2)
        size = 16;
    if (type & 0x4)
        size = 32;
    if (type & 0x8)
        size = 64;
    if (type & 0x10)
        size = 32;
    if (type & 0x20)
        size = 64;
    list_append(self.registers, (unitype) size, 'i');
    list_append(self.registers, (unitype) 0, 'i'); // 0 references to this register so far
    list_append(self.registers, (unitype) list_init(), 'r'); // list of handles for registers
    /* registers made by namespaces are all power blocks facing upward. They start at 0, self.ypos (to separate input, outputs, and pad registers) and move rightward size bits, with each gate being compDist right of the last */
    for (int i = 0; i < size; i++) {
        list_append(self.registers -> data[self.registers -> length - 1].r, (unitype) (int) self.components -> length, 'i');
        list_append(self.components, (unitype) "POWER", 's');
        if (self.registers -> length == 4) {
            list_append(self.positions, (unitype) (0 - self.compDistX * i), 'd');
            list_append(self.positions, (unitype) (-self.compDistY), 'd');
            list_append(self.positions, (unitype) 180.0, 'd');
        } else {
            list_append(self.positions, (unitype) (self.gateLeftX - self.compDistX * i), 'd');
            list_append(self.positions, (unitype) self.compDistY, 'd');
            list_append(self.positions, (unitype) 0, 'd');
        }
        list_append(self.io, (unitype) 0, 'i');
        list_append(self.io, (unitype) 0, 'i');
        list_append(self.io, (unitype) 0, 'i');
        list_append(self.inpComp, self.compSlots -> data[list_find(self.compSlots, (unitype) "POWER", 's') + 1], 'i');
        list_append(self.inpComp, (unitype) 0, 'i');
        list_append(self.inpComp, (unitype) 0, 'i');
    }
    if (self.registers -> length != 4)
        self.gateLeftX -= self.compDistX * (size + 2);
    // printf("registers: ");
    // list_print(self.registers);
    *selfp = self;
}

extern void recordPaddedReg(seagate *selfp, int reference1, int reference2, int operation); // prototype for this function which has its own file because life is suffering

int packageExpression(seagate *selfp) { // the parsing
    seagate self = *selfp;
    int index = self.strPtr; // for errors
    list_clear(self.quantities);
    char synt;
    int step;
    char end = 0;
    while (!end) {
        list_t *tempList = list_init();
        step = 1;
        synt = checkSyntax(&self, self.strPtr, step);
        while (synt != '\0') {
            if (synt == ';') {
                end = 1;
                break;
            }
            if (synt != ' ') {
                list_append(tempList, (unitype) synt, 'c');
            }
            step += 1;
            synt = checkSyntax(&self, self.strPtr, step);
        }
        if (tempList -> length > 0)
            list_append(self.quantities, (unitype) tempList, 'r');
        if (!end)
            list_append(self.quantities, (unitype) strdup(self.strData -> data[self.strPtr].s), 's');
        self.strPtr += 1;
    }
    self.strPtr -= 1;
    // printf("strPtr (%d): %s\n", self.strPtr, self.strData -> data[self.strPtr].s);
    // printf("quantities (begin expression): ");
    // list_print(self.quantities);
    // list_copy(self.quantities, self.subsect);

    /* routine: break into subsection */
    list_clear(self.toDoStack); // represents a list of instructions to perform after the expression has been evaluated
    list_clear(self.deleteStack); // an expression essentially creates a stack frame of temporary registers that need to be deleted when they are finished, this list has both a string and an index in that order, the index is used to delete back-to-front, the string is for safety
    while (self.quantities -> length > 0) {
    printf("quantities: ");
    list_print(self.quantities);
    list_clear(self.subsect);
    int secondPointer = 0;
    int firstPointer = 0;
    char run = 1;
    list_t *leftover = list_init();
    while (run) {
        if (self.quantities -> type[secondPointer] == 'r') {
            list_t *paraList = list_init();
            for (int i = 0; i < self.quantities -> data[secondPointer].r -> length; i++) {
                if (self.quantities -> data[secondPointer].r -> data[i].c == ')') {
                    run = 0;
                    list_t *afterP = list_init();
                    for (int j = i + 1; j < self.quantities -> data[secondPointer].r -> length; j++) {
                        list_append(afterP, self.quantities -> data[secondPointer].r -> data[j], 'c');
                    }
                    if (afterP -> length == 0) {
                        list_delete(self.quantities, secondPointer);
                    } else {
                        list_free(self.quantities -> data[secondPointer].r);
                        self.quantities -> data[secondPointer].r = afterP;
                    }
                    secondPointer -= 1;
                    break;
                }
                if (self.quantities -> data[secondPointer].r -> data[i].c == '(') {
                    firstPointer = secondPointer + 1;
                    // printf("paraList: ");
                    // list_print(paraList);
                    list_clear(leftover);
                    for (int j = 0; j < i; j++) {
                        list_append(leftover, self.quantities -> data[secondPointer].r -> data[j], 'c');
                    }
                    list_clear(self.subsect);
                    list_clear(paraList);
                } else {
                    list_append(paraList, self.quantities -> data[secondPointer].r -> data[i], 'c');
                }
            }
            if (!run) {
                break;
            }
            if (paraList -> length != 0)
                list_append(self.subsect, (unitype) paraList, 'r');
        } else {
            list_append(self.subsect, (unitype) strdup(self.quantities -> data[secondPointer].s), 's');
        }
        secondPointer += 1;
        if (secondPointer >= self.quantities -> length) {
            if (firstPointer == 0) {
                secondPointer -= 1;
                if (self.quantities -> type[secondPointer] == 'r')
                    secondPointer -= 1;
                break;
            }
            printf("Syntax error: no matching ')' at word %d\n", index);
            return -1;
        }
    }
    if (self.quantities -> type[firstPointer - 1] == 'r') {
        list_free(self.quantities -> data[firstPointer - 1].r);
        self.quantities -> data[firstPointer - 1].r = leftover;
        if (leftover -> length == 0) {
            list_delete(self.quantities, firstPointer - 1);
            firstPointer -= 1;
            secondPointer -= 1;
        }
    } else {
        list_free(leftover);
    }
    
    printf("subsect: ");
    list_print(self.subsect);

    /* first pass: namespace checking and initialise constants */
    for (int j = 0; j < self.subsect -> length; j++) {
        if (self.subsect -> type[j] == 's') {
            if (checkNamespace(&self, self.subsect -> data[j].s) == -1) {
                /* we assume this is a constant if it does not already have a namespace */
                if (checkConstant(&self, self.subsect -> data[j].s) == -1) {
                    printf("Error parsing constant %s, no matching namespace\n", self.subsect -> data[j].s);
                    return -1;
                }
                if (self.constantType == 0x10) {
                    /* it's a float, we don't know how to handle that */
                } else {
                    /* it's a signed integer. That's doable */
                    int constSize = 32;
                    char *conName = malloc(20); // need the conName to match the constant's string form exactly, since we will use it for namespace lookup
                    // conName[0] = 'C';
                    // conName[1] = 'O';
                    // conName[2] = 'N';
                    sprintf(conName, "%lld", self.constant.l);
                    list_append(self.registers, (unitype) conName, 's'); // create a new register to hold the constant value
                    list_append(self.registers, (unitype) constSize, 'i');
                    list_append(self.registers, (unitype) 0, 'i'); // 0 references to this register so far
                    list_append(self.registers, (unitype) list_init(), 'r'); // list of handles for registers
                    // do not add to deleteStack, since constants are kept as globals
                    for (int i = 0; i < constSize; i++) {
                        list_append(self.registers -> data[self.registers -> length - 1].r, (unitype) (int) self.components -> length, 'i');
                        list_append(self.components, (unitype) "POWER", 's');
                        list_append(self.positions, (unitype) (self.gateLeftX - self.compDistX * i), 'd'); // constants go to the left of input registers
                        list_append(self.positions, (unitype) self.compDistY, 'd');
                        list_append(self.positions, (unitype) 0, 'd');
                        list_append(self.io, (unitype) 0, 'i');
                        list_append(self.io, (unitype) 0, 'i');
                        list_append(self.io, (unitype) 0, 'i');
                        list_append(self.inpComp, self.compSlots -> data[list_find(self.compSlots, (unitype) "POWER", 's') + 1], 'i');
                        list_append(self.inpComp, (unitype) 0, 'i');
                        list_append(self.inpComp, (unitype) 0, 'i');
                    }
                    if (self.registers -> length != 4)
                        self.gateLeftX -= self.compDistX * (constSize + 2);
                    long long value = self.constant.l; // load the value into the register
                    unsigned long long mask = (unsigned long long) 1 << (constSize - 1);
                    for (int i = 0; i < constSize; i++) {
                        if (value & mask) {
                            self.io -> data[self.io -> length - 2 - 3 * i].i = 1; // set the bits
                        }
                        mask >>= 1;
                    }

                }
            } else {
                /* this namespace has been defined */
            }
        }
    }

    /* second pass: modify registers */
    /* gonna use a stack-based approach */
    step = 0;
    while (step < self.subsect -> length) {
        if (self.subsect -> type[step] == 'r') {
            // printf("parsing: ");
            // list_print(self.subsect -> data[step].r);
            if (self.subsect -> data[step].r -> length == 2 && self.subsect -> data[step].r -> data[0].c == '+' && self.subsect -> data[step].r -> data[1].c == '+') {
                if (step == 0) {
                    printf("hit prefix ++ case\n");
                    if (list_count(self.userNamespace, self.subsect -> data[step + 1], 's') == 0) {
                        printf("Syntax Error (%d): cannot increment %s: expression must be a modifiable lvalue\n", index, self.subsect -> data[step + 1].s);
                        return -1;
                    }
                    recordPaddedReg(&self, checkNamespace(&self, self.subsect -> data[step + 1].s), 0, 11);
                    recordPaddedReg(&self, checkNamespace(&self, self.subsect -> data[step + 1].s), self.registers -> length - 4, 0); // replaces the entire register with the temporary that is incremented by 1
                    list_delete(self.subsect -> data[step].r, 0);
                    list_delete(self.subsect -> data[step].r, 0);
                } else {
                    if (step == self.subsect -> length - 1) {
                        printf("hit posfix ++ case\n");
                        if (list_count(self.userNamespace, self.subsect -> data[step - 1], 's') == 0) {
                            printf("Syntax Error (%d): cannot increment %s: expression must be a modifiable lvalue\n", index, self.subsect -> data[step - 1].s);
                            return -1;
                        }
                        recordPaddedReg(&self, checkNamespace(&self, self.subsect -> data[step - 1].s), 0, 11); // creates a copy with the INC suffix
                        list_append(self.toDoStack, (unitype) strdup(self.subsect -> data[step - 1].s), 's'); // makes a note to increment this by one after the expression is evaluated
                        // self.subsect -> data[step - 1].s = realloc(self.subsect -> data[step - 1].s, strlen(self.subsect -> data[step - 1].s + 4));
                        // memcpy(self.subsect -> data[step - 1].s + strlen(self.subsect -> data[step - 1].s), "INC", 4); // replaces the copy in the working version with the temporary that is incremented (apparently this is not how C works)
                        char *renamedString = malloc(strlen(self.subsect -> data[step - 1].s) + 4);
                        memcpy(renamedString, self.subsect -> data[step - 1].s, strlen(self.subsect -> data[step - 1].s));
                        memcpy(renamedString + strlen(self.subsect -> data[step - 1].s), "INC", 4);
                        list_append(self.toDoStack, (unitype) renamedString, 's'); // note part 2
                        list_delete(self.subsect -> data[step].r, 0);
                        list_delete(self.subsect -> data[step].r, 0);
                    } else {
                        list_delete(self.subsect -> data[step].r, 0);
                    }
                }
            }
            if (self.subsect -> data[step].r -> length == 2 && self.subsect -> data[step].r -> data[0].c == '-' && self.subsect -> data[step].r -> data[1].c == '-') {
                if (step == 0) {
                    if (list_count(self.userNamespace, self.subsect -> data[step + 1], 's') == 0) {
                        printf("Syntax Error (%d): cannot decrement %s: expression must be a modifiable lvalue\n", index, self.subsect -> data[step + 1].s);
                        return -1;
                    }
                    recordPaddedReg(&self, checkNamespace(&self, self.subsect -> data[step + 1].s), 0, 12);
                    recordPaddedReg(&self, checkNamespace(&self, self.subsect -> data[step + 1].s), self.registers -> length - 4, 0); // replaces the entire register with the temporary that is decremented by 1
                    list_delete(self.subsect -> data[step].r, 0);
                    list_delete(self.subsect -> data[step].r, 0);
                } else {
                    if (step == self.subsect -> length - 1) {
                        if (list_count(self.userNamespace, self.subsect -> data[step - 1], 's') == 0) {
                            printf("Syntax Error (%d): cannot decrement %s: expression must be a modifiable lvalue\n", index, self.subsect -> data[step - 1].s);
                            return -1;
                        }
                        recordPaddedReg(&self, checkNamespace(&self, self.subsect -> data[step - 1].s), 0, 12); // creates a copy with the DEC suffix
                        list_append(self.toDoStack, (unitype) strdup(self.subsect -> data[step - 1].s), 's'); // makes a note to decrement this by one after the expression is evaluated
                        // self.subsect -> data[step - 1].s = realloc(self.subsect -> data[step - 1].s, strlen(self.subsect -> data[step - 1].s + 4));
                        // memcpy(self.subsect -> data[step - 1].s + strlen(self.subsect -> data[step - 1].s), "DEC", 4); // replaces the copy in the working version with the temporary that is decremented (apparently this is not how C works)
                        char *renamedString = malloc(strlen(self.subsect -> data[step - 1].s) + 4);
                        memcpy(renamedString, self.subsect -> data[step - 1].s, strlen(self.subsect -> data[step - 1].s));
                        memcpy(renamedString + strlen(self.subsect -> data[step - 1].s), "DEC", 4);
                        list_append(self.toDoStack, (unitype) renamedString, 's'); // note part 2
                        list_delete(self.subsect -> data[step].r, 0);
                        list_delete(self.subsect -> data[step].r, 0);
                    } else {
                        list_delete(self.subsect -> data[step].r, 0); // subtracting the negative is the same as adding
                        self.subsect -> data[step].r -> data[0].c = '+';
                    }
                }
            }
            if (self.subsect -> data[step].r -> length == 2 && self.subsect -> data[step].r -> data[0].c == '+' && self.subsect -> data[step].r -> data[1].c == '-') {
                if (step == 0) {
                    recordPaddedReg(&self, checkNamespace(&self, self.subsect -> data[step + 1].s), 0, 13); // creates a copy with the NEG suffix
                    self.subsect -> data[step + 1].s = realloc(self.subsect -> data[step + 1].s, strlen(self.subsect -> data[step - 1].s + 4));
                    memcpy(self.subsect -> data[step + 1].s + strlen(self.subsect -> data[step + 1].s), "NEG", 4); // replaces the copy in the working version with the temporary that is negated
                    list_delete(self.subsect -> data[step].r, 0);
                    list_delete(self.subsect -> data[step].r, 0);
                } else {
                    list_delete(self.subsect -> data[step].r, 0); // just subtract
                }
            }
            if (self.subsect -> data[step].r -> length == 2 && self.subsect -> data[step].r -> data[0].c == '-' && self.subsect -> data[step].r -> data[1].c == '+') {
                if (step == 0) {
                    recordPaddedReg(&self, checkNamespace(&self, self.subsect -> data[step + 1].s), 0, 13); // creates a copy with the NEG suffix
                    self.subsect -> data[step + 1].s = realloc(self.subsect -> data[step + 1].s, strlen(self.subsect -> data[step - 1].s + 4));
                    memcpy(self.subsect -> data[step + 1].s + strlen(self.subsect -> data[step + 1].s), "NEG", 4); // replaces the copy in the working version with the temporary that is negated
                    list_delete(self.subsect -> data[step].r, 0);
                    list_delete(self.subsect -> data[step].r, 0);
                } else {
                    list_delete(self.subsect -> data[step].r, 1); // just subtract
                }
            }
            if (self.subsect -> data[step].r -> length == 1 && self.subsect -> data[step].r -> data[0].c == '-') {
                if (step == 0) {
                    printf("negating %s\n", self.subsect -> data[step + 1].s);
                    recordPaddedReg(&self, checkNamespace(&self, self.subsect -> data[step + 1].s), 0, 13); // creates a copy with the NEG suffix
                    self.subsect -> data[step + 1].s = realloc(self.subsect -> data[step + 1].s, strlen(self.subsect -> data[step + 1].s + 4));
                    memcpy(self.subsect -> data[step + 1].s + strlen(self.subsect -> data[step + 1].s), "NEG", 4); // replaces the copy in the working version with the temporary that is negated
                    list_delete(self.subsect -> data[step].r, 0);
                }
            }
            if (step == self.subsect -> length - 1) {
                list_delete(self.subsect, step); // delete this item from list
                break;
            }
            int bitwiseCount = 0;
            int logicCount = 0;
            if (step == 0) { // get rid of logical and bitwise NOTs
                for (int i = 0; i < self.subsect -> data[step].r -> length; i++) {
                    if (self.subsect -> data[step].r -> data[i].c == '!') {
                        logicCount += 1;
                    } else {
                    if (self.subsect -> data[step].r -> data[i].c == '~') {
                        bitwiseCount += 1;
                    } else {
                        printf("Error, unknown character at word %d\n", index);
                        return -1;
                    }   
                    }
                }
                if (bitwiseCount % 2) {
                    recordPaddedReg(&self, checkNamespace(&self, self.subsect -> data[step + 1].s), 0, 1); // creates a copy with the NOT suffix
                    self.subsect -> data[step + 1].s = realloc(self.subsect -> data[step + 1].s, strlen(self.subsect -> data[step + 1].s + 4));
                    memcpy(self.subsect -> data[step + 1].s + strlen(self.subsect -> data[step + 1].s), "NOT", 4); // replaces the copy in the working version with the temporary that is notted (bitwise)
                }
                if (logicCount % 2) {
                    recordPaddedReg(&self, checkNamespace(&self, self.subsect -> data[step + 1].s), 0, 5); // creates a copy with the N suffix
                    self.subsect -> data[step + 1].s = realloc(self.subsect -> data[step + 1].s, strlen(self.subsect -> data[step + 1].s + 2));
                    memcpy(self.subsect -> data[step + 1].s + strlen(self.subsect -> data[step + 1].s), "N", 2); // replaces the copy in the working version with the temporary that is notted (logical)
                }
                list_clear(self.subsect -> data[step].r);
                list_delete(self.subsect, step); // delete this item from list
            }
            if (step != 0 && step != self.subsect -> length - 1 && self.subsect -> data[step].r -> length > 2) {
                if (self.subsect -> data[step].r -> data[0].c == '+' && self.subsect -> data[step].r -> data[1].c == '+') {
                    if (step == 0) {
                        printf("Syntax error at word %d\n", index);
                        return -1;
                    } else {
                        if (list_count(self.userNamespace, self.subsect -> data[step - 1], 's') == 0) {
                            printf("Syntax Error (%d): cannot increment %s: expression must be a modifiable lvalue\n", index, self.subsect -> data[step - 1].s);
                            return -1;
                        }
                        recordPaddedReg(&self, checkNamespace(&self, self.subsect -> data[step - 1].s), 0, 11); // creates a copy with the INC suffix
                        list_append(self.toDoStack, (unitype) strdup(self.subsect -> data[step - 1].s), 's'); // makes a note to increment this by one after the expression is evaluated
                        self.subsect -> data[step - 1].s = realloc(self.subsect -> data[step - 1].s, strlen(self.subsect -> data[step - 1].s + 4));
                        // memcpy(self.subsect -> data[step - 1].s + strlen(self.subsect -> data[step - 1].s), "INC", 4); // replaces the copy in the working version with the temporary that is incremented by 1
                        list_append(self.toDoStack, (unitype) strdup(self.subsect -> data[step - 1].s), 's'); // note part 2
                        list_delete(self.subsect -> data[step].r, 0);
                        list_delete(self.subsect -> data[step].r, 0);
                    }
                }
                if (self.subsect -> data[step].r -> data[0].c == '-' && self.subsect -> data[step].r -> data[1].c == '-') {
                    if (step == 0) {
                        printf("Syntax error at word %d\n", index);
                        return -1;
                    } else {
                        if (list_count(self.userNamespace, self.subsect -> data[step - 1], 's') == 0) {
                            printf("Syntax Error (%d): cannot decrement %s: expression must be a modifiable lvalue\n", index, self.subsect -> data[step - 1].s);
                            return -1;
                        }
                        recordPaddedReg(&self, checkNamespace(&self, self.subsect -> data[step - 1].s), 0, 12); // creates a copy with the DEC suffix
                        list_append(self.toDoStack, (unitype) strdup(self.subsect -> data[step - 1].s), 's'); // makes a note to increment this by one after the expression is evaluated
                        self.subsect -> data[step - 1].s = realloc(self.subsect -> data[step - 1].s, strlen(self.subsect -> data[step - 1].s + 4));
                        // memcpy(self.subsect -> data[step - 1].s + strlen(self.subsect -> data[step - 1].s), "DEC", 4); // replaces the copy in the working version with the temporary that is decremented by 1
                        list_append(self.toDoStack, (unitype) strdup(self.subsect -> data[step - 1].s), 's'); // note part 2
                        list_delete(self.subsect -> data[step].r, 0);
                        list_delete(self.subsect -> data[step].r, 0);
                    }
                }
                if (self.subsect -> data[step].r -> data[0].c == '+' && self.subsect -> data[step].r -> data[1].c == '+') {
                    if (step == 0) {
                        printf("Syntax error at word %d\n", index);
                        return -1;
                    } else {
                        if (list_count(self.userNamespace, self.subsect -> data[step + 1], 's') == 0) {
                            printf("Syntax Error (%d): cannot increment %s: expression must be a modifiable lvalue\n", index, self.subsect -> data[step + 1].s);
                            return -1;
                        }
                        recordPaddedReg(&self, checkNamespace(&self, self.subsect -> data[step + 1].s), 0, 11);
                        recordPaddedReg(&self, checkNamespace(&self, self.subsect -> data[step + 1].s), self.registers -> length - 4, 0);
                        list_delete(self.subsect -> data[step].r, 0);
                        list_delete(self.subsect -> data[step].r, 0);
                    }
                }
                if (self.subsect -> data[step].r -> data[0].c == '-' && self.subsect -> data[step].r -> data[1].c == '-') {
                    if (step == 0) {
                        printf("Syntax error at word %d\n", index);
                        return -1;
                    } else {
                        if (list_count(self.userNamespace, self.subsect -> data[step + 1], 's') == 0) {
                            printf("Syntax Error (%d): cannot decrement %s: expression must be a modifiable lvalue\n", index, self.subsect -> data[step + 1].s);
                            return -1;
                        }
                        recordPaddedReg(&self, checkNamespace(&self, self.subsect -> data[step + 1].s), 0, 12);
                        recordPaddedReg(&self, checkNamespace(&self, self.subsect -> data[step + 1].s), self.registers -> length - 4, 0);
                        list_delete(self.subsect -> data[step].r, 0);
                        list_delete(self.subsect -> data[step].r, 0);
                    }
                }
                /* assume there is no more extra pluses and minuses */
                if (list_count(self.subsect -> data[step].r, (unitype) '+', 'c') + list_count(self.subsect -> data[step].r, (unitype) '-', 'c') > 1) {
                    printf("syntax error: too many +/- at word %d\n", index);
                    return -1;
                }
            }

            /* stack approach */
            if (step != 0 && step != self.subsect -> length - 1) {
                bitwiseCount = 0;
                logicCount = 0;
                int tin = self.subsect -> data[step].r -> length - 1;
                // get rid of logical and bitwise NOTs
                while (self.subsect -> data[step].r -> data[tin].c == '!' || self.subsect -> data[step].r -> data[tin].c == '~') {
                    if (self.subsect -> data[step].r -> data[tin].c == '!') {
                        logicCount += 1;
                    } else {
                    if (self.subsect -> data[step].r -> data[tin].c == '~') {
                        bitwiseCount += 1;
                    }
                    }
                    list_delete(self.subsect -> data[step].r, tin);
                    tin -= 1;
                }
                if (bitwiseCount % 2) {
                    recordPaddedReg(&self, checkNamespace(&self, self.subsect -> data[step + 1].s), 0, 1); // creates a copy with the NOT suffix
                    self.subsect -> data[step + 1].s = realloc(self.subsect -> data[step + 1].s, strlen(self.subsect -> data[step + 1].s + 4));
                    memcpy(self.subsect -> data[step + 1].s + strlen(self.subsect -> data[step + 1].s), "NOT", 4); // replaces the copy in the working version with the temporary that is notted (bitwise)
                }
                if (logicCount % 2) {
                    recordPaddedReg(&self, checkNamespace(&self, self.subsect -> data[step + 1].s), 0, 5); // creates a copy with the N suffix
                    self.subsect -> data[step + 1].s = realloc(self.subsect -> data[step + 1].s, strlen(self.subsect -> data[step + 1].s + 2));
                    memcpy(self.subsect -> data[step + 1].s + strlen(self.subsect -> data[step + 1].s), "N", 2); // replaces the copy in the working version with the temporary that is notted (logical)
                }


                if (self.subsect -> data[step].r -> length > 3) {
                    printf("Syntax error: too many operators at word %d\n", index);
                    return -1;
                }
                if (self.subsect -> data[step].r -> data[self.subsect -> data[step].r -> length - 1].c == '=') {
                    if (self.subsect -> data[step].r -> length == 1 || (self.subsect -> data[step].r -> data[self.subsect -> data[step].r -> length - 2].c != '=' && self.subsect -> data[step].r -> data[self.subsect -> data[step].r -> length - 2].c != '!')) {
                        list_append(self.toDoStack, (unitype) strdup(self.subsect -> data[step - 1].s), 's'); // makes a note to assign after expression is evaluated
                        list_append(self.toDoStack, (unitype) "TODO", 's'); // marker to fill when evaluating operator
                        // list_delete(self.subsect -> data[step].r, self.subsect -> data[step].r -> length - 1); // delete the = sign
                    }
                }
            }
        }
        step += 1;
    }
    // list_print(self.subsect);

    /* third pass: bodmas */
    /* by now the subsect list looks very similar to the one in the expression evaluator, it just has extra operations */
    /* I will not tolerate anything that is at this point less than 2 characters long, it will just immediately syntax error */
    list_clear(self.order);
    for (int i = 0; i < self.subsect -> length; i++) {
        list_append(self.order, (unitype) -1, 'i');
        if (self.subsect -> type[i] == 'r' && self.subsect -> data[i].r -> length > 2) {
            printf("Syntax error: operation too long at %d\n", index);
            return -1;
        }
    }
    int throw10;
    int throw11 = 3;
    while (throw11 < 14) {
        throw10 = 0;
        while (throw10 < self.subsect -> length) { // check for x class operators
            if (self.subsect -> type[throw10] == 'r' && associatedOperation(&self, self.subsect -> data[throw10].r, 1) == throw11) {
                self.order -> data[throw10].i = throw11;
            }
            throw10 += 1;
        }
        throw11++;
    }
    /* fourth pass: eval */
    int currentOrder = 3;
    while (currentOrder < 14) {
        if (list_count(self.order, (unitype) currentOrder, 'i')) {
            for (int i = 0; i < self.order -> length; i++) {
                if (self.order -> data[i].i == currentOrder) {
                    int opCode = associatedOperation(&self, self.subsect -> data[i].r, 2);
                    recordPaddedReg(&self, checkNamespace(&self, self.subsect -> data[i - 1].s), checkNamespace(&self, self.subsect -> data[i + 1].s), opCode);
                    if (opCode == 30) { // special case: fill TODO
                        int place = list_find(self.toDoStack, self.subsect -> data[i - 1], 's');
                        if (place == -1) {
                            printf("Syntax Error: Unsure assignment at word %d\n", index);
                            return -1;
                        }
                        self.toDoStack -> data[place + 1].s = strdup(self.subsect -> data[i + 1].s);
                    }
                    self.subsect -> data[i - 1].s = strdup(self.opResult); // replaces the copy in the working version with the result
                    
                    list_delete(self.subsect, i);
                    list_delete(self.subsect, i);
                    list_delete(self.order, i);
                    list_delete(self.order, i);
                    i--;
                    self.order -> data[i].i = -1;
                }
            }
        }
        currentOrder += 1;
    }
    /* wrap up*/
    // printf("quantities before: ");
    // list_print(self.quantities);
    if (self.quantities -> length > 2) {
        // printf("firstPointer: %d secondPointer: %d\n", firstPointer, secondPointer);
        for (int i = firstPointer + 1; i <= secondPointer; i++) {
            list_delete(self.quantities, firstPointer);
        }
        self.quantities -> data[firstPointer].s = strdup(self.subsect -> data[0].s);
        // printf("quantities after : ");
        // list_print(self.quantities);
    } else {
    if (self.quantities -> length == 2) {
        if (self.subsect -> length == 1) {
            list_clear(self.quantities);
            list_copy(self.subsect, self.quantities);
        } else {
            printf("Syntax error: something bad happened\n");
            return -1;
        }
    } else {
    if (self.quantities -> length == 1) {
        *selfp = self;
        return 0;
    }
    }
    }
    }
    printf("subsect: ");
    list_print(self.subsect);


    /* remember when you are doing the post-expression assigment to go BACK TO FRONT, so that if a = b = c, b will set itself to c FIRST then a will set itself to b */
    /* that's why it's called the stack approach actually that wasn't originally why but it is now */
    /* there might be issues with ++ because of this, but at this point I do not care and just want to see some results */

    printf("completed expression\n");
    *selfp = self;
    return 0;
}

int main(int argc, char *argv[]) { // walks through the source code (line by line for the most part)
    seagate self;
    self.pstr = list_init();
    self.strData = list_init();
    self.syntaxic = list_init(); // position (int) followed by syntax character
    self.keywords = list_init();
    self.registers = list_init();
    self.namespace = list_init(); // name (string), followed by type indicator (integer), followed by 0 - variable, 1 - function
    self.userNamespace = list_init();
    loadKeywords(&self);
    if (argc == 1) {
        printf("using file \"seagateFunction.c\"\n");
        self.filein = strdup("seagateFunction.c");
    } else {
        printf("using file \"%s\"\n", argv[1]);
        self.filein = strdup(argv[1]);
    }
    FILE *fi = fopen(self.filein, "r");
    if (fi == NULL) {
        printf("failed to open file \"%s\"\n", self.filein);
        return -1;
    }
    /* parsing the file:
    The file should begin with unsigned char seagate(unsigned char {string})
    We first want to eliminate extra whitespace */
    char string = 0;
    char lineComment = 0;
    char multilineComment = 0;
    char including = 0;
    char oldpchar = 'g';
    char directpchar = 'g';
    char pchar;
    int end = fscanf(fi, "%c", &pchar);
    // if (pchar > 64 && pchar < 91) { // lowercase only
    //     pchar += 32;
    // }
    while (end != EOF) {
        if (pchar == '#')
            including = 1;
        if (pchar == '"' && !lineComment && !multilineComment) {
            if (string) {
                string = 0;
            } else {
                string = 1;
                if (self.pstr -> length > 0 && self.pstr -> data[self.pstr -> length - 1].c == ' ') {
                    list_delete(self.pstr, self.pstr -> length - 1);
                }
            }
        }
        if (string) {
            list_append(self.pstr, (unitype) pchar, 'c');
            oldpchar = pchar;
        } else {
            if ((pchar == ' ' && (spaceDeprivedChar(oldpchar))) || (pchar == '\n' && !including) || pchar == '\r') { // CRLF really messing me up
                // nothing
            } else {
                if (pchar == '/' && oldpchar == '/') {
                    lineComment = 1;
                    if (self.pstr -> length > 0 && self.pstr -> data[self.pstr -> length - 1].c == '/') {
                        list_delete(self.pstr, self.pstr -> length - 1);
                    }
                }
                if (pchar == '*' && directpchar == '/') {
                    multilineComment = 1;
                    if (self.pstr -> length > 0 && self.pstr -> data[self.pstr -> length - 1].c == '/')
                        list_delete(self.pstr, self.pstr -> length - 1);
                }
                if (!lineComment && !multilineComment) {
                    list_append(self.pstr, (unitype) pchar, 'c');
                    if (self.pstr -> length > 1 && self.pstr -> data[self.pstr -> length - 2].c == ' ' && (spaceDeprivedChar(pchar))) {
                        list_delete(self.pstr, self.pstr -> length - 2);
                    }
                }
                if (pchar == '/' && directpchar == '*' && multilineComment) {
                    multilineComment = 0;
                }
                oldpchar = pchar;
            }
            if (pchar == '\n') {
                if (lineComment) {
                    lineComment = 0;
                    oldpchar = ' ';
                }
                if (including)
                    including = 0;
            }
        }
        directpchar = pchar;
        end = fscanf(fi, "%c", &pchar);
        // if (pchar > 64 && pchar < 91) { // lowercase only
        //     pchar += 32;
        // }
    }
    list_append(self.pstr, (unitype) '\0', 'c');
    if (self.pstr -> data[0].c == ' ' || self.pstr -> data[0].c == '\n') {
        list_delete(self.pstr, 0);
    }
    fclose(fi);

    // list_print(pstr);
    // printf("%s\n", list_toString(pstr));

    self.fileout = malloc(sizeof(char) * (strlen(self.filein) + 5));
    strcpy(self.fileout, self.filein);
    self.fileout[strlen(self.filein) - 2] = '_';
    self.fileout[strlen(self.filein) - 1] = 'o';
    self.fileout[strlen(self.filein)] = 'u';
    self.fileout[strlen(self.filein) + 1] = 't';
    self.fileout[strlen(self.filein) + 2] = '.';
    self.fileout[strlen(self.filein) + 3] = 'c';
    self.fileout[strlen(self.filein) + 4] = '\0';
    FILE *fo = fopen(self.fileout, "w+");
    char *tempStr = list_toString(self.pstr);
    fprintf(fo, "%s", tempStr);
    printf("formatted code in %s\n", self.fileout);
    free(tempStr);

    /* now that the code is in ultra-compressed form,
    we can start doing manual lookups for keywords and creating namespaces 
    
    Issues:
    strings can cause bad things to happen, the \ escape character is not implemented, and I don't care enough to do it since there should never be strings if you use seagate as intended
    
    */
    readString(&self);
    
    /* type indicators:
    bit mask
    or'd with 0x80000000 if it is signed
    or'd with 0x40000000 if it is a pointer, but like don't use pointers
    0x1 - char
    0x2 - short
    0x4 - int
    0x8 - long long
    0x10 - float
    0x20 - double
    */
    self.strPtr = 0;
    self.typeSize = 0;
    int currentType = checkType(&self);
    printf("record\n");
    recordRegFromNamespace(&self, currentType, 1, 1);
    printf("recorded\n");
    char synDiv = checkSyntax(&self, self.strPtr, 1);
    currentType = checkType(&self);
    if (self.namespace -> data[1].i == 0 || currentType == 0 || synDiv != '(' || 
    strcmp(self.namespace -> data[0].s, "seagate") != 0) {
        printf("Error: file must start with \"{type} seagate({type} {string1})\"\n");
        return -1;
    }
    recordRegFromNamespace(&self, currentType, 0, 1);

    while (checkSyntax(&self, self.strPtr, 1) == ',' && checkSyntax(&self, self.strPtr, 2) == 0) { // function has extra parameters
        currentType = checkType(&self);
        if (currentType == 0) {
            printf("invalid syntax at word %d\n", self.strPtr);
            return -1;
        }
        recordRegFromNamespace(&self, currentType, 0, 1);
    }
    if (checkSyntax(&self, self.strPtr, 1) != ')' || checkSyntax(&self, self.strPtr, 2) != '{' || checkSyntax(&self, self.strPtr, 3) != 0) { // function definition end
        printf("invalid syntax at word %d\n", self.strPtr);
        return -1;
    }

    /* now we're in the body of the function, lvalues could be:

    a label (for goto), we will be ignoring this for now
    goto, we will be ignoring this for now
    do, we will be ignoring this because I have never used it once in my life
    typedef, we will be ignoring this for now
    random {, which has already been detected as an error in the above if check. Even though it is valid C, just don't.


    return
    something in the namespaces
    a type (to define a new variable)
    if
    while
    for
    switch

    so only 7 possible things, that's totally doable

    I'm going to start with return
    */

    int test57 = '5' + '7'; // the fact that you can do shit like this is the reason i'm going to be diagnosed with Agoraphobia by the time this project is complete
    int testNot57 = !~!57; // you gotta be fucking kidding me dude
    int test102 = test57 --+-+-+6; // i'll never finish
    while (self.strPtr < self.strData -> length) {
        if (strcmp(self.strData -> data[self.strPtr].s, "return") == 0) { // return value
            printf("return at word %d\n", self.strPtr);
            self.strPtr += 1;
            /* return possibilities include:
            constants
            namespaces
            operations on either of those
            nothing
            */
            if (packageExpression(&self) == -1) { // we parse the expression after the return statement
                return -1;
            }
            /* pipe the output into the return register */
            recordPaddedReg(&self, 0, checkNamespace(&self, self.subsect -> data[0].s), 15);
            /* assign via the toDoStack */
            for (int i = self.toDoStack -> length - 2; i > -1; i -= 2) {
                printf("assigning %s (%d) to %s (%d) \n", self.toDoStack -> data[i].s, checkNamespace(&self, self.toDoStack -> data[i].s), self.toDoStack -> data[i + 1].s, checkNamespace(&self, self.toDoStack -> data[i + 1].s));
                recordPaddedReg(&self, checkNamespace(&self, self.toDoStack -> data[i].s), checkNamespace(&self, self.toDoStack -> data[i + 1].s), 0);
            }
            /* delete stack frame via deleteStack */
            for (int i = self.toDoStack -> length - 1; i > -1; i -= 2) {
                for (int j = 0; j < 4; j++) { // delete the full register (pop off stack)
                    list_delete(self.registers, self.deleteStack -> data[i].i);
                }
            }
            break;
        } else {
        int strPtrSaved = self.strPtr; // tempsave the strPtr so we can checkType
        printf("checking %s at word %d\n", self.strData -> data[self.strPtr].s, self.strPtr);
        currentType = checkType(&self);
        if (currentType != 0) { // new register
            recordRegFromNamespace(&self, currentType, 0, 0);
            printf("syntaxic: ");
            list_print(self.syntaxic);
            int updateReg = self.registers -> length - 4; // this register represents the new variable we've just created
            printf("(new) syntax at %d: %c\n", self.strPtr, checkSyntax(&self, self.strPtr, 1));
            if (checkSyntax(&self, self.strPtr, 1) == '=') {
                printf("assigning %s\n", self.namespace -> data[self.namespace -> length - 3].s);
                setSyntax(&self, self.strPtr, 1, ' ');
                if (packageExpression(&self) == -1) { // parse the expression
                    return -1;
                }
            }
            /* pipe the output into the updateReg */
            recordPaddedReg(&self, updateReg, checkNamespace(&self, self.subsect -> data[0].s), 15);
            /* assign via the toDoStack */
            for (int i = self.toDoStack -> length - 2; i > -1; i -= 2) {
                printf("assigning %s (%d) to %s (%d) \n", self.toDoStack -> data[i].s, checkNamespace(&self, self.toDoStack -> data[i].s), self.toDoStack -> data[i + 1].s, checkNamespace(&self, self.toDoStack -> data[i + 1].s));
                recordPaddedReg(&self, checkNamespace(&self, self.toDoStack -> data[i].s), checkNamespace(&self, self.toDoStack -> data[i + 1].s), 0);
            }
            /* delete stack frame via deleteStack */
            for (int i = self.toDoStack -> length - 1; i > -1; i -= 2) {
                for (int j = 0; j < 4; j++) { // delete the full register (pop off stack)
                    list_delete(self.registers, self.deleteStack -> data[i].i);
                }
            }
            
        } else {
        self.strPtr = strPtrSaved;
        int oldReg = checkNamespace(&self, self.strData -> data[self.strPtr].s);
        if (oldReg != -1) { // reassignment of old register
            printf("(redefine) syntax at %d: %c\n", self.strPtr + 1, checkSyntax(&self, self.strPtr + 1, 1));
            if (checkSyntax(&self, self.strPtr + 1, 1) == '=') { // case: assignment
                currentType = self.namespace -> data[list_find(self.namespace, self.strData -> data[self.strPtr], 's') + 1].i;
                char *redefine = malloc(strlen(self.strData -> data[self.strPtr].s) + 4);
                memcpy(redefine, self.strData -> data[self.strPtr].s, strlen(self.strData -> data[self.strPtr].s));
                memcpy(redefine + strlen(self.strData -> data[self.strPtr].s), "REF", 4);
                recordRegFromNamespace(&self, currentType, 0, 0);
                printf("syntaxic: ");
                list_print(self.syntaxic);
                int updateReg = self.registers -> length - 4; // this register represents the new variable we've just created
                printf("reassigning %s\n", self.strData -> data[self.strPtr - 1].s);
                setSyntax(&self, self.strPtr, 1, ' ');
                if (packageExpression(&self) == -1) { // parse the expression
                    return -1;
                }
                /* pipe the output into the updateReg */
                recordPaddedReg(&self, updateReg, checkNamespace(&self, self.subsect -> data[0].s), 15);
                /* reassign the oldReg to the new */
                recordPaddedReg(&self, oldReg, checkNamespace(&self, self.subsect -> data[0].s), 0);

                /* assign via the toDoStack */
                for (int i = self.toDoStack -> length - 2; i > -1; i -= 2) {
                    printf("assigning %s (%d) to %s (%d) \n", self.toDoStack -> data[i].s, checkNamespace(&self, self.toDoStack -> data[i].s), self.toDoStack -> data[i + 1].s, checkNamespace(&self, self.toDoStack -> data[i + 1].s));
                    recordPaddedReg(&self, checkNamespace(&self, self.toDoStack -> data[i].s), checkNamespace(&self, self.toDoStack -> data[i + 1].s), 0);
                }
                /* delete stack frame via deleteStack */
                for (int i = self.toDoStack -> length - 1; i > -1; i -= 2) {
                    for (int j = 0; j < 4; j++) { // delete the full register (pop off stack)
                        list_delete(self.registers, self.deleteStack -> data[i].i);
                    }
                }
                }
            if (checkSyntax(&self, self.strPtr + 1, 1) == '+' && checkSyntax(&self, self.strPtr + 1, 2) == '+' && checkSyntax(&self, self.strPtr + 1, 3) == ';') { // case: increment
                self.strPtr += 1; // skip inputX
                printf("incrementing %s\n", self.strData -> data[self.strPtr - 1].s);
                recordPaddedReg(&self, oldReg, 0, 11);
                /* reassign the oldReg to the new */
                printf("%d %d\n", oldReg, self.registers -> length - 4);
                recordPaddedReg(&self, oldReg, self.registers -> length - 4, 0);
                printf("%s\n", self.strData -> data[self.strPtr].s);
            }
            if (checkSyntax(&self, self.strPtr + 1, 1) == '-' && checkSyntax(&self, self.strPtr + 1, 2) == '-' && checkSyntax(&self, self.strPtr + 1, 3) == ';') { // case: increment
                self.strPtr += 1; // skip inputX
                printf("decrementing %s\n", self.strData -> data[self.strPtr - 1].s);
                recordPaddedReg(&self, oldReg, 0, 12);
                /* reassign the oldReg to the new */
                printf("%d %d\n", oldReg, self.registers -> length - 4);
                recordPaddedReg(&self, oldReg, self.registers -> length - 4, 0);
                printf("%s\n", self.strData -> data[self.strPtr].s);
            }
        } else {
            printf("Unidentified symbol %s at word %d\n", self.strData -> data[self.strPtr].s, self.strPtr);
        }
        }

        }
    }

    /* Eventually we also have to make registers for all of the namespaces:
    I think they should go right to left, layed out in a line or maybe an array going up or downward */

    list_print(self.namespace);


    fclose(fo);
    self.gateout = malloc(sizeof(char) * (strlen(self.filein) + 9));
    strcpy(self.gateout, self.filein);
    self.gateout[strlen(self.filein) - 2] = '_';
    self.gateout[strlen(self.filein) - 1] = 'g';
    self.gateout[strlen(self.filein)] = 'a';
    self.gateout[strlen(self.filein) + 1] = 't';
    self.gateout[strlen(self.filein) + 2] = 'e';
    self.gateout[strlen(self.filein) + 3] = 's';
    self.gateout[strlen(self.filein) + 4] = '.';
    self.gateout[strlen(self.filein) + 5] = 't';
    self.gateout[strlen(self.filein) + 6] = 'x';
    self.gateout[strlen(self.filein) + 7] = 't';
    self.gateout[strlen(self.filein) + 8] = '\0';
    FILE *go = fopen(self.gateout, "w+");
    exportGates(&self, self.gateout);
    printf("gate output in %s\n", self.gateout);
    fclose(go);

    /* free stuff */
    list_free(self.pstr);
    list_free(self.strData);
    list_free(self.syntaxic);
    list_free(self.keywords);
    list_free(self.registers);
    list_free(self.namespace);
    list_free(self.components);
    list_free(self.compSlots);
    list_free(self.inpComp);
    list_free(self.io);
    list_free(self.positions);
    list_free(self.wiring);
    list_free(self.quantities);
    list_free(self.subsect);
    list_free(self.order);
    list_free(self.operatorPrecedence);
    free(self.filein);
    free(self.fileout);
    free(self.gateout);
    printf("completed\n");
}