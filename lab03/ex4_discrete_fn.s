.globl f # this allows other files to find the function f

# f takes in two arguments:
# a0 is the value we want to evaluate f at
# a1 is the address of the "output" array (read the lab spec for more information).
# The return value should be stored in a0
f:
    # Your code here
    addi t0 a0 3  # 将a0转换为索引0~6，存至t0
    slli t0 t0 2  # 将t0逻辑左移2位（即乘4），转换为字节偏移量0、4、8、...、24
    add t0 a1 t0  # t0=a1+t0，数组基址+偏移量，得到答案的地址
    lw a0 0(t0)   # loadword，访问答案的值，存入返回值a0

    # This is how you return from a function. You'll learn more about this later.
    # This should be the last line in your program.
    jr ra
