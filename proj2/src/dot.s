.globl dot

.text
# =======================================================
# FUNCTION: Dot product of 2 int arrays
# Arguments:
#   a0 (int*) is the pointer to the start of arr0
#   a1 (int*) is the pointer to the start of arr1
#   a2 (int)  is the number of elements to use
#   a3 (int)  is the stride of arr0
#   a4 (int)  is the stride of arr1
# Returns:
#   a0 (int)  is the dot product of arr0 and arr1
# Exceptions:
#   - If the number of elements to use is less than 1,
#     this function terminates the program with error code 36
#   - If the stride of either array is less than 1,
#     this function terminates the program with error code 37
# =======================================================
dot:
    li t3, 1
    blt a2, t3, exception1
    blt a3, t3, exception2
    blt a4, t3, exception2

    # Prologue
    # no prologue

loop_start:
    mv t0, a0       # t0 points to each element in array1
    mv t1, a1       # t1 points to each element in array2
    li t2, 0        # t2 stores the number of elements used
    li t3, 0        # t3 stores result

loop_continue:
    bge t2, a2, loop_end
    lw t4, 0(t0)
    lw t5, 0(t1)
    mul t4, t4, t5
    add t3, t3, t4
    slli t4, a3, 2
    add t0, t0, t4
    slli t4, a4, 2
    add t1, t1, t4
    addi t2, t2, 1
    j loop_continue

loop_end:
    mv a0, t3

    # Epilogue
    # no epilogue

    jr ra

exception1:
    li a0, 36
    j exit

exception2:
    li a0, 37
    j exit
