# ShanghaiTech CS110 2019 Project 1.2

#==============================================================================
#                              Project 1-2 Part 3
#                              parsetools.s README
#==============================================================================
# You should implement hex_to_str() and parse_int(). If you want to change test
# partition, make sure you have totally understood the test case before you
# changing them.
#==============================================================================

.data
#-------------------------------------------------------
# Test Data - Feel free to add your own after this line
#-------------------------------------------------------
# Used in hex_to_str
hex_str1:   .asciiz "abcdef12\n"
hex_str2:   .asciiz "00034532\n"

# Used in parse_int
hex_str3:   .asciiz "abcdef12"
hex_str4:   .asciiz "00034532"
dec_str1:   .asciiz "12345983"
dec_str2:   .asciiz "00023420"
#-------------------------------------------------------
# Test Data - Feel free to add your own above this line
#-------------------------------------------------------

test_header_name:   .asciiz "Running test parsetools:\n"
test_hex_to_str_name:   .asciiz "Testing hex_to_str():\n"
test_parse_int_name: .asciiz "Testing parse_int():\n"

newline:   .asciiz "\n"

tc_actual_str:      .asciiz "actual:   "
tc_expected_str:    .asciiz "expected: "
tc_testpass_str:    .asciiz " ...test passes\n"
tc_testfail_str:    .asciiz " ...test FAILS\n"

readline_err_ecall:
    .asciiz "Error in readline: Could not read from file.\n"
readline_err_bufsize:
    .asciiz "Error in readline: Exceeded maximum buffer size.\n"

.globl main
.text
#------------------------------------------------------------------------------
# function hex_to_str()
#------------------------------------------------------------------------------
# Writes a 32-bit number in hexadecimal format to a string buffer, followed by
# the newline character and the NUL-terminator. The output must contain 8 digits
# so if neccessary put leading 0s in the buffer. Therefore, you should always
# be writing 10 characters (8 digits, 1 newline, 1 NUL-terminator).
#
# For example:
#  0xabcd1234 => "abcd1234\n\0"
#  0x134565FF => "134565ff\n\0"
#  0x38       => "00000038\n\0"
#
# Write hex letters using lowercase, not uppercase. Do not add the prefix '0x'.
#
# Hint: Consider each group of 4 bits at a time and look at an ASCII table. If
# you code has more than a few branch statements, you are probably not doing
# things very efficiently.
# Attention: Its a bit different from dec_to_str, not just copy it.
#
# Arguments:
#  a1 = int to write
#  a2 = character buffer to write into
#
# Returns: none
#------------------------------------------------------------------------------
hex_to_str:
    ### YOUR CODE ###
    # base of number: 16
    li t0, 0x10
    # the number of loop
    li t1, 0x00
    # number to separate 9 and A
    li t2, 0x0A
    # upper bound of loop times
    li t4, 0x08

    # push stack
    addi sp, sp, -4
    # save a1
    sw a1, 0(sp)

    # move a2 to the highest address
    addi a2, a2, 7
    # \0 character
    li t3, 0
    # store \0
    sb t3, 2(a2)
    # \n character
    li t3, 10
    # store \n
    sb t3, 1(a2)

# loop through each number of the int
prs_hex2str_loop:
    # if loop times reach 8
    bge t1, t4, prs_hex2str_end

    # save floor(a1 / 16) to t3
    srli t3, a1, 4
    # save floor(a1 / 16) * 16 to t3
    slli t3, t3, 4
    # save the remainder in t3
    sub t3, a1, t3
    # int divided by base
    srli a1, a1, 4

    # if the number is greater or equal to 'A'
    bge t3, t2, prs_hex2str_A_to_F_branch
# if the number is among 0 1 ... 9
prs_hex2str_zero_to_nine_branch:
    # map [0 1 ... 9] to the their ASCII codes
    addi t3, t3, 48
    # jump to loop end
    j prs_hex2str_loop_end
# if the number is among A B ... F
prs_hex2str_A_to_F_branch:
    # map [A B ... F] to the their ASCII codes
    addi t3, t3, 87

# end of each loop
prs_hex2str_loop_end:
    # store the number to destination
    sb t3, 0(a2)
    # move to next byte
    addi a2, a2, -1
    # increase the number of processed numbers
    addi t1, t1, 1
    # jump back to keep loop
    j prs_hex2str_loop

# end of the function
prs_hex2str_end:
    # restore a1
    lw a1, 0(sp)
    # pop stack
    addi sp, sp, 4
    # move a2 to the first character
    addi a2, a2, 1

    # return
    jr ra

