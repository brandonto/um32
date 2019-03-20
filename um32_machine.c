//******************************************************************************
//
// Copyright (c) 2019, Brandon To
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//     * Redistributions of source code must retain the above copyright
//       notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above copyright
//       notice, this list of conditions and the following disclaimer in the
//       documentation and/or other materials provided with the distribution.
//     * Neither the name of the author nor the names of its contributors may be
//       used to endorse or promote products derived from this software without
//       specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
// DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
//******************************************************************************
#include "um32_machine.h"

#include "um32_memory.h"
#include <errno.h>
#include <limits.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>

//#define UM32_MACHINE_DEBUG_ENABLED

#ifdef UM32_MACHINE_DEBUG_ENABLED
#include <stdio.h>
void
um32_machine_logState(um32_machine_pt machine_p, um32_platter_t platter)
{
    char buf[128];
    um32_platter_toString(platter, buf);

    uint32_t valA = um32_platter_toUInt32(machine_p->reg_a[platter.regA]);
    uint32_t valB = um32_platter_toUInt32(machine_p->reg_a[platter.regB]);
    uint32_t valC = um32_platter_toUInt32(machine_p->reg_a[platter.regC]);
    printf("%s { valA = %u, valB = %u, valC = %u }\n", buf, valA, valB, valC);
}
#endif

um32_machine_pt
um32_machine_create(void)
{
    // Allocates memory for um32
    //
    um32_machine_pt machine_p =
        (um32_machine_pt)um32_memory_malloc(sizeof(um32_machine_t));
    if (machine_p == NULL) { return NULL; }

    // Initialize all registers to 0
    //
    memset(machine_p, 0, sizeof(um32_machine_t));

    return machine_p;
}

void
um32_machine_free(um32_machine_pt machine_p)
{
    if (machine_p == NULL) { return; }

    // Free memory for um32
    //
    um32_memory_free(machine_p);
}

//  The machine shall be initialized with a '0' array whose contents
//  shall be read from a "program" scroll. All registers shall be
//  initialized with platters of value '0'. The execution finger shall
//  point to the first platter of the '0' array, which has offset zero.
//
bool
um32_machine_init(um32_machine_pt machine_p, FILE* file_p)
{
    if ((machine_p == NULL) || (file_p == NULL)) { return false; }

    // Get the size of the program in bytes
    //
    long programSizeBytes;
    if (fseek(file_p, 0L, SEEK_END) != 0) { return false; }
    if ((programSizeBytes = ftell(file_p)) == -1) { return false; }
    if (fseek(file_p, 0L, SEEK_SET) != 0) { return false; }

    // Allocate enough memory to store entire program
    //
    machine_p->zeroArray_p =
        (um32_platter_pt)um32_memory_malloc((size_t)programSizeBytes);
    if (machine_p->zeroArray_p == NULL) { return false; }

    // Copies the entire program into the 0 array
    //
    char* mem_p = (char*)(machine_p->zeroArray_p);
    for (int i=0; i<programSizeBytes; i++)
    {
        mem_p[i] = fgetc(file_p);
    }

    // Save a pointer to the end of the 0 array
    //
    machine_p->zeroArrayEnd_p = (um32_platter_pt)(mem_p + programSizeBytes);

    // Point execution finger to start of 0 array
    //
    machine_p->executionFinger_p = machine_p->zeroArray_p;

    // Endian swap all platters
    //
    um32_platter_pt curPlatter_p = machine_p->zeroArray_p;
    while (curPlatter_p < machine_p->zeroArrayEnd_p)
    {
        *curPlatter_p = um32_platter_toHostByteOrder(*curPlatter_p);
        curPlatter_p++;
    }

    return true;
}

//  Standard Operators.
//  -------------------
//

//  Operator #0. Conditional Move.
//
//                  The register A receives the value in register B,
//                  unless the register C contains 0.
//
inline static void
um32_machine_handleOperatorConditionalMove(um32_machine_pt machine_p,
                                           um32_platter_t platter)
{
    if (um32_platter_toUInt32(machine_p->reg_a[platter.regC]) != 0)
    {
        machine_p->reg_a[platter.regA] = machine_p->reg_a[platter.regB];
    }
}

