.globl classify

.text
# =====================================
# COMMAND LINE ARGUMENTS
# =====================================
# Args:
#   a0 (int)        argc
#   a1 (char**)     argv
#   a1[1] (char*)   pointer to the filepath string of m0
#   a1[2] (char*)   pointer to the filepath string of m1
#   a1[3] (char*)   pointer to the filepath string of input matrix
#   a1[4] (char*)   pointer to the filepath string of output file
#   a2 (int)        silent mode, if this is 1, you should not print
#                   anything. Otherwise, you should print the
#                   classification and a newline.
# Returns:
#   a0 (int)        Classification
# Exceptions:
#   - If there are an incorrect number of command line args,
#     this function terminates the program with exit code 31
#   - If malloc fails, this function terminates the program with exit code 26
#
# Usage:
#   main.s <M0_PATH> <M1_PATH> <INPUT_PATH> <OUTPUT_PATH>
classify:
    li t0, 5
    bne a0, t0, argument_error

    addi sp, sp, -56
    sw s0, 0(sp)
    sw s1, 4(sp)
    sw s2, 8(sp)
    sw s3, 12(sp)
    sw s4, 16(sp)
    sw s5, 20(sp)
    sw s6, 24(sp)
    sw s7, 28(sp)
    sw s8, 32(sp)
    sw s9, 36(sp)
    sw s10, 40(sp)
    sw s11, 44(sp)
    sw a2, 48(sp)
    sw ra, 52(sp)

    mv s0, a1                 # s0 points to argv


    # Read pretrained m0
    li a0, 4
    jal ra, malloc
    beq a0, x0, malloc_error
    mv s2, a0                 # s2 points to row_num of m0

    li a0, 4
    jal ra, malloc
    beq a0, x0, malloc_error
    mv s3, a0                 # s3 points to col_num of m0

    mv a2, s3
    mv a1, s2
    lw a0, 4(s0)
    jal ra, read_matrix
    mv s4, a0                 # s4 points to m0


    # Read pretrained m1
    li a0, 4
    jal ra, malloc
    beq a0, x0, malloc_error
    mv s5, a0                 # s5 points to row_num of m1

    li a0, 4
    jal ra, malloc
    beq a0, x0, malloc_error
    mv s6, a0                 # s6 points to col_num of m1

    mv a2, s6
    mv a1, s5
    lw a0, 8(s0)
    jal ra, read_matrix
    mv s7, a0                 # s7 points to m1


    # Read input matrix
    li a0, 4
    jal ra, malloc
    beq a0, x0, malloc_error
    mv s8, a0                 # s8 points to row_num of input_matrix

    li a0, 4
    jal ra, malloc
    beq a0, x0, malloc_error
    mv s9, a0                 # s9 points to col_num of input_matrix

    mv a2, s9
    mv a1, s8
    lw a0, 12(s0)
    jal ra, read_matrix
    mv s10, a0                # s10 points to input_matrix


    # Compute h = matmul(m0, input)
    lw t0, 0(s2)
    lw t1, 0(s9)
    mul a0, t0, t1
    slli a0, a0, 2
    jal ra, malloc
    beq a0, x0, malloc_error
    mv s11, a0                # s11 points to h

    mv a0, s4
    lw a1, 0(s2)
    lw a2, 0(s3)
    mv a3, s10
    lw a4, 0(s8)
    lw a5, 0(s9)
    mv a6, s11
    jal ra, matmul


    # Compute h = relu(h)
    mv a0, s11
    lw t0, 0(s2)
    lw t1, 0(s9)
    mul a1, t0, t1
    jal ra, relu


    # Free s3, since it represents col_num of m0, which is of no use afterwards
    mv a0, s3
    jal ra, free


    # Compute o = matmul(m1, h)
    lw t0, 0(s5)
    lw t1, 0(s9)
    mul a0, t0, t1
    slli a0, a0, 2
    jal ra, malloc
    beq a0, x0, malloc_error
    mv s3, a0                 # s3 points to o

    mv a0, s7
    lw a1, 0(s5)
    lw a2, 0(s6)
    mv a3, s11
    lw a4, 0(s2)
    lw a5, 0(s9)
    mv a6, s3
    jal ra, matmul


    # Write output matrix o
    lw a0, 16(s0)
    mv a1, s3
    lw a2, 0(s5)
    lw a3, 0(s9)
    jal ra, write_matrix


    # Compute and return argmax(o)
    mv a0, s3
    lw t0, 0(s5)
    lw t1, 0(s9)
    mul a1, t0, t1
    jal ra, argmax
    mv s1, a0                 # s1 is result


    # If enabled, print argmax(o) and newline
    li t0, 1
    lw t1, 48(sp)
    beq t1, t0, end
    mv a0, s1
    jal ra, print_int
    li a0, '\n'
    jal ra, print_char

end:
    mv a0, s2
    jal ra, free
    mv a0, s3
    jal ra, free
    mv a0, s4
    jal ra, free
    mv a0, s5
    jal ra, free
    mv a0, s6
    jal ra, free
    mv a0, s7
    jal ra, free
    mv a0, s8
    jal ra, free
    mv a0, s9
    jal ra, free
    mv a0, s10
    jal ra, free
    mv a0, s11
    jal ra, free

    mv a0, s1

    lw s0, 0(sp)
    lw s1, 4(sp)
    lw s2, 8(sp)
    lw s3, 12(sp)
    lw s4, 16(sp)
    lw s5, 20(sp)
    lw s6, 24(sp)
    lw s7, 28(sp)
    lw s8, 32(sp)
    lw s9, 36(sp)
    lw s10, 40(sp)
    lw s11, 44(sp)
    lw ra, 52(sp)
    addi sp, sp, 56

    jr ra

malloc_error:
    li a0, 26
    j exit

argument_error:
    li a0, 31
    j exit