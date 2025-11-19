.globl matmul

.text
# =======================================================
# FUNCTION: Matrix Multiplication of 2 integer matrices
#   d = matmul(m0, m1)
# Arguments:
#   a0 (int*)  is the pointer to the start of m0
#   a1 (int)   is the # of rows (height) of m0
#   a2 (int)   is the # of columns (width) of m0
#   a3 (int*)  is the pointer to the start of m1
#   a4 (int)   is the # of rows (height) of m1
#   a5 (int)   is the # of columns (width) of m1
#   a6 (int*)  is the pointer to the the start of d
# Returns:
#   None (void), sets d = matmul(m0, m1)
# Exceptions:
#   Make sure to check in top to bottom order!
#   - If the dimensions of m0 do not make sense,
#     this function terminates the program with exit code 38
#   - If the dimensions of m1 do not make sense,
#     this function terminates the program with exit code 38
#   - If the dimensions of m0 and m1 don't match,
#     this function terminates the program with exit code 38
# =======================================================
matmul:
    # Error checks
    li t0, 1
    blt a1, t0, exception
    blt a2, t0, exception
    blt a4, t0, exception
    blt a5, t0, exception
    bne a2, a4, exception

    # Prologue
    # no prologue

    mv t0, a0  # m0 row ptr
    mv t1, a3  # m1 column ptr
    li t2, 0   # m0 row index
    li t3, 0   # m1 column index
    mv t6, a6  # points to result matrix element

outer_loop_start:
    bge t2, a1, outer_loop_end

inner_loop_start:
    bge t3, a5, inner_loop_end

    addi sp, sp, -48
    sw a0, 0(sp)
    sw a1, 4(sp)
    sw a2, 8(sp)
    sw a3, 12(sp)
    sw a4, 16(sp)
    sw a5, 20(sp)
    sw t0, 24(sp)
    sw t1, 28(sp)
    sw t2, 32(sp)
    sw t3, 36(sp)
    sw t6, 40(sp)
    sw ra, 44(sp)

    mv a0, t0
    mv a1, t1
    li a3, 1
    mv a4, a5

    jal ra, dot

    lw t6, 40(sp)
    sw a0, 0(t6)

    lw a0, 0(sp)
    lw a1, 4(sp)
    lw a2, 8(sp)
    lw a3, 12(sp)
    lw a4, 16(sp)
    lw a5, 20(sp)
    lw t0, 24(sp)
    lw t1, 28(sp)
    lw t2, 32(sp)
    lw t3, 36(sp)
    lw ra, 44(sp)
    addi sp, sp, 48

    addi t6, t6, 4
    addi t1, t1, 4
    addi t3, t3, 1
    j inner_loop_start

inner_loop_end:
    li t3, 0
    mv t1, a3
    addi t2, t2, 1
    slli t4, a2, 2
    add t0, t0, t4
    j outer_loop_start

outer_loop_end:
    # Epilogue
    # no epilogue

    jr ra

exception:
    li a0, 38
    j exit