//           #1. Array Index.
//
//                  The register A receives the value stored at offset
//                  in register C in the array identified by B.
//
inline static void
um32_machine_handleOperatorArrayIndex(um32_machine_pt machine_p,
                                      um32_platter_t platter)
{
    uint32_t valB = um32_platter_toUInt32(machine_p->reg_a[platter.regB]);
    uint32_t valC = um32_platter_toUInt32(machine_p->reg_a[platter.regC]);
    um32_platter_pt array_p = valB ? (um32_platter_pt)(uintptr_t)valB
                                   : machine_p->zeroArray_p;

    machine_p->reg_a[platter.regA] = *(array_p + valC);
}

//           #2. Array Amendment.
//
//                  The array identified by A is amended at the offset
//                  in register B to store the value in register C.
//
inline static void
um32_machine_handleOperatorArrayAmendment(um32_machine_pt machine_p,
                                          um32_platter_t platter)
{
    uint32_t valA = um32_platter_toUInt32(machine_p->reg_a[platter.regA]);
    uint32_t valB = um32_platter_toUInt32(machine_p->reg_a[platter.regB]);
    um32_platter_pt array_p = valA ? (um32_platter_pt)(uintptr_t)valA
                                   : machine_p->zeroArray_p;

    *(array_p + valB) = machine_p->reg_a[platter.regC];
}

//           #3. Addition.
//
//                  The register A receives the value in register B plus 
//                  the value in register C, modulo 2^32.
//
inline static void
um32_machine_handleOperatorAddition(um32_machine_pt machine_p,
                                    um32_platter_t platter)
{
    uint32_t valB = um32_platter_toUInt32(machine_p->reg_a[platter.regB]);
    uint32_t valC = um32_platter_toUInt32(machine_p->reg_a[platter.regC]);

    machine_p->reg_a[platter.regA] = um32_platter_fromUInt32(valB + valC);
}

//           #4. Multiplication.
//
//                  The register A receives the value in register B times
//                  the value in register C, modulo 2^32.
//
inline static void
um32_machine_handleOperatorMultiplication(um32_machine_pt machine_p,
                                          um32_platter_t platter)
{
    uint32_t valB = um32_platter_toUInt32(machine_p->reg_a[platter.regB]);
    uint32_t valC = um32_platter_toUInt32(machine_p->reg_a[platter.regC]);

    machine_p->reg_a[platter.regA] = um32_platter_fromUInt32(valB * valC);
}

//           #5. Division.
//
//                  The register A receives the value in register B
//                  divided by the value in register C, if any, where
//                  each quantity is treated treated as an unsigned 32
//                  bit number.
//
inline static void
um32_machine_handleOperatorDivision(um32_machine_pt machine_p,
                                    um32_platter_t platter)
{
    uint32_t valB = um32_platter_toUInt32(machine_p->reg_a[platter.regB]);
    uint32_t valC = um32_platter_toUInt32(machine_p->reg_a[platter.regC]);

    machine_p->reg_a[platter.regA] = um32_platter_fromUInt32(valB / valC);
}

//           #6. Not-And.
//
//                  Each bit in the register A receives the 1 bit if
//                  either register B or register C has a 0 bit in that
//                  position.  Otherwise the bit in register A receives
//                  the 0 bit.
//
inline static void
um32_machine_handleOperatorNotAnd(um32_machine_pt machine_p,
                                  um32_platter_t platter)
{
    uint32_t valB = um32_platter_toUInt32(machine_p->reg_a[platter.regB]);
    uint32_t valC = um32_platter_toUInt32(machine_p->reg_a[platter.regC]);

    machine_p->reg_a[platter.regA] = um32_platter_fromUInt32(~(valB & valC));
}

//  Other Operators.
//  ----------------
//
//  The following instructions ignore some or all of the A, B and C
//  registers.
//

//           #7. Halt.
//
//                  The universal machine stops computation.
//
inline static void
um32_machine_handleOperatorHalt(void)
{
    // The function stub is here to make explicit that the halt operator was
    // considered. There is no implementation required because the halt operator
    // just stops the virtual machine.
    //
}

//           #8. Allocation.
//
//                  A new array is created with a capacity of platters
//                  commensurate to the value in the register C. This
//                  new array is initialized entirely with platters
//                  holding the value 0. A bit pattern not consisting of
//                  exclusively the 0 bit, and that identifies no other
//                  active allocated array, is placed in the B register.
//
inline static void
um32_machine_handleOperatorAllocation(um32_machine_pt machine_p,
                                      um32_platter_t platter)
{
    uint32_t valB = um32_platter_toUInt32(machine_p->reg_a[platter.regB]);
    um32_platter_pt array_p = (um32_platter_pt)(uintptr_t)valB;
    uint32_t valC = um32_platter_toUInt32(machine_p->reg_a[platter.regC]);

    size_t bytesToAlloc = (size_t)(valC * sizeof(um32_platter_t));
    array_p = (um32_platter_pt)um32_memory_malloc(bytesToAlloc);
    if (array_p == NULL)
    {
        printf("Unable to allocate array of platters.\n");
        return;
    }
    memset(array_p, 0, bytesToAlloc);

    machine_p->reg_a[platter.regB] = um32_platter_fromUInt32((uintptr_t)(void*)array_p);
}

//           #9. Abandonment.
//
//                  The array identified by the register C is abandoned.
//                  Future allocations may then reuse that identifier.
//
inline static void
um32_machine_handleOperatorAbandonment(um32_machine_pt machine_p,
                                       um32_platter_t platter)
{
    uint32_t valC = um32_platter_toUInt32(machine_p->reg_a[platter.regC]);
    um32_platter_pt array_p = (um32_platter_pt)(uintptr_t)valC;

    um32_memory_free(array_p);
}

//          #10. Output.
//
//                  The value in the register C is displayed on the console
//                  immediately. Only values between and including 0 and 255
//                  are allowed.
//
inline static void
um32_machine_handleOperatorOutput(um32_machine_pt machine_p,
                                  um32_platter_t platter)
{
    uint32_t valC = um32_platter_toUInt32(machine_p->reg_a[platter.regC]);
    if (valC >= 255)
    {
        printf("Only values between and including 0 and 255 are allowed.\n");
        return;
    }

    if (write(1, &(machine_p->reg_a[platter.regC]), 1) == -1)
    {
        printf("Error writing to outpu.\n");
    }
}

//          #11. Input.
//
//                  The universal machine waits for input on the console.
//                  When input arrives, the register C is loaded with the
//                  input, which must be between and including 0 and 255.
//                  If the end of input has been signaled, then the 
//                  register C is endowed with a uniform value pattern
//                  where every place is pregnant with the 1 bit.
//
inline static void
um32_machine_handleOperatorInput(um32_machine_pt machine_p,
                                 um32_platter_t platter)
{
    int input = getchar();
    machine_p->reg_a[platter.regC] =
        um32_platter_fromUInt32((input == EOF) ? 0xFFFFFFFF : (uint32_t)input);
}

//          #12. Load Program.
//
//                  The array identified by the B register is duplicated
//                  and the duplicate shall replace the '0' array,
//                  regardless of size. The execution finger is placed
//                  to indicate the platter of this array that is
//                  described by the offset given in C, where the value
//                  0 denotes the first platter, 1 the second, et
//                  cetera.
//
//                  The '0' array shall be the most sublime choice for
//                  loading, and shall be handled with the utmost
//                  velocity.
//
inline static void
um32_machine_handleOperatorLoadProgram(um32_machine_pt machine_p,
                                       um32_platter_t platter)
{
    // Update execution finger. This needs to be done regardless of whether or
    // not the source array exists so this operator can act as a jump.
    //
    uint32_t valC = um32_platter_toUInt32(machine_p->reg_a[platter.regC]);
    machine_p->executionFinger_p = machine_p->zeroArray_p + valC;

    // Get source array and size of source array
    //
    uint32_t valB = um32_platter_toUInt32(machine_p->reg_a[platter.regB]);
    um32_platter_pt srcArray_p = (um32_platter_pt)(uintptr_t)valB;
    if (srcArray_p == NULL)
    {
        //printf("Loading from an unallocated array.\n");
        return;
    }

    // Reallocate enough memory to store new program
    //
    size_t srcArraySize = um32_memory_malloc_usable_size(srcArray_p);
    machine_p->zeroArray_p =
        (um32_platter_pt)um32_memory_realloc(machine_p->zeroArray_p,
                                             srcArraySize);
    if (machine_p->zeroArray_p == NULL)
    {
        printf("Unable to allocate memory for new program.\n");
        return;
    }

    // Copies new program into 0 array
    //
    memcpy(machine_p->zeroArray_p, srcArray_p, srcArraySize);

    // Update pointer to end of 0 array
    //
    machine_p->zeroArrayEnd_p = machine_p->zeroArray_p + srcArraySize;
}

//  Special Operators.
//  ------------------
//

//          #13. Orthography.
//
//                  The value indicated is loaded into the register A
//                  forthwith.
//
inline static void
um32_machine_handleOperatorOrthography(um32_machine_pt machine_p,
                                       um32_platter_special_t platter)
{
    machine_p->reg_a[platter.regA] = um32_platter_fromUInt32(platter.value);
}

//  Once initialized, the machine begins its Spin Cycle. In each cycle
//  of the Universal Machine, an Operator shall be retrieved from the
//  platter that is indicated by the execution finger. Before this operator
//  is discharged, the execution finger shall be advanced to the next
//  platter, if any.
//
void
um32_machine_run(um32_machine_pt machine_p)
{
    while (machine_p->executionFinger_p < machine_p->zeroArrayEnd_p)
    {
        um32_platter_t curPlatter = *(machine_p->executionFinger_p++);

#ifdef UM32_MACHINE_DEBUG_ENABLED
        um32_machine_logState(machine_p, curPlatter);
#endif

        switch(curPlatter.operatorNum)
        {
        case UM32_OPERATOR_CONDITIONAL_MOVE:
            um32_machine_handleOperatorConditionalMove(machine_p, curPlatter);
            break;
        case UM32_OPERATOR_ARRAY_INDEX:
            um32_machine_handleOperatorArrayIndex(machine_p, curPlatter);
            break;
        case UM32_OPERATOR_ARRAY_AMENDMENT:
            um32_machine_handleOperatorArrayAmendment(machine_p, curPlatter);
            break;
        case UM32_OPERATOR_ADDITION:
            um32_machine_handleOperatorAddition(machine_p, curPlatter);
            break;
        case UM32_OPERATOR_MULTIPLICATION:
            um32_machine_handleOperatorMultiplication(machine_p, curPlatter);
            break;
        case UM32_OPERATOR_DIVISION:
            um32_machine_handleOperatorDivision(machine_p, curPlatter);
            break;
        case UM32_OPERATOR_NOT_AND:
            um32_machine_handleOperatorNotAnd(machine_p, curPlatter);
            break;
        case UM32_OPERATOR_HALT:
            um32_machine_handleOperatorHalt();
            return;
        case UM32_OPERATOR_ALLOCATION:
            um32_machine_handleOperatorAllocation(machine_p, curPlatter);
            break;
        case UM32_OPERATOR_ABANDONMENT:
            um32_machine_handleOperatorAbandonment(machine_p, curPlatter);
            break;
        case UM32_OPERATOR_OUTPUT:
            um32_machine_handleOperatorOutput(machine_p, curPlatter);
            break;
        case UM32_OPERATOR_INPUT:
            um32_machine_handleOperatorInput(machine_p, curPlatter);
            break;
        case UM32_OPERATOR_LOAD_PROGRAM:
            um32_machine_handleOperatorLoadProgram(machine_p, curPlatter);
            break;
        case UM32_OPERATOR_ORTHOGRAPHY:
            um32_machine_handleOperatorOrthography(machine_p,
                    um32_platter_special_fromPlatter(curPlatter));
            break;
        }
    }
}
