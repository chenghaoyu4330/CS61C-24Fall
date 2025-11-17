.globl argmax

.text
# =================================================================
# FUNCTION: Given a int array, return the index of the largest
#   element. If there are multiple, return the one
#   with the smallest index.
# Arguments:
#   a0 (int*) is the pointer to the start of the array
#   a1 (int)  is the # of elements in the array
# Returns:
#   a0 (int)  is the first index of the largest element
# Exceptions:
#   - If the length of the array is less than 1,
#     this function terminates the program with error code 36
# =================================================================
argmax:
    li t3, 1
    blt a1, t3, exception
    
    # Prologue
    addi sp, sp, -4
    sw s0, 0(sp)

loop_start:
    mv t0, a0       # t0 points to each element in array
    slli t1, a1, 2
    add t1, t1, a0  # t1 points to the follower of the last element
    lw t2, 0(a0)    # t2 stores the max element
    li s0, 0        # s0 stores the index of the max element

loop_continue:
    bge t0, t1, loop_end  # loop exit condition
    lw t3, 0(t0)          # t3 stores each element
    blt t2, t3, process   # update max element
    addi t0, t0, 4
    j loop_continue
    process:
        mv t2, t3
        sub s0, t0, a0    # store offset bytes
        srli s0, s0, 2    # calculate offset index
        addi t0, t0, 4
        j loop_continue

loop_end:
    mv a0, s0

    # Epilogue
    lw s0, 0(sp)
    addi sp, sp, 4

    jr ra

exception:
    li a0, 36
    j exit
