# CS 110 Spring 2019 hw3
# quick_sort.s

#========================================================================================
#                              Quick Sort RISC-V
#                                   README
#========================================================================================
# In this file you will be implementing the def_quick_sort and def_partition
# function, which form the most important part of this code.
#
# You should sort a 10-integer array. These arrays was reserved in static
# partition of memory. You can change to any 10 numbers you want to sort for
# testing and we will change them to our test case for grading.
#
# We give you the choice to test your code using predefined array: in line 41.
# You can decomment it and use array to test your code.
#
#
#                               IMPORTANT!!!
# Our asembler will support the following registers:
#
# zero, ra, sp, a0 - a1, a2-a4, t3-t6 and s2-s5
# The name x0 can be used in lieu of zero. Other register numbers
# (eg. x1, x2, etc.) are not supported.
#
# We will manually check whether you use registers properly after the ddl.
# So the grade on your autolab is not the final grade of your homework.
#======================================================================================

.data
    space:                  .asciiz  " "                # a space string.
    line:                   .asciiz "\n"                # a newline string.
    colonsp:                .asciiz ": "                # a colon string with space.
    .align                  2                           # make data aligned in word
    size:                   .word  10                   # the size of the array
    inputstring:            .asciiz "List need sort: "  # the origin array
    sorted_array_string:    .asciiz "Sorted:         "  # the output array

#=================================================     Reserved Array     ===========================================|
    # array:          .word 0 0 0 0 0 0 0 0 0 0           # array to be sorted                                        |
    array:           .word 678 567 456 765 876 987 543 654 684 374 # use this line if you want to test this array    |
#====================================================================================================================|

.text
.globl  main
main:
    j receive_values_end           # print the testing array

    receive_values_end:
        li    a0, 4                # 4 = print_string ecall.
        la    a1, inputstring
        ecall
        la    a1, array
        jal   print                # print user input values

    # Set up the main quick_sort call.
    # Arrays are
    la       a1, array             # a1 adrs of the array
    li       a2, 0                 # left val
    lw       a3, size              # right val
    addi     a3, a3, -1            # make a3 the higher index
    jal      def_quick_sort

    li       a0, 4                 # 4 = print_string ecall.
    la       a1, sorted_array_string
    ecall
    la       a1, array
    jal      print                  # print out the sorted list

    j        exit


########################################################
####################your code here######################
########################################################

# In this part you will implemente quick sort and partition seperately.
# You should learn how to use stack and function call before implemente.
# WARNING: using registers properly or you will get 30% deduction after ddl.
#      50% meaningful comments is needed or you will get 50% deduction after ddl.

########################################################
####################Function Convention#################
########################################################
# 1. Define word length/index is the actual index the same
# in the pseudocode.
# 2. Define byte length/index is the address offset in memory
# 3. def_swap_item receives the byte index.
# 4. def_partition receives the word index, and return word index.
# 5. def_quick_sort receives the word index.
########################################################

## Pseudocode ##
# algorithm swap(A, i, j) is
#   tmp = A[j]
#   A[j] = A[i]
#   A[i] = tmp
################
def_swap_item:
    # Swap two item of A
    # Parameter list
    # A: a1, i: a2, j: a3

    # locate A[i]
    add     a2, a1, a2

    # locate A[j]
    add     a3, a1, a3

    # t3 for tmp
    lw      t3, 0(a3)
    # t4 for A[i]
    lw      t4, 0(a2)

    # set A[j]
    sw      t4, 0(a3)
    # set A[j]
    sw      t3, 0(a2)

    # Return
    jr      ra


## Pseudocode ##
# algorithm quicksort(A, lo, hi) is
#     if lo < hi then
#         p := partition(A, lo, hi)
#         quicksort(A, lo, p - 1 )
#         quicksort(A, p + 1, hi)
################
def_quick_sort:
    # your code
    # Sort a range of A, from lo to hi
    # Parameter list
    # A: a1, lo: a2, hi: a3

    bge     a2, a3, empty_list
        ## Paritition
        # Reserve 4 words for a1, a2, a3, ra registers
        addi    sp, sp, -16
        sw      a1, 0(sp)   # Save a1
        sw      a2, 4(sp)   # Save a2
        sw      a3, 8(sp)   # Save a3
        sw      ra, 12(sp)  # Save ra

        # not need to setup parameters for partition
        # the parameters are shared
        jal     ra, def_partition

        # retrieve return value
        # t3 for p
        mv      t3, a1

        lw      a1, 0(sp)   # Load a1
        lw      a2, 4(sp)   # Load a2
        lw      a3, 8(sp)   # Load a3
        lw      ra, 12(sp)  # Load ra
        # restore stack
        addi    sp, sp, 16

        ## Sort left section
        # reserve 4 words for a1, a2, a3, ra registers
        addi    sp, sp, -20
        sw      t3, 0(sp)   # Save t3
        sw      a1, 4(sp)   # Save a1
        sw      a2, 8(sp)   # Save a2
        sw      a3, 12(sp)  # Save a3
        sw      ra, 16(sp)  # Save ra

        # setup parameters
        addi    a1, a1, 0
        addi    a2, a2, 0
        addi    a3, t3, -1

        # call sort recursively
        jal     ra, def_quick_sort

        lw      t3, 0(sp)   # Load t3
        lw      a1, 4(sp)   # Load a1
        lw      a2, 8(sp)   # Load a2
        lw      a3, 12(sp)  # Load a3
        lw      ra, 16(sp)  # Load ra
        # restore stack
        addi    sp, sp, 20

        ## Sort right section
        # reserve 4 words for a1, a2, a3, ra registers
        addi    sp, sp, -20
        sw      t3, 0(sp)   # Save t3
        sw      a1, 4(sp)   # Save a1
        sw      a2, 8(sp)   # Save a2
        sw      a3, 12(sp)  # Save a3
        sw      ra, 16(sp)  # Save ra

        # setup parameters
        addi    a1, a1, 0
        addi    a2, t3, 1
        addi    a3, a3, 0

        # call sort recursively
        jal     ra, def_quick_sort

        lw      t3, 0(sp)   # Load t3
        lw      a1, 4(sp)   # Load a1
        lw      a2, 8(sp)   # Load a2
        lw      a3, 12(sp)  # Load a3
        lw      ra, 16(sp)  # Load ra
        # restore stack
        addi    sp, sp, 20

    empty_list:
    jr  ra