#------------------------------------------------------------------------------
# function parse_int()
#------------------------------------------------------------------------------
# Parses the string as an unsigned integer. The only bases supported are 10 and
# 16. We will assume that the number is valid, and that overflow does not happen.
#
# Arguments:
#  a1 = string containing a number
#  a2 = base (will be either 10 or 16)
#
# Returns: the number
#------------------------------------------------------------------------------
parse_int:
    ### YOUR CODE ###
    # Initialize a0 with zero
    li a0, 0
prs_int_loop:
    # load current character
    lbu t0, 0(a1)

    # if meet the tail '\0'
    beqz t0, prs_int_end

    li t1, 97   # lower bound of small letter
    # if falls in ['a'...'f']
    bge t0, t1, prs_int_small_letter
    li t1, 65   # lower bound of capital letter
    # if falls in ['A'...'F']
    bge t0, t1, prs_int_cap_letter
    li t1, 48   # lower bound of digits
    # if falls in ['0'...'9']
    bge t0, t1, prs_int_digit

prs_int_small_letter:
    # map 'a'...'f' to 10..15
    addi t0, t0, -87
    # skip following conversion
    j prs_int_calc
prs_int_cap_letter:
    # map 'A'...'F' to 10..15
    addi t0, t0, -55
    # skip following conversion
    j prs_int_calc
prs_int_digit:
    # map '0'...'9' to 0..9
    addi t0, t0, -48
    # skip following conversion
    j prs_int_calc

prs_int_calc:
    # a0 = a0 * base
    mul a0, a0, a2
    # a0 = a0 * base + t0
    add a0, a0, t0

    # move to next character
    addi a1, a1, 1
    # start next loop
    j prs_int_loop

# end of parse_int function
prs_int_end:
    jr ra  # return

#------------------------------------------------------------------------------
# function streq() - paste your implementation here
#------------------------------------------------------------------------------
# Arguments:
#  a1 = string 1
#  a2 = string 2
#
# Returns: 0 if string 1 and string 2 are equal, -1 if they are not equal
#------------------------------------------------------------------------------
streq:
    ### PASTE YOUR CODE IN string.s HERE ###
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

###############################################################################
#                 Sample test cases, BE CAREFUL to change them
###############################################################################
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
# Treats arguments as signed ints
#  a1 = register containing actual result
#  a2 = expected value of register
# CLOBBERS: a2, a1, a0, and t0
#-------------------------------------------
check_int_equals:
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

    bne t0, a1, check_int_notequal
    jal print_newline
    la a1, tc_testpass_str
    jal print_str
    j check_int_end
check_int_notequal:
    jal print_newline
    la a1, tc_testfail_str
    jal print_str
check_int_end:
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
    jal test_hex_to_str

    jal print_newline
    jal test_parse_int

    li a0, 10
    ecall

#-------------------------------------------
# Tests hex_to_str()
#-------------------------------------------
test_hex_to_str:
    addi sp, sp, -8
    sw s1, 4(sp)
    sw ra, 0(sp)
    la a1, test_hex_to_str_name
    jal print_str

    li a0, 9
    li a1, 10
    ecall
    mv s1, a0

    li a1, 2882400018
    mv a2, s1
    jal hex_to_str
    mv a1, s1
    la a2, hex_str1
    jal check_str_equals

    li a1, 214322
    mv a2, s1
    jal hex_to_str
    mv a1, s1
    la a2, hex_str2
    jal check_str_equals

    lw ra, 0(sp)
    lw s1, 4(sp)
    addi sp, sp, 8
    jr ra

#-------------------------------------------
# Tests parse_int()
#-------------------------------------------
test_parse_int:
    addi sp, sp, -4
    sw ra, 0(sp)

    la a1, test_parse_int_name
    jal print_str

    la a1, hex_str3
    li a2, 16
    jal parse_int
    mv a1, a0
    li a2, 0xabcdef12
    jal check_int_equals

    la a1, hex_str4
    li a2, 16
    jal parse_int
    mv a1, a0
    li a2, 0x00034532
    jal check_int_equals

    la a1, dec_str1
    li a2, 10
    jal parse_int
    mv a1, a0
    li a2, 12345983
    jal check_int_equals

    la a1, dec_str2
    li a2, 10
    jal parse_int
    mv a1, a0
    li a2, 00023420
    jal check_int_equals

    lw ra, 0(sp)
    addi sp, sp, 4
    jr ra
