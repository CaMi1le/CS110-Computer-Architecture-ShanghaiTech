# ShanghaiTech CS110 2019 Project 1.2

#==============================================================================
#                              Project 1-2 Part 1
#                               String README
#==============================================================================
# In this file you will be implementing some utilities for manipulating strings.
# The functions you need to implement are:
#  - strlen()
#  - strncpy()
#  - copy_of_str()
#  - streq()
#  - dec_to_str()
# Test partition also in this file, if you want to change test partition,
# make sure you have totally understood the test case before you changing them.
#==============================================================================
.data
newline:    .asciiz "\n"
tab:        .asciiz "\t"

tc_actual_str:      .asciiz "actual:   "
tc_expected_str:    .asciiz "expected: "
tc_testpass_str:    .asciiz " ...test passes\n"
tc_testfail_str:    .asciiz " ...test FAILS\n"

#-------------------------------------------------------
# Test Data - Feel free to add your own after this line
#-------------------------------------------------------
test_str1:      .asciiz "a9bw enijn webb"
test_str2:      .asciiz "a9bw en"
test_str3:      .asciiz ""

test_num1:      .asciiz "4396"
test_num2:      .asciiz "77777777"
test_num3:      .asciiz "1011"
#-------------------------------------------------------
# Test Data - Feel free to add your own above this line
#-------------------------------------------------------

test_header_name:       .asciiz "Running string tests:\n"
test_strlen_name:       .asciiz "Testing strlen():\n"
test_strncpy_name:      .asciiz "Testing strncpy():\n"
test_copy_of_str_name:  .asciiz "Testing copy_of_str():\n"
test_dec_to_str_name:   .asciiz "Testing dec_to_str():\n"

# 20 byte buffer x 5 = 100 byte buffer
test_buffer:    .word 0xFFFFFFFF 0xFFFFFFFF 0xFFFFFFFF 0xFFFFFFFF 0xFFFFFFFF
                .word 0xFFFFFFFF 0xFFFFFFFF 0xFFFFFFFF 0xFFFFFFFF 0xFFFFFFFF
                .word 0xFFFFFFFF 0xFFFFFFFF 0xFFFFFFFF 0xFFFFFFFF 0xFFFFFFFF
                .word 0xFFFFFFFF 0xFFFFFFFF 0xFFFFFFFF 0xFFFFFFFF 0xFFFFFFFF
                .word 0xFFFFFFFF 0xFFFFFFFF 0xFFFFFFFF 0xFFFFFFFF 0xFFFFFFFF

.globl main
.text
#------------------------------------------------------------------------------
# function streq(), a basic function of this project, implement first.
# And we haven't add any separate test cases of this, but nearly any other
# test case will use this function, so make sure your implementation correct.
#------------------------------------------------------------------------------
# Arguments:
#  a1 = string 1
#  a2 = string 2
#
# Returns: 0 if string 1 and string 2 are equal, -1 if they are not equal
#
# CLOBBERS: a0, a1, a2, t0, t1
#------------------------------------------------------------------------------
streq:
    ### YOUR CODE ###
    # Initialize the reuturn value with -1
    li a0, -1
str_eq_loop:
    # Read the current character of string 1
    lbu t0, 0(a1)
    # Read the current character of string 2
    lbu t1, 0(a2)
    # Check if t0 and t1 are equal. If so, skip the next line and proceed the loop.
    # Otherwise, the next line will be executed, which reports inequality.
    beq t0, t1, str_eq_proceed
    # Break loop and report inequality by returning a0(already set to -1) directly.
    j str_neq_end
str_eq_proceed:
    # Check if having reached the \0
    # Note that, the if program can run to this line, string 1 and string 2
    # must be equal so far, thus, we only need to check one of them
    # here t0 is chosen
    beq t0, zero, str_eq_end
    # Move one byte forward, as a character only takes one byte
    addi a1, a1, 1
    addi a2, a2, 1
    # Go to check the next character in two strings
    j str_eq_loop
    # When jumping to here, it means two strings are equal and traversed to
    # the \0. Set a0 to zero, indicating the equality holds.
str_eq_end:
    li a0, 0
    # Program can run to the following line in two ways.
    # First, the inequality is detected and directly jump to here.
    # Second, program run out of loop and set the equality, then exit
    # normally through here.
str_neq_end:
    # Go back to the caller. Essential step.
    jr ra

#------------------------------------------------------------------------------
# function strlen()
#------------------------------------------------------------------------------
# Arguments:
#  a1 = string input
#
# Returns: the length of the string
#
# CLOBBERS: a0, t0
#------------------------------------------------------------------------------
strlen:
    ### YOUR CODE ###
    # Initialize the reuturn value with 0
    li a0, 0
