#ifndef CHJANT_TYPE
#define CHJANT_TYPE

#include "include/list.h"

/* chjant type */
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
    list_t *namespace; // spec: 3 items per namespace, name, type, 0 for variable, 1 for function (i should change the name so C++ doesn't get confused)
    list_t *userNamespace; // this one only contains string names, is used to keep track of what registers can be incremented, assigned, etc, you cannot assign a constant or intermediate register
    list_t *allocatedArena; // so basically this is tied to the registers and specifies the origin area for components to be drawn (tied to that register)
    // because of bad foresight, there is still the register's "size of references" piece of data which really should be in this list but we live with our decisions
    // I will allocate 5 items of data for each register in the allocatedArena though, in case I want to migrate that and other data here
    // actually I don't feel like doing this, we're instead going to use band-aid solution #14 and just increment all type1 registers whenever we need space

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
    list_t *deleteStack;
    char *opResult;
} chjant;

int checkNamespace(chjant *selfp, char *findName); // needed for chjant.c and recordPaddedReg.c, defined in chjant.c

#endif