## Pseudocode ##
# algorithm partition(A, lo, hi) is
#     pivot := A[hi]
#     i := lo - 1
#     for j := lo to hi - 1 do
#         if A[j] < pivot then
#             i := i + 1
#             swap A[i] with A[j]
#     swap A[i + 1] with A[hi]
#     return i + 1
################
def_partition:
    # your code

    # Parameter list
    # A: a1, lo: a2, hi: a3
    # Return value: a1

    # Multiply 4 to map to the byte offset
    slli    a2, a2, 2
    slli    a3, a3, 2

    # t3 for pivot
    add     t3, a1, a3
    lw      t3, 0(t3)
    # t4 for i
    addi    t4, a2, -4

    # Loop for swaping items
    begin_loop_partition:
    # a2 for j
    bge     a2, a3, end_loop
        # t5 for A[j]
        add     t5, a1, a2
        lw      t5, 0(t5)
        bge     t5, t3, larger_than_pivot
            # Accumulate i
            addi    t4, t4, 4

            # reserve 7 words for a1, a2, a3, t3, t4, t5, ra registers
            addi    sp, sp, -28
            sw      t3, 0(sp)   # Save t3
            sw      t4, 4(sp)   # Save t4
            sw      t5, 8(sp)   # Save t5
            sw      a1, 12(sp)  # Save a1
            sw      a2, 16(sp)  # Save a2
            sw      a3, 20(sp)  # Save a3
            sw      ra, 24(sp)  # Save ra

            # setup parameters for swap
            addi    a1, a1, 0
            # set a3 first, for a2 are involved, which is also to be changed
            addi    a3, a2, 0
            addi    a2, t4, 0

            jal     ra, def_swap_item

            lw      t3, 0(sp)   # Load t3
            lw      t4, 4(sp)   # Load t4
            lw      t5, 8(sp)   # Load t5
            lw      a1, 12(sp)  # Load a1
            lw      a2, 16(sp)  # Load a2
            lw      a3, 20(sp)  # Load a3
            lw      ra, 24(sp)  # Load ra
            # restore stack
            addi    sp, sp, 28
        larger_than_pivot:
    addi    a2, a2, 4
    jal     x0, begin_loop_partition
    end_loop:
    # reserve 7 words for a1, a2, a3, t3, t4, t5, ra registers
    addi    sp, sp, -28
    sw      t3, 0(sp)   # Save t3
    sw      t4, 4(sp)   # Save t4
    sw      t5, 8(sp)   # Save t5
    sw      a1, 12(sp)  # Save a1
    sw      a2, 16(sp)  # Save a2
    sw      a3, 20(sp)  # Save a3
    sw      ra, 24(sp)  # Save ra

    # setup parameters for swap
    addi    a1, a1, 0
    # set i+1
    addi    a2, t4, 4
    # set hi
    addi    a3, a3, 0

    jal     ra, def_swap_item

    lw      t3, 0(sp)   # Load t3
    lw      t4, 4(sp)   # Load t4
    lw      t5, 8(sp)   # Load t5
    lw      a1, 12(sp)  # Load a1
    lw      a2, 16(sp)  # Load a2
    lw      a3, 20(sp)  # Load a3
    lw      ra, 24(sp)  # Load ra
    # Restore stack
    addi    sp, sp, 28

    # Return pivot to a1
    # Note that, add 4 before to make sure a1 is positive
    # Because, right shift operation may not work on
    # negative integers
    addi    a1, t4, 4
    srli    a1, a1, 2
    jr      ra


# programs ends
#
exit:
    # your code
    # exit command 10
    li      a0, 10
    ecall                     # system call


###       Printing array
print:
    print_loop_prep:
        mv      t3, a1
        lw      t4, size
        li      t5, 0
    print_loop:
        # your code
        ## Pseudocode ##
        # for i := 0 to size-1:
        #     print A[i]
        #     print " "
        ################

        beq     t5, t4, print_end

        # t6 for A[i]
        slli    t6, t5, 2
        add     t6, t3, t6
        lw      t6, 0(t6)

        # enviroment call for print integer
        li      a0, 1
        mv      a1, t6
        ecall

        # print space
        # enviroment call for print string
        li      a0, 4
        la      a1, space
        ecall

        # accumulate i
        addi    t5, t5, 1

        jal     x0, print_loop
    print_end:
        li      a0, 4
        la      a1, line
        ecall
        jr      ra

########################################################
####################End of your code####################
########################################################