str_len_loop:
    # Calculate the address of the current character by the offset a0
    add t0, a1, a0
    # Read the current character of the string
    lbu t0, 0(t0)
    # Check if having reached the \0
    beq t0, zero, str_len_end
    # Accumulate length of string
    # Here, a0 also play a role as the index of next character
    addi a0, a0, 1
    # Go to check the next character
    j str_len_loop
str_len_end:
    # Go back to the caller. Essential step.
    jr ra

#------------------------------------------------------------------------------
# function strncpy()
#------------------------------------------------------------------------------
# Arguments:
#  a1 = pointer to destination array
#  a2 = source string
#  a3 = number of characters to copy
#
# Returns: the destination array
#
# CLOBBERS: a0, t0, t1, t2
#------------------------------------------------------------------------------
strncpy:
    ### YOUR CODE ###
    # Initialize the destination array to a1, for the head of array never changed
    mv a0, a1
    # Initialize the loop variable with 0, for the start index
    li t0, 0
str_ncpy_loop:
    # Quit loop if n copys have been done
    bge t0, a3, str_ncpy_end

    # Calculate the address of the current character by the offset t0
    add t1, a2, t0
    # Read the current character of source
    lbu t1, 0(t1)

    # Calculate the address of the current character by the offset a0
    add t2, a1, t0
    # Write the retrieved character from source back to the destination array
    sb t1, 0(t2)

    # Accumulate index
    addi t0, t0, 1

    # Go to check the next character
    j str_ncpy_loop
str_ncpy_end:
    # Append \0 onto the destination array
    # Note that, t2 now still contains a pointer which points to the last character
    # of destination. Simply append a \0 through one byte offset.
    li t1, 0
    sb t1, 1(t2)

    # Go back to the caller. Essential step.
    jr ra

#------------------------------------------------------------------------------
# function copy_of_str()
#------------------------------------------------------------------------------
# Creates a copy of a string. You will need to use sbrk (ecall 9) to allocate
# space for the string. See wiki for details. strlen() and strncpy() will be
# helpful for this function. ATTENTION: Be careful of the allocation space.
# Taking a second thought on how large (strlen or strlen + 1) should we allocate.
#
# Arguments:
#   a1 = string to copy
#
# Returns: pointer to the copy of the string
#
# CLOBBERS: a0, a2, a3, t0
#------------------------------------------------------------------------------
copy_of_str:
    ### YOUR CODE ###
    #--------------------------------------------------------------------------
    # Obtain the length of copy
    #--------------------------------------------------------------------------
    # Reserve 8 bytes for registers
    addi sp, sp, -8
    sw ra, 0(sp)    # Push ra
    sw a1, 4(sp)    # Push a1

    # No need to setup arguments, since a1 has already been the source string
    # whose length we want to obtain.

    # Call strlen
    jal ra, strlen
    # Now the length of source are stored in a0, move it to t0
    mv t0, a0

    lw ra, 0(sp)    # Pop ra
    lw a1, 4(sp)    # Pop a1
    # Restore stack
    addi sp, sp, 8
    #--------------------------------------------------------------------------

    #--------------------------------------------------------------------------
    # Allocate buffer for string
    #--------------------------------------------------------------------------
    # Reserve 12 bytes for registers
    # Note that, store all registers we're using for ecall is possible to change
    # any register required to be maintained by caller.
    addi sp, sp, -12
    sw ra, 0(sp)    # Push ra
    sw a1, 4(sp)    # Push a1
    sw t0, 8(sp)    # Push t0

    # Setup arguments
    # Specify ecall id, 9 for sbrk
    li a0, 9
    # Specify the buffer size
    # Note that, t0 now is the length of source. Adding up one reserves the
    # space for the last \0.
    addi a1, t0, 1

    # Ecall: Call sbrk to allocate strlen + 1 size for string
    ecall
    # Now the address of allocated string is returned to a0

    # Restore registers
    lw ra, 0(sp)    # Pop ra
    lw a1, 4(sp)    # Pop a1
    lw t0, 8(sp)    # Pop t0
    addi sp, sp, 12
    #--------------------------------------------------------------------------

    #--------------------------------------------------------------------------
    # Copy string
    #--------------------------------------------------------------------------
    # Reserve 12 bytes for registers
    addi sp, sp, -12
    sw ra, 0(sp)    # Push ra
    sw a1, 4(sp)    # Push a1
    sw t0, 8(sp)    # Push t0

    # Setup arguments
    # Specify source. This step must come before destination, because it requires
    # the value of a1 containing the source address
    mv a2, a1
    # Specify destination
    mv a1, a0
    # Specify the length to copy. Note that it's strlen instead of strlen + 1.
    mv a3, t0

    # Call strncpy
    jal ra, strncpy

    # Now the address of duplicated string is returned to a0

    # Restore registers
    lw ra, 0(sp)    # Pop ra
    lw a1, 4(sp)    # Pop a1
    lw t0, 8(sp)    # Pop t0
    addi sp, sp, 12
    #--------------------------------------------------------------------------

    # Go back to the caller. Essential step.
    jr ra


