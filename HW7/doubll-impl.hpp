
///////////////////////////////////////////////////////////////////////////////
// The node in the list
template <typename _Tp>
__detail::__List_node_base<_Tp>::__List_node_base()
    : _M_data()
    , _M_prev(nullptr)
    , _M_next(nullptr)
{
    return;
}

// Initialize the node with a value (implement this)
template <typename _Tp>
__detail::__List_node_base<_Tp>::__List_node_base(const _Tp &value)
    : _M_data(value)
    , _M_prev(nullptr)
    , _M_next(nullptr)
{
    return;
}

///////////////////////////////////////////////////////////////////////////////

namespace __detail{

// Iterator of the list (implement this)
template <typename _Tp>
class __List_iterator{

private:
    // The pointer which records the current node
    __List_node_base<_Tp>*       _M_current;

    // Allow doubll to access the private members
    // Operations such as insertion and erase requires the access permission
    friend class doubll<_Tp>;
    // Allow const iterator to access the private members
    // const iterator has a constructor which adapts iterator to const iterator
    // requires the permission of access to internal node
    friend class __List_const_iterator<_Tp>;

  public:
    // Default constructor
    __List_iterator() : _M_current(nullptr) { return; }

    // Copy constructor
    __List_iterator(const __List_iterator<_Tp>& other)
        : _M_current(other._M_current)
    {
        return;
    }

    // Initialize a iterator with node
    __List_iterator(__List_node_base<_Tp>* node)
        : _M_current(node)
    {
        return;
    }

    // Destructor. Using default implementation
    ~__List_iterator() = default;

    // Copy operator
    __List_iterator& operator=(const __List_iterator<_Tp>& rhs)
    {
        // Copy stuff is simply about the internal node pointer
        _M_current = rhs._M_current;
        return (*this);
    }

    // Dereference operator
    _Tp& operator*()
    {
        // Simply return the data. Note that the value returned by arrow operator
        // is already of reference type.
        return _M_current->_M_data;
    }

    // Arrow operator
    _Tp* operator->()
    {
        // Return the address of data
        // However, arrow operator only implement the bottom layer of arraw access
        // in C++. Recall that, the arrow operator are usually used to access members
        // of some object's pointers.
        return &_M_current->_M_data;
    }

    // Comparison operator: Equality
    bool operator==(const __List_iterator<_Tp>& rhs) const
    {
        // Note that, here we compare the two iterator whether points to the same
        // node, instead of the internal data.
        return _M_current == rhs._M_current;
    }

    // Comparison operator: Inequality
    bool operator!=(const __List_iterator<_Tp>& rhs) const
    {
        // Refer to equality comparison for details.
        return _M_current != rhs._M_current;
    }

    // Prefix increment operator
    __List_iterator<_Tp>& operator++()
    {
        // Intuitively, the increment operation resembles accumulating index in an
        // array. But note that, prefix increment return object itself after the operation,
        // thus, you get the increased value.
        _M_current = _M_current->_M_next;
        return (*this);
    }

    // Postfix increment operator
    __List_iterator<_Tp> operator++(int)
    {
        // Intuitively, the increment operation resembles accumulating index in an
        // array. But note that, postfix increment return object itself before the operation,
        // thus, you get the original value. The implementation here calls the prefix increment
        // method after recording the old value, then return it back after increment. 
        __List_iterator<_Tp> ret(*this);
        operator++();
        return ret;
    }

    // Postfix decrement operator
    __List_iterator<_Tp>& operator--()
    {
        // Intuitively, the decrement operation resembles substracting index in an
        // array. But note that, prefix decrement return object itself after the operation,
        // thus, you get the decreased value.
        _M_current = _M_current->_M_prev;
        return (*this);
    }

    // Prefix increment operator
    __List_iterator<_Tp> operator--(int)
    {
        // Intuitively, the decrement operation resembles substracting index in an
        // array. But note that, postfix decrement return object itself before the operation,
        // thus, you get the original value. The implementation here calls the prefix decrement
        // method after recording the old value, then return it back after decrement.
        __List_iterator<_Tp> ret(*this);
        operator--();
        return ret;
    }

};

// Const iterator of the list (implement this)
template <typename _Tp>
class __List_const_iterator{

private:
    // The pointer which records the current node
    const __List_node_base<_Tp>*         _M_current;

    // Allow doubll to access the private members
    // Operations such as insertion and erase requires the access permission
    friend class doubll<_Tp>;

public:
    // Default constructor
    __List_const_iterator() : _M_current(nullptr) { return; }

    // Copy constructor
    __List_const_iterator(const __List_const_iterator<_Tp> &other)
        : _M_current(other._M_current)

    {
        return;
    }

    // Conversion from iterator
    __List_const_iterator(const __List_iterator<_Tp> &other)
        : _M_current(other._M_current)

