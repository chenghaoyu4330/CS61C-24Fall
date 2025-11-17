.globl relu

.text
# ==============================================================================
# FUNCTION: Performs an inplace element-wise ReLU on an array of ints
# Arguments:
#   a0 (int*) is the pointer to the array
#   a1 (int)  is the # of elements in the array
# Returns:
#   None
# Exceptions:
#   - If the length of the array is less than 1,
#     this function terminates the program with error code 36
# ==============================================================================
relu:
    # Prologue
    # no prologue

    li t3, 1
    blt a1, t3, exception

loop_start:
    mv t0, a0       # t0 points to each element in array
    slli t1, a1, 2
    add t1, t1, a0  # t1 points to the follower of the last element

loop_continue:
    bge t0, t1, loop_end
    lw t2, 0(t0)
    addi t0, t0, 4
    bge t2, zero, loop_continue
    sw x0, -4(t0)
    j loop_continue

loop_end:
    # Epilogue
    # no epilogue

    jr ra

exception:
    li a0, 36
    j exit