#------------------------------------------------------------------------------
# function dec_to_str()
#------------------------------------------------------------------------------
# Convert a number to its unsigned decimal integer string representation, eg.
# 35 => "35", 1024 => "1024".
#
# Arguments:
#  a1 = int to write
#  a2 = character buffer to write into
#
# Returns: the number of digits written
#
# CLOBBERS: a0, a1, t0, t1, t2, t3
#------------------------------------------------------------------------------
dec_to_str:
    ### YOUR CODE ###
    # Initialize the digits written a0 with 0
    li a0, 0
    # Initialize the denominator t0 with 10
    li t0, 10
str_dec2str_loop:
    #--------------------------------------------------------------------------
    # Get the last digit
    # Note that, here we only care about unsigned integer, which means all
    # arithmetic operators must subject to unsigned integer.
    # Note that, the following algorithm only gets the reverse result. Hence,
    # reverse process is required. Actually, the following algorithm takes least
    # div/rem operation, improving efficiency.
    #--------------------------------------------------------------------------
    # Get last digit by calculating the remainder of current decimal number
    # divided by 10.
    remu t1, a1, t0
    # now current digit is saved in t1. We need to truncate the last digit for
    # the next loop afterward.

    # Divided by 10
    divu a1, a1, t0
    # the resultant quotient is the remaining digits
    #--------------------------------------------------------------------------

    #--------------------------------------------------------------------------
    # Convert the currently obtained digit
    #--------------------------------------------------------------------------
    # Map an integer t1 in [0, 9] to character ['0', '9']
    # According to the ASCII table, the mapping offset shall be 48
    addi t1, t1, 48
    # Calculate the address of the current character by the offset a0
    add t2, a2, a0
    # Save t1 to the current character
    sb t1, 0(t2)
    #--------------------------------------------------------------------------

    # Accumulate a0
    addi a0, a0, 1

    # Put the conditional branch here. Considering the input argument might be 0,
    # ensure loop at least one time
    beq a1, zero, str_dec2str_reverse

    # Go to convert the next digits
    j str_dec2str_loop

str_dec2str_reverse:
    # Here reuse t0 as the loop variable
    mv t0, a2
    # Here reuse t1 as the reverse correspondence
    add t1, a2, a0
    addi t1, t1, -1

str_dec2str_reverse_loop:
    # If t0 meets t1, end reverse
    bge t0, t1, str_dec2str_end

    #--------------------------------------------------------------------------
    # Swap two character
    #--------------------------------------------------------------------------
    # Load character at t0
    lb t2, 0(t0)
    # Load character at t1
    lb t3, 0(t1)
    # Save character at t1 to t0
    sb t3, 0(t0)
    # Save character at t0 to t1
    sb t2, 0(t1)
    #--------------------------------------------------------------------------

    # Accumulate t0
    addi t0, t0, 1
    # Decrement t1
    addi t1, t1, -1

    # Go to swap the next pair
    j str_dec2str_reverse_loop

str_dec2str_end:
    #--------------------------------------------------------------------------
    # Append \0 onto the string buffer
    #--------------------------------------------------------------------------
    # Set t1 to 0, i.e '\0'
    li t1, 0
    # Calculate the address of end identifier '\0'
    add t2, a2, a0
    # Save t1, i.e '\0', to the calculated address
    sb t1, 0(t2)
    #--------------------------------------------------------------------------

    # Go back to the caller. Essential step.
    jr ra

###############################################################################
#                 Sample test cases, BE CAREFUL to change them
###############################################################################
#==============================================================================
#                              string.s Test Cases
#==============================================================================
#-------------------------------------------
# Prints a newline
# CLOBBERS: a1 and a0
#-------------------------------------------
print_newline:
    la a1, newline
    li a0, 4
    ecall
    jr ra

#-------------------------------------------
# Prints the string
#  a1 = label containing address of string
# CLOBBERS: a1 and a0
#-------------------------------------------
print_str:
    li a0, 4
    ecall
    jr ra

