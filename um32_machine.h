#ifndef UM32_MACHINE_H
#define UM32_MACHINE_H

#include "um32_platter.h"
#include <stdbool.h>
#include <stdio.h>

#define UM32_NUM_GENERAL_PURPOSE_REGISTERS 8

// Structure representing a UM32 virtual machine.
//
//  Physical Specifications.
//  ------------------------
//
//  The machine shall consist of the following components:
//
//    * An infinite supply of sandstone platters, with room on each
//      for thirty-two small marks, which we call "bits."
//
//                           least meaningful bit
//                                              |
//                                              v
//              .--------------------------------.
//              |VUTSRQPONMLKJIHGFEDCBA9876543210|
//              `--------------------------------'
//               ^
//               |
//               most meaningful bit
//
//              Figure 0. Platters
//      
//      Each bit may be the 0 bit or the 1 bit. Using the system of
//      "unsigned 32-bit numbers" (see patent #4,294,967,295) the
//      markings on these platters may also denote numbers.
//
//    * Eight distinct general-purpose registers, capable of holding one
//      platter each.
//
//    * A collection of arrays of platters, each referenced by a distinct
//      32-bit identifier. One distinguished array is referenced by 0
//      and stores the "program." This array will be referred to as the
//      '0' array.
//
//    * A 1x1 character resolution console capable of displaying glyphs
//      from the "ASCII character set" (see patent #127) and performing
//      input and output of "unsigned 8-bit characters" (see patent
//      #255).
//
typedef struct
{
    um32_platter_t   reg_a[UM32_NUM_GENERAL_PURPOSE_REGISTERS];
    um32_platter_pt  zeroArray_p;
    um32_platter_pt  zeroArrayEnd_p;
    um32_platter_pt  executionFinger_p;
} um32_machine_t;
typedef um32_machine_t* um32_machine_pt;

um32_machine_pt um32_machine_create(void);
void um32_machine_free(um32_machine_pt machine_p);
bool um32_machine_init(um32_machine_pt machine_p, FILE* file_p);
void um32_machine_run(um32_machine_pt machine_p);

#endif /* UM32_MACHINE_H */
