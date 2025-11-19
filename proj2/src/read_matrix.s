.globl read_matrix

.text
# ==============================================================================
# FUNCTION: Allocates memory and reads in a binary file as a matrix of integers
#
# FILE FORMAT:
#   The first 8 bytes are two 4 byte ints representing the # of rows and columns
#   in the matrix. Every 4 bytes afterwards is an element of the matrix in
#   row-major order.
# Arguments:
#   a0 (char*) is the pointer to string representing the filename
#   a1 (int*)  is a pointer to an integer, we will set it to the number of rows
#   a2 (int*)  is a pointer to an integer, we will set it to the number of columns
# Returns:
#   a0 (int*)  is the pointer to the matrix in memory
# Exceptions:
#   - If malloc returns an error,
#     this function terminates the program with error code 26
#   - If you receive an fopen error or eof,
#     this function terminates the program with error code 27
#   - If you receive an fclose error or eof,
#     this function terminates the program with error code 28
#   - If you receive an fread error or eof,
#     this function terminates the program with error code 29
# ==============================================================================
read_matrix:

    # Prologue
    addi sp, sp, -24
    sw ra, 0(sp)
    sw a1, 4(sp)
    sw a2, 8(sp)
    sw s0, 12(sp)  # stores file descriptor returned by fopen
    sw s1, 16(sp)  # stores the number of bytes to read from the file 
    sw s2, 20(sp)  # stores the pointer to the matrix in memory

    li a1, 0
    jal ra, fopen
    li t0, -1
    beq a0, t0, exception2
    mv s0, a0

    lw a1, 4(sp)
    li a2, 4
    mv s1, a2
    jal ra, fread
    bne a0, s1, exception4

    mv a0, s0
    lw a1, 8(sp)
    li a2, 4
    jal ra, fread
    bne a0, s1, exception4

    lw a1, 4(sp)
    lw a2, 8(sp)
    lw t0, 0(a1)
    lw t1, 0(a2)
    mul a0, t0, t1
    slli a0, a0, 2
    mv s1, a0

    jal ra, malloc
    beq a0, x0, exception1
    mv s2, a0

    mv a0, s0
    mv a1, s2
    mv a2, s1
    jal ra, fread
    bne a0, s1, exception4

    mv a0, s0
    jal ra, fclose
    bne a0, x0, exception3

    mv t0, s2  # move to t0 because we need to recover s2

    # Epilogue
    lw ra, 0(sp)
    lw s0, 12(sp)
    lw s1, 16(sp)
    lw s2, 20(sp)
    addi sp, sp, 24

    mv a0, t0  # return the pointer to the matrix in memory
    jr ra

exception1:
    li a0, 26
    j exit

exception2:
    li a0, 27
    j exit

exception3:
    li a0, 28
    j exit

exception4:
    li a0, 29
    j exit