    {
        return;
    }

    // Initialize a iterator with node
    __List_const_iterator(const __List_node_base<_Tp>* node)
        : _M_current(node)
    {
        return;
    }

    // Destructor. Using default implementation
    ~__List_const_iterator() = default;

    // Copy operator
    __List_const_iterator& operator=(const __List_const_iterator<_Tp>& rhs)
    {
        // Copy stuff is simply about the internal node pointer
        _M_current = rhs._M_current;
        return (*this);
    }

    // Dereference operator
    const _Tp& operator*()
    {
        // Simply return the data. Note that the value returned by arrow operator
        // is already of reference type.
        return _M_current->_M_data;
    }

    // Arrow operator
    const _Tp* operator->()
    {
        // Return the address of data
        // However, arrow operator only implement the bottom layer of arraw access
        // in C++. Recall that, the arrow operator are usually used to access members
        // of some object's pointers.
        return &_M_current->_M_data;
    }

    // Comparison operator: Equality
    bool operator==(const __List_const_iterator<_Tp>& rhs) const
    {
        // Note that, here we compare the two iterator whether points to the same
        // node, instead of the internal data.
        return _M_current == rhs._M_current;
    }

    // Comparison operator: Inequality
    bool operator!=(const __List_const_iterator<_Tp>& rhs) const
    {
        // Refer to equality comparison for details.
        return _M_current != rhs._M_current;
    }

    // Prefix increment operator
    __List_const_iterator<_Tp>& operator++()
    {
        // Intuitively, the increment operation resembles accumulating index in an
        // array. But note that, prefix increment return object itself after the operation,
        // thus, you get the increased value.
        _M_current = _M_current->_M_next;
        return (*this);
    }

    // Postfix increment operator
    __List_const_iterator<_Tp> operator++(int)
    {
        // Intuitively, the increment operation resembles accumulating index in an
        // array. But note that, postfix increment return object itself before the operation,
        // thus, you get the original value. The implementation here calls the prefix increment
        // method after recording the old value, then return it back after increment.
        __List_const_iterator<_Tp> ret(*this);
        operator++();
        return ret;
    }

    // Postfix decrement operator
    __List_const_iterator<_Tp>& operator--()
    {
        // Intuitively, the decrement operation resembles substracting index in an
        // array. But note that, prefix decrement return object itself after the operation,
        // thus, you get the decreased value.
        _M_current = _M_current->_M_prev;
        return (*this);
    }

    // Prefix increment operator
    __List_const_iterator<_Tp> operator--(int)
    {
        // Intuitively, the decrement operation resembles substracting index in an
        // array. But note that, postfix decrement return object itself before the operation,
        // thus, you get the original value. The implementation here calls the prefix decrement
        // method after recording the old value, then return it back after decrement.
        __List_const_iterator<_Tp> ret(*this);
        operator--();
        return ret;
    }

};

}

///////////////////////////////////////////////////////////////////////////////
// Doubly linked list class

// Default constructor
template <typename _Tp>
doubll<_Tp>::doubll()
{
    // Initialize head
    _M_head._M_prev = nullptr;
    _M_head._M_next = &_M_tail;

    // Initialize tail
    _M_tail._M_prev = &_M_head;
    _M_tail._M_next = nullptr;

    // Initialize size with 0
    _M_size = 0;
}

// Create a list with SIZE copies of VALUE
template <typename _Tp>
doubll<_Tp>::doubll(size_type size, const _Tp& value)
    : doubll<_Tp>()
{
    // Loop for size times while inserting value at the end
    for(doubll<_Tp>::size_type i = 0; i < size; i++){
        insert(end(), value);
    }
}

// Destructor
template <typename _Tp>
doubll<_Tp>::~doubll()
{
    // Removal starts from beginning
    doubll<_Tp>::iterator it = begin();
    // until the end. Note that, the head and tail node should not
    // be deleted by erase, they are living on the stack.
    while(it != end()){
        it = erase(it);
    }
    // Unset size
    _M_size = 0;
}

// The size of the list
template <typename _Tp>
typename doubll<_Tp>::size_type doubll<_Tp>::size() const
{
    // Return recorded size directly
    return _M_size;
}

// Iterator to the begin of the list
// Refer to std::list for more information
template <typename _Tp>
typename doubll<_Tp>::iterator doubll<_Tp>::begin()
{
    // Note that begin does not mean the head, it's the node right following the head
    return doubll<_Tp>::iterator(_M_head._M_next);
}

// Iterator to the end of the list
// Refer to std::list for more information
template <typename _Tp>
typename doubll<_Tp>::iterator doubll<_Tp>::end()
{
    // Note that end is exactly the end node. It cannot be any valid internal nodes, it
    // sometimes becomes the identifier of the end of list
    return doubll<_Tp>::iterator(&_M_tail);
}

