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
#include "um32_platter.h"

#include <arpa/inet.h>
#include <stdio.h>

inline um32_platter_t
um32_platter_toHostByteOrder(um32_platter_t platter)
{
    return um32_platter_fromUInt32(ntohl(um32_platter_toUInt32(platter)));
}

inline um32_platter_t
um32_platter_toNetworkByteOrder(um32_platter_t platter)
{
    return um32_platter_fromUInt32(htonl(um32_platter_toUInt32(platter)));
}

// Converts um32_platter_t to uint32_t, taking endianness in consideration
//
inline uint32_t
um32_platter_toUInt32(um32_platter_t platter)
{
    return *(uint32_t*)(void*)&platter;
}

// Converts uint32_t to um32_platter_t, taking endianness in consideration
//
inline um32_platter_t
um32_platter_fromUInt32(uint32_t val)
{
    uint32_t valTemp = val;
    return *(um32_platter_t*)&valTemp;
}

void
um32_platter_toString(um32_platter_t platter, char* buf_p)
{
    static char* const um32_operator_stringTable_a[UM32_OPERATOR_MAX] =
    {
        "CONDITIONAL_MOVE",
        "ARRAY_INDEX",
        "ARRAY_AMENDMENT",
        "ADDITION",
        "MULTIPLICATION",
        "DIVISION",
        "NOT_AND",
        "HALT",
        "ALLOCATION",
        "ABANDONMENT",
        "OUTPUT",
        "INPUT",
        "LOAD_PROGRAM",
        "ORTHOGRAPHY",
    };

    sprintf(buf_p, "{ operator = %s, regA = %u, regB = %u, regC = %u }",
            um32_operator_stringTable_a[platter.operatorNum],
            platter.regA, platter.regB, platter.regC);
}

// Convert um32_platter_t to um32_platter_special_t
//
inline um32_platter_special_t
um32_platter_special_fromPlatter(um32_platter_t platter)
{
    return *(um32_platter_special_t*)(void*)&platter;
}
