.globl write_matrix

.text
# ==============================================================================
# FUNCTION: Writes a matrix of integers into a binary file
# FILE FORMAT:
#   The first 8 bytes of the file will be two 4 byte ints representing the
#   numbers of rows and columns respectively. Every 4 bytes thereafter is an
#   element of the matrix in row-major order.
# Arguments:
#   a0 (char*) is the pointer to string representing the filename
#   a1 (int*)  is the pointer to the start of the matrix in memory
#   a2 (int)   is the number of rows in the matrix
#   a3 (int)   is the number of columns in the matrix
# Returns:
#   None
# Exceptions:
#   - If you receive an fopen error or eof,
#     this function terminates the program with error code 27
#   - If you receive an fclose error or eof,
#     this function terminates the program with error code 28
#   - If you receive an fwrite error or eof,
#     this function terminates the program with error code 30
# ==============================================================================
write_matrix:

    # Prologue
    addi sp, sp, -24
    sw ra, 0(sp)
    sw a1, 4(sp)
    sw a2, 8(sp)
    sw a3, 12(sp)
    sw s0, 16(sp)  # stores file descriptor returned by fopen
    sw s1, 20(sp)  # stores the number of elements to write to the file 

    # open file
    li a1, 1
    jal ra, fopen
    li t0, -1
    beq a0, t0, exception1
    mv s0, a0

    # write number of rows
    addi a1, sp, 8
    li a2, 1
    mv s1, a2
    li a3, 4
    jal ra, fwrite
    bne a0, s1, exception2

    # write number of columns
    mv a0, s0
    addi a1, sp, 12
    li a2, 1
    mv s1, a2
    li a3, 4
    jal ra, fwrite
    bne a0, s1, exception2

    # write matrix
    mv a0, s0
    lw a1, 4(sp)
    lw t1, 8(sp)
    lw t2, 12(sp)
    mul a2, t1, t2
    mv s1, a2
    li a3, 4
    jal ra, fwrite
    bne a0, s1, exception2

    # close file
    mv a0, s0
    jal ra, fclose
    bne a0, x0, exception3

    # Epilogue
    lw ra, 0(sp)
    lw s0, 16(sp)
    lw s1, 20(sp)
    addi sp, sp, 24

    jr ra

exception1:
    li a0, 27
    j exit

exception2:
    li a0, 30
    j exit

exception3:
    li a0, 28
    j exit