// Const iterator to the begin of the list
// Refer to std::list for more information
template <typename _Tp>
typename doubll<_Tp>::const_iterator doubll<_Tp>::cbegin() const
{
    // Note that begin does not mean the head, it's the node right following the head
    // This the const version of begin()
    return doubll<_Tp>::const_iterator(_M_head._M_next);
}

// Const iterator to the end of the list
// Refer to std::list for more information
template <typename _Tp>
typename doubll<_Tp>::const_iterator doubll<_Tp>::cend() const
{
    // Note that end is exactly the end node. It cannot be any valid internal nodes, it
    // sometimes becomes the identifier of the end of list.
    // This is the const version of end()
    return doubll<_Tp>::const_iterator(&_M_tail);
}

// Insert VALUE before POS. Return iterator to the inserted node
template <typename _Tp>
typename doubll<_Tp>::iterator doubll<_Tp>::insert(doubll<_Tp>::iterator pos, const _Tp& value)
{
    // Here, notice that private member _M_current is available to doubll, because it
    // is declared as the friend class of iterator. It's esstensial to make iterator open
    // to doubll, a lot of methods involves the direct operations on the members of iterator
    __detail::__List_node_base<_Tp>* cur = pos._M_current;
    __detail::__List_node_base<_Tp>* prev = pos._M_current->_M_prev;
    // Create new node
    __detail::__List_node_base<_Tp>* newNode = new __detail::__List_node_base<_Tp>(value);
    // Connect the current node to new node
    cur->_M_prev = newNode;
    newNode->_M_next = cur;
    // Connect the previous node to new node
    prev->_M_next = newNode;
    newNode->_M_prev = prev;
    // Update the size
    _M_size++;
    // Return the iterator which points to inserted node
    return doubll<_Tp>::iterator(newNode);
}

template <typename _Tp>
typename doubll<_Tp>::iterator doubll<_Tp>::insert(doubll<_Tp>::const_iterator pos, const _Tp &value)
{
    // Construct the nonconst iterator by the following expression
    doubll<_Tp>::iterator p = doubll<_Tp>::iterator(const_cast<__detail::__List_node_base<_Tp> *>(pos._M_current));
    // Reuse nonconst version of insert()
    return insert(p, value);
}

// Erase node at POS. Returns iterator following the last removed element.
template <typename _Tp>
typename doubll<_Tp>::iterator doubll<_Tp>::erase(doubll<_Tp>::iterator pos)
{
    // Here, notice that private member _M_current is available to doubll, because it
    // is declared as the friend class of iterator. It's esstensial to make iterator open
    // to doubll, a lot of methods involves the direct operations on the members of iterator
    __detail::__List_node_base<_Tp>* cur = pos._M_current;
    __detail::__List_node_base<_Tp>* prev = pos._M_current->_M_prev;
    __detail::__List_node_base<_Tp>* next = pos._M_current->_M_next;
    // Disconnect with next node
    cur->_M_prev = cur->_M_next = nullptr;
    // Delete the node and release memory
    delete cur;
    // Make adjacent nodes connected
    prev->_M_next = next;
    next->_M_prev = prev;
    // Update size
    _M_size--;
    // Return the following node
    return doubll<_Tp>::iterator(next);
}

template <typename _Tp>
typename doubll<_Tp>::iterator doubll<_Tp>::erase(doubll<_Tp>::const_iterator pos)
{
    // Construct the nonconst iterator by the following expression
    doubll<_Tp>::iterator p = doubll<_Tp>::iterator(const_cast<__detail::__List_node_base<_Tp> *>(pos._M_current));
    // Reuse nonconst version of erase()
    return erase(p);
}

// Reverses the order of the elements in the list.
template <typename _Tp>
void doubll<_Tp>::reverse()
{
    // Start from the beginning
    doubll<_Tp>::iterator it = begin();
    // until the end
    while(it != end()){
        // Here, notice that private member _M_current is available to doubll, because it
        // is declared as the friend class of iterator. It's esstensial to make iterator open
        // to doubll, a lot of methods involves the direct operations on the members of iterator
        __detail::__List_node_base<_Tp>* cur = it._M_current;
        __detail::__List_node_base<_Tp>* prev = it._M_current->_M_prev;
        __detail::__List_node_base<_Tp>* next = it._M_current->_M_next;
        // Note that obtain the correct next node before moving the current node
        it++;

        // Extract the current node out by connect two adjacent nodes
        prev->_M_next = next;
        next->_M_prev = prev;
        // Insert item right after the head
        cur->_M_prev = &_M_head;
        cur->_M_next = _M_head._M_next;
        // Note that use _M_head._M_next before set to cur
        _M_head._M_next->_M_prev = cur;
        _M_head._M_next = cur;
    }
}

///////////////////////////////////////////////////////////////////////////////
