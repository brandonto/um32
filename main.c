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
#include <stdio.h>
#include <string.h>

void
printUsage(void)
{
    printf("Usage: um32 [OPTIONS] FILE\n");
    printf("Options:\n");
    printf("  -h, --help          display this information\n");
}

int
main(int argc, char* argv[])
{
    // Parse command line arguments
    //
    if ((argc != 2) && (argc != 3))
    {
        printf("Invalid number of arguments.\n");
        printUsage();
        return -1;
    }

    if ((strcmp(argv[1], "-h") == 0) || (strcmp(argv[1], "--help") == 0))
    {
        printUsage();
        return -1;
    }
    else if (argc == 3)
    {
        printf("Invalid number of arguments.\n");
        printUsage();
        return -1;
    }

    // Open file stream of program
    //
    char* programName = argv[1];
    FILE* file_p = fopen(programName, "r");
    if (file_p  == NULL)
    {
        printf("Unable to open file.\n");
        return -1;
    }

    // Run program using virtual machine
    //
    um32_machine_pt machine_p = um32_machine_create();
    if (machine_p == NULL)
    {
        printf("Unable to create UM32 virtual machine.\n");
        fclose(file_p);
        return -1;
    }

    if (!um32_machine_init(machine_p, file_p))
    {
        printf("Unable to initialize UM32 virtual machine.\n");
        um32_machine_free(machine_p);
        fclose(file_p);
        return -1;
    }

    um32_machine_run(machine_p);

    // Free any allocated resources before exiting
    //
    um32_machine_free(machine_p);
    fclose(file_p);

    return 0;
}
