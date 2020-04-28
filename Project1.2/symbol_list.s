# ShanghaiTech CS110 2019 Project 1.2

#==============================================================================
#                              Project 1-2 Part 2
#                               SymbolList README
#==============================================================================
# In this file you will implement a linked list-based data structure for
# storing symbols. Its purpose is analogous to SymbolTable from Proj1-1, but to
# distinguish it we will call it SymbolList.
#
# Each node in the SymbolList contains a (addr, name) pair. An empty SymbolList
# is simply a pointer to NULL. As (addr, name) pairs are added to a SymbolList,
# new list nodes are dynamically allocated and appended to the front of the list. 
# However, as there is no free() in Venus, you do not need to write a free_list()
# function. You may use the functions in string.s during your implementation. 
# What you should do is paste specified function into corresponding position.
#
# You do not need to perform error checking on duplicate addr or name entries
# in SymbolList, nor do you need to do alignment checking. Don't worry about
# provided addresses being too big.
#
# If SymbolList were to be written in C, each list node would be declared as 
# follows:
#   typedef struct symbollist { 
#       int addr;
#       char* name;
#       struct symbollist* next; 
#   } SymbolList;
#
# You need to write new_node(), symbol_for_addr(), addr_for_symbol() and add_to_list().
# You will find test cases in this file. If you want to change test partition, 
# make sure you have totally understood the test case before you changing them.
#==============================================================================

.data
#-------------------------------------------------------
# Test Data - Feel free to add your own after this line
#-------------------------------------------------------
# Test strings
test_label1:    .asciiz "Label 1"
test_label2:    .asciiz "Label 2"
test_label3:    .asciiz "Label 3"

# Test list
node1:      .word 1234 test_label1 0
node2:      .word 3456 test_label2 node1
node3:      .word 5678 test_label3 node2
#-------------------------------------------------------
# Test Data - Feel free to add your own above this line
#-------------------------------------------------------

test_header_name:   
        .asciiz "Running symbol list tests:\n"
test_symbol_for_addr_name:
        .asciiz "Testing symbol_for_addr():\n"
test_addr_for_symbol_name:
        .asciiz "Testing addr_for_symbol():\n"
test_add_to_list_name:  
        .asciiz "Testing add_to_list():\n"

newline:   .asciiz "\n"
tab:       .asciiz "\t"

tc_actual_str:      .asciiz "actual:   "
tc_expected_str:    .asciiz "expected: "
tc_testpass_str:    .asciiz " ...test passes\n"
tc_testfail_str:    .asciiz " ...test FAILS\n"

.globl main
.text
#------------------------------------------------------------------------------
# function new_node()
#------------------------------------------------------------------------------
# Creates a new uninitialized SymbolList node on heap.
# Arguments: none
# Returns: pointer to a SymbolList node 
# CLOBBERS: a1, a0, ra
#------------------------------------------------------------------------------
new_node:   
    ### YOUR CODE ###
    li a1, 12          # one node is 12 bytes
    li a0, 9		   # for sbrk, a0 is 9
    ecall         	   # ecall
    jr ra              # return, the new node pointer is a0
#------------------------------------------------------------------------------
# function symbol_for_addr()
#------------------------------------------------------------------------------
#  Iterates through the SymbolList and searches for an entry with the given addr.
#  If an entry is found, return a pointer to the name. Otherwise return NULL.
#
#  Arguments:
#   a1 = addr to look for
#   a2 = pointer to a SymbolList (NULL indicates empty list)
#
#  Returns: a pointer to the name if found or NULL if not found
# CLOBBERS: sp, a0, a1, a2, t0, t1, ra
#------------------------------------------------------------------------------
symbol_for_addr:
    ### YOUR CODE ###
    addi sp, sp, -12     # for stack pointer
    sw ra, 0(sp)         # save ra
    sw a1, 4(sp)         # save addr to search
    sw a2, 8(sp)         # save pointer of SymbolList
    lw t0, 8(sp)         # use t0 to iterate

sym_symbol_loop:
    beq t0, zero, sym_symbol_wrongreturn  # if t0 is zero, go to wrongreturn fuction
    lw t1, 0(t0)        # t1 stores the addr of current node
    beq t1, a1, sym_symbol_rightreturn	   # compare t1 with addr to search. if right, go to rightreturn fuction
    lw t0, 8(t0)		# t0 is the addr of next node
    j sym_symbol_loop  # iterate

# if addr is found
sym_symbol_rightreturn:
	lw a0, 4(t0)        # the addr of name
	lw ra, 0(sp)		# load ra
	addi sp, sp, 12 	# for stack pointer
	jr ra               # return, the pointer is a0

# if addr is not found or empty list
sym_symbol_wrongreturn:
	lw ra, 0(sp)		# load ra
	addi sp, sp, 12		# for stack pointer
	li a0, 0			# null if note found
	jr ra 				# return, the pointer is a0

#------------------------------------------------------------------------------
# function addr_for_symbol()
#------------------------------------------------------------------------------
# Iterates through the SymbolList and searches for an entry with the given name.
# If an entry is found, return that addr. Otherwise return -1.
#
# Arguments:
#  a1 = pointer to a SymbolList (NULL indicates empty list)
#  a2 = name to look for
#
# Returns:  addr (int) of symbol if found or -1 if not found
# CLOBBERS: sp, a1, a2, a0, s2, ra
#------------------------------------------------------------------------------
addr_for_symbol:
    ### YOUR CODE ###
    addi sp, sp -16   # for stack pointer
    sw ra, 0(sp)	  # save ra
    sw a1, 4(sp)	  # save pointer of symbollist
    sw a2, 8(sp)	  # save the name to look for
    sw s2, 12(sp)	  # save s2
    lw s2, 4(sp)	  # use s2 to iterate

sym_addr_loop:
	beq s2, zero, sym_addr_wrongreturn # if not found, go to wrongreturn function
	lw a1, 4(s2)      # a1 is node's name
	lw a2, 8(sp)      # a2 is name to look for
	jal streq         # compare two name
	beq a0, zero, sym_addr_rightreturn	 # if equal, go to rightreturn
	lw s2, 8(s2)      # s2 is addr of next node
	j sym_addr_loop # iterate

# if found, return addr of symbol
sym_addr_rightreturn:
	lw ra, 0(sp)      # load ra
	lw a0, 0(s2)      # a0 = addr of symbol
	lw s2, 12(sp)     # load s2
	addi sp, sp, 16   # for stack pointer
	jr ra             # return, return message is a0
	
# if not found, return -1
sym_addr_wrongreturn:
	lw ra, 0(sp)      # load ra
	lw s2, 12(sp)     # load s2
	li a0, -1		  # -1 if not found
	addi sp, sp, 16   # for stack pointer
	jr ra  			  # return, return message is a0
#------------------------------------------------------------------------------
# function add_to_list()
#------------------------------------------------------------------------------
# Adds a (name, addr) pair to the FRONT of the list. You should call new_node()
# to create a new node. You do not need to perform error checking on duplicate 
# addr or name entries in SymbolList, nor do you need to do alignment checking.
#
# As with Project 1-1, you WILL need to create a copy of the string that was
# passed in. copy_of_str() from Part 1 will be useful. After new entry has been
# added to the list, return the new list pointer.
#
# Arguments:
#   a1 = ptr to list (may be NULL)
#   a2 = address of symbol (integer)
#   a3 = pointer to name of symbol (string)
#
# Returns: the new list pointer
# CLOBBERS: sp, a0, a1, a2, t0, t1, t2, t3, ra
#------------------------------------------------------------------------------
add_to_list:    
    ### YOUR CODE ###
    addi sp, sp, -16			# for stack pointer
    sw ra, 0(sp)				# save ra
    sw a1, 4(sp)				# save ptr of list
    sw a2, 8(sp)				# save addr of symbol
    sw a3, 12(sp)				# save ptr to name of symbol

    mv a1, a3	     			# a1 is string to copy
    jal copy_of_str             # copy the string
    mv t0, a0					# t0 is the pointer to copy name
    jal new_node				# create a new node
    mv t1, a0					# save the new node pointer in t1

    lw t2, 8(sp)                # t2 is addr of symbol
    sw t2, 0(t1)				# save in new node 
    sw t0, 4(t1)                # save ptr to new node name copy
    lw t3, 4(sp) 				# new node's next is list head
    sw t3, 8(t1)				# save next addr

    mv a0, t1                   # a0 is new list pointer
    lw ra, 0(sp)				# load ra
    addi sp, sp, 16				# for stack pointer
    jr ra                       # return

###############################################################################
#                 DO NOT MODIFY ANYTHING BELOW THIS POINT                       
###############################################################################
#------------------------------------------------------------------------------
# function print_list() - DO NOT MODIFY THIS FUNCTION
#------------------------------------------------------------------------------
# Arguments:
#  a1 = pointer to a SymbolList (NULL indicates empty list)
#  a2 = print function
#------------------------------------------------------------------------------
print_list:
    addi sp, sp, -12        # Begin print_list
    sw s2, 8(sp)
    sw s1, 4(sp)
    sw ra, 0(sp)
    mv s1, a1
    mv s2, a2
print_list_loop:
    beq s1, zero, print_list_end
    lw a1, 0(s1)
    lw a2, 4(s1)
    jalr s2
    lw s1, 8(s1)
    j print_list_loop
print_list_end:
    lw s2, 8(sp)
    lw s1, 4(sp)
    lw ra, 0(sp)
    addi sp, sp, 12
    jr ra           # End print_list    

#------------------------------------------------------------------------------
# function print_symbol() - DO NOT MODIFY THIS FUNCTION
#------------------------------------------------------------------------------
# Prints one symbol to standard output.
#
# Arguments:
#  a1 = addr of symbol
#  a2 = name of symbol
#
# Returns: none
#------------------------------------------------------------------------------
print_symbol:
    li a0, 1           # Begin print_symbol()
    ecall
    la a1, tab
    li a0, 4
    ecall
    mv a1, a2
    ecall
    la a1, newline
    ecall
    jr ra           # End print_symbol()

###############################################################################
#                 DO NOT MODIFY ANYTHING ABOVE THIS POINT                       
###############################################################################


#------------------------------------------------------------------------------
# function strlen() - paste your implementation here
#------------------------------------------------------------------------------
# Arguments:
#  a1 = string input
#
# Returns: the length of the string
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

    la a1, node3
    jal test_symbol_for_addr
    jal print_newline

    la a1, node3
    jal test_addr_for_symbol
    jal print_newline
    
    jal test_add_to_list
    
    li a0, 10
    ecall

#-------------------------------------------
# Tests symbol_for_addr()
# a1 = address of beginning of list
#-------------------------------------------
test_symbol_for_addr:
    addi sp, sp, -8
    sw ra, 0(sp)
    sw a1, 4(sp)
    la a1, test_symbol_for_addr_name
    jal print_str

    li a1, 1234
    lw a2, 4(sp)
    jal symbol_for_addr
    mv a1, a0
    la a2, test_label1
    jal check_str_equals

    lw ra, 0(sp)
    addi sp, sp, 8
    jr ra

#-------------------------------------------
# Tests addr_for_symbol()
# a1 = address of beginning of list
#-------------------------------------------
test_addr_for_symbol:
    addi sp, sp, -12
    sw s1, 8(sp)
    sw a1, 4(sp)
    sw ra, 0(sp)
    la a1, test_addr_for_symbol_name
    jal print_str

    lw a1, 4(sp)
    la a2, test_label1
    jal addr_for_symbol
    mv a1, a0
    li a2, 1234
    jal check_int_equals

    lw ra, 0(sp)
    lw s1, 8(sp)
    addi sp, sp, 12
    jr ra

#-------------------------------------------
# Tests add_to_list()
# a1 = address of beginning of list
#
# Because the print is more complex than
# previous tests, the auto comparison haven't
# been implemented. You should check it yourself.
#-------------------------------------------
test_add_to_list:               # Begin test_add_to_list
    addi sp, sp, -4
    sw ra, 0(sp)
    
    la a1, test_add_to_list_name
    jal print_str
    li a1, 0            
    li a2, 1234
    la a3, test_label1
    jal add_to_list         # Test label 1
    mv a1, a0
    li a2, 3456
    la a3, test_label2
    jal add_to_list         # Test label 2
    mv a1, a0
    li a2, 5678
    la a3, test_label3          
    jal add_to_list         # Test label 3
    
    mv a1, a0               # print your list
    la a2, print_symbol
    jal print_list

    jal print_newline

    la a1, node3            # print expected list
    la a2, print_symbol
    jal print_list

    lw ra, 0(sp)
    addi sp, sp, 4
    jr ra