#-------------------------------------------
# Checks whether the actual result matches the expected result
# Treats arguments as unsigned ints
#  a1 = register containing actual result
#  a2 = expected value of register
# CLOBBERS: a2, a1, a0, and t0
#-------------------------------------------
check_uint_equals:
    addi sp, sp, -4
    sw ra, 0(sp)
    mv t0, a1

    la a1, tc_actual_str
    jal print_str

    mv a1, t0
    li a0, 1
    ecall

    jal print_newline

    la a1, tc_expected_str
    jal print_str

    mv a1, a2
    li a0, 1
    ecall

    bne t0, a1, check_uint_notequal
    jal print_newline
    la a1, tc_testpass_str
    jal print_str
    j check_uint_end
check_uint_notequal:
    jal print_newline
    la a1, tc_testfail_str
    jal print_str
check_uint_end:
    lw ra, 0(sp)
    addi sp, sp, 4
    jr ra


#-------------------------------------------
# Checks whether the actual result matches the expected result
# Arguments should be strings
#  a1 = register containing actual result
#  a2 = label to expected string
# CLOBBERS: a2, a1, a0, and t0
#-------------------------------------------
check_str_equals:
    addi sp, sp, -4
    sw ra, 0(sp)
    mv t0, a1

    la a1, tc_actual_str
    jal print_str

    mv a1, t0
    li a0, 4
    ecall

    jal print_newline

    la a1, tc_expected_str
    jal print_str

    mv a1, a2
    li a0, 4
    ecall

    jal print_newline

    mv a1, t0
    jal streq
    bne a0, zero, check_str_notequal
    la a1, tc_testpass_str
    jal print_str
    j check_str_end
check_str_notequal:
    la a1, tc_testfail_str
    jal print_str
check_str_end:
    lw ra, 0(sp)
    addi sp, sp, 4
    jr ra

#-------------------------------------------
# Test driver
#-------------------------------------------
main:
    la a1, test_header_name
    jal print_str
    jal print_newline

    jal test_strlen
    jal print_newline

    jal test_strncpy
    jal print_newline

    jal test_copy_of_str
    jal print_newline

    jal test_dec_to_str

    li a0, 10
    ecall

#-------------------------------------------
# Tests strlen()
#-------------------------------------------
test_strlen:
    addi sp, sp, -4
    sw ra, 0(sp)
    la a1, test_strlen_name
    jal print_str

    la a1, test_str1
    jal strlen
    mv a1, a0
    li a2, 15
    jal check_uint_equals

    la a1, test_str3
    jal strlen
    mv a1, a0
    li a2, 0
    jal check_uint_equals

    la a1, test_str2
    jal strlen
    mv a1, a0
    li a2, 7
    jal check_uint_equals

    lw ra, 0(sp)
    addi sp, sp, 4
    jr ra

#-------------------------------------------
# Tests strncpy()
#-------------------------------------------
test_strncpy:
    addi sp, sp, -4
    sw ra, 0(sp)
    la a1, test_strncpy_name
    jal print_str

    la a1, test_buffer
    la a2, test_str1
    li a3, 16       # note: len + 1
    jal strncpy

    mv a1, a0
    la a2, test_str1
    jal check_str_equals

    la a1, test_buffer
    la a2, test_str2
    li a3, 8        # note: len + 1
    jal strncpy

    mv a1, a0
    la a2, test_str2
    jal check_str_equals

    lw ra, 0(sp)
    addi sp, sp, 4
    jr ra

#-------------------------------------------
# Tests copy_of_str()
#-------------------------------------------
test_copy_of_str:
    addi sp, sp, -12
    sw s1, 8(sp)
    sw s2, 4(sp)
    sw ra, 0(sp)
    la a1, test_copy_of_str_name
    jal print_str

    la a1, test_str1
    jal copy_of_str
    mv s1, a0

    mv a1, s1
    la a2, test_str1
    jal check_str_equals

    la a1, test_str2
    jal copy_of_str
    mv s2, a0

    mv a1, s2
    la a2, test_str2
    jal check_str_equals

    lw s1, 8(sp)
    lw s2, 4(sp)
    lw ra, 0(sp)
    addi sp, sp, 12
    jr ra

#-------------------------------------------
# Tests dec_to_str()
#-------------------------------------------
test_dec_to_str:
    addi sp, sp, 4
    sw ra, 0(sp)

    la a1, test_dec_to_str_name
    jal print_str

    li a1, 4396
    la a2, test_buffer
    jal dec_to_str
    la a1, test_buffer
    la a2, test_num1
    jal check_str_equals

    li a1, 77777777
    la a2, test_buffer
    jal dec_to_str
    la a1, test_buffer
    la a2, test_num2
    jal check_str_equals

    li a1, 001011
    la a2, test_buffer
    jal dec_to_str
    la a1, test_buffer
    la a2, test_num3
    jal check_str_equals

    lw ra, 0(sp)
    addi sp, sp, -4
    jr ra
