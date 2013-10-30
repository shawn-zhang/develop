.file "hello.s"
.data
.align 4
msg:
    .string "hello world\n"
    len = . - msg
.text
    .globl main
    .type main, @function
main:
    
