#ifndef SEAGATE_TYPE
#define SEAGATE_TYPE

#include "include/list.h"

/* seagate type */
typedef struct {
    char *filein; // filename
    char *fileout; // filename_out (prettier in C)
    char *gateout; // filename_gates
    int charPtr;
    int strPtr;
    int typeSize;
    int constantType;
    unitype constant;
    list_t *pstr; // characters
    list_t *strData; // strings
    list_t *syntaxic; // list of spaceDeprivedChars and their positions
    list_t *keywords; // all keywords in C
    list_t *registers; // register spec: 4 items per register, it goes name (string), size in bits (int), size of references (how far above this the references extend) (int), list of component handles (list_t *)
    list_t *namespace; // spec: 3 items per namespace, name, type, 0 for variable, 1 for function

    /* logicgates data emulator */
    list_t *components; // list of components (1 item for each component, a string with "POWER", "AND", etc), a component's "ID" or "Handle" is that component's index in this list
    list_t *compSlots; // a lookup table for which components have two inputs vs one
    list_t *inpComp; // list of component ID inputs, 3 items per component, format: number of possible connections (either 1 or 2), connection 1 (ID, 0 if none), connection 2 (ID, 0 if none)
    list_t *io; // list of binary inputs and outputs of a component (3 items for each component, 2 inputs followed by the output of the component (either a 0 or 1))
    list_t *positions; // list of component positions (3 items for each component, doubles specifying x, y, and angle)
    list_t *wiring; // list of component connections (3 items per connection, it goes sender (ID), reciever (ID), powered (0 or 1))
    double compDistX; // how far apart components are (X)
    double compDistY; // Y
    double gateLeftX;
    double gateRightX;

    /* expression evaluator (modified) */
    list_t *quantities;
    list_t *subsect;
    list_t *order;
    list_t *operatorPrecedence; // associative array
    list_t *toDoStack;
    char *opResult;
} seagate;

int checkNamespace(seagate *selfp, char *findName); // needed for seagate.c and recordPaddedReg.c, defined in seagate.c

#endif