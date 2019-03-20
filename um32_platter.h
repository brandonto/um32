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
#ifndef UM32_PLATTER_H
#define UM32_PLATTER_H

#include <stdbool.h>
#include <stdint.h>

// Enumeration for all operators support by the virtual machine
//
//  Operators.
//  ----------
//
//  The Universal Machine may produce 14 Operators. The number of the
//  operator is described by the most meaningful four bits of the
//  instruction platter.
//
//              .--------------------------------.
//              |VUTSRQPONMLKJIHGFEDCBA9876543210|
//              `--------------------------------'
//               ^^^^
//               |
//               operator number
//
//              Figure 1. Operator Description
//
typedef enum
{
    UM32_OPERATOR_CONDITIONAL_MOVE  = 0,
    UM32_OPERATOR_ARRAY_INDEX       = 1,
    UM32_OPERATOR_ARRAY_AMENDMENT   = 2,
    UM32_OPERATOR_ADDITION          = 3,
    UM32_OPERATOR_MULTIPLICATION    = 4,
    UM32_OPERATOR_DIVISION          = 5,
    UM32_OPERATOR_NOT_AND           = 6,
    UM32_OPERATOR_HALT              = 7,
    UM32_OPERATOR_ALLOCATION        = 8,
    UM32_OPERATOR_ABANDONMENT       = 9,
    UM32_OPERATOR_OUTPUT            = 10,
    UM32_OPERATOR_INPUT             = 11,
    UM32_OPERATOR_LOAD_PROGRAM      = 12,
    UM32_OPERATOR_ORTHOGRAPHY       = 13,
    UM32_OPERATOR_MAX               = 14,
} um32_operator_operatorNum_t;

// Structure representing a platter. The specifications follows:
//
//  Standard Operators.
//  -------------------
//
//  Each Standard Operator performs an errand using three registers,
//  called A, B, and C. Each register is described by a three bit
//  segment of the instruction platter. The register C is described by
//  the three least meaningful bits, the register B by the three next
//  more meaningful than those, and the register A by the three next
//  more meaningful than those.
//
//                                      A     C
//                                      |     |
//                                      vvv   vvv
//              .--------------------------------.
//              |VUTSRQPONMLKJIHGFEDCBA9876543210|
//              `--------------------------------'
//               ^^^^                      ^^^
//               |                         |
//               operator number           B
//
//              Figure 2. Standard Operators
//
typedef struct
{
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
    uint32_t regC:3;
    uint32_t regB:3;
    uint32_t regA:3;
    uint32_t unused:19;
    uint32_t operatorNum:4;
#else /* __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__ */
    uint32_t operatorNum:4;
    uint32_t unused:19;
    uint32_t regA:3;
    uint32_t regB:3;
    uint32_t regC:3;
#endif
} um32_platter_t;
typedef um32_platter_t* um32_platter_pt;

// Operator ORTHOGRAPHY requires a special interpretation of the platter. The
// um32_platter_t structure can be cast to this structure for that purpose. The
// specifications follows:
//
//  Special Operators.
//  ------------------
//
//  One special operator does not describe registers in the same way.
//  Instead the three bits immediately less significant than the four
//  instruction indicator bits describe a single register A. The
//  remainder twenty five bits indicate a value, which is loaded
//  forthwith into the register A.
//
//                   A
//                   |
//                   vvv
//              .--------------------------------.
//              |VUTSRQPONMLKJIHGFEDCBA9876543210|
//              `--------------------------------'
//               ^^^^   ^^^^^^^^^^^^^^^^^^^^^^^^^
//               |      |
//               |      value
//               |
//               operator number
//
//               Figure 3. Special Operators
//
typedef struct
{
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
    uint32_t value:25;
    uint32_t regA:3;
    uint32_t operatorNum:4;
#else /* __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__ */
    uint32_t operatorNum:4;
    uint32_t regA:3;
    uint32_t value:25;
#endif
} um32_platter_special_t;
typedef um32_platter_special_t* um32_platter_special_pt;

um32_platter_t um32_platter_toHostByteOrder(um32_platter_t platter);
uint32_t um32_platter_toUInt32(um32_platter_t platter);
um32_platter_t um32_platter_fromUInt32(uint32_t val);
void um32_platter_toString(um32_platter_t platter, char* buf_p);
um32_platter_special_t um32_platter_special_fromPlatter(um32_platter_t platter);

#endif /* UM32_PLATTER_H */
