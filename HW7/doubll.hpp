//
// ShanghaiTech University
//
// Computer Architecture I
//
// HW 7: C++ doubly linked list
//
// (c) Yanjie Song
//

#ifndef _HW7_DOUBLL_HPP_
#define _HW7_DOUBLL_HPP_

#include <cstddef> // For size_t

// Detailed implementation of doubll.
// Namespaces are used to prevent users from
// directly using them.
namespace __detail
{
  // The node in the list
  template <typename _Tp>
  class __List_node_base
  {
  public:
    // Default constructor (implement this)
    __List_node_base ();

    // Initialize the node with a value (implement this)
    explicit __List_node_base (const _Tp &value);

    // Copy constructor (compiler generated)
    __List_node_base (const __List_node_base &other) = default;

    // Destructor (compiler generated)
    ~__List_node_base () = default;

    // Copy assignment operator (compiler generated)
    __List_node_base &operator= (const __List_node_base &rhs) = default;

    // Data of the node
    _Tp _M_data;

    // Previous node
    __List_node_base *_M_prev;

    // Next node
    __List_node_base *_M_next;
  };

  // Iterator of the list (implement this)
  template <typename _Tp>
  class __List_iterator;

  // Const iterator of the list (implement this)
  template <typename _Tp>
  class __List_const_iterator;
}

// Doubly linked list class
template <typename _Tp>
class doubll
{
public:
  // Type renaming
  typedef _Tp value_type;
  typedef size_t size_type;
  typedef typename __detail::__List_iterator<_Tp> iterator;
  typedef typename __detail::__List_const_iterator<_Tp> const_iterator;

  // Default constructor
  doubll ();

  // Copy and move constructor are deleted
  doubll (const doubll& other) = delete;
  doubll (doubll&& other) = delete;

  // Create a list with SIZE copies of VALUE
  doubll (size_type size, const _Tp& value);

  // Destructor
  ~doubll ();

  // Copy and move assignment operator are deleted
  doubll& operator= (const doubll& other) = delete;
  doubll& operator= (doubll&& other) = delete;

  // The size of the list
  size_type size () const;

  // Iterator to the begin of the list
  // Refer to std::list for more information
  iterator begin ();

  // Iterator to the end of the list
  // Refer to std::list for more information
  iterator end ();

  // Const iterator to the begin of the list
  // Refer to std::list for more information
  const_iterator cbegin () const;

  // Const iterator to the end of the list
  // Refer to std::list for more information
  const_iterator cend () const;

  // Insert VALUE before POS. Return iterator to the inserted node
  iterator insert (iterator pos, const _Tp &value);
  iterator insert (const_iterator pos, const _Tp &value);

  // Erase node at POS. Returns iterator following the last removed element.
  iterator erase (iterator pos);
  iterator erase (const_iterator pos);

  // Reverses the order of the elements in the list.
  void reverse ();
private:
  __detail::__List_node_base<_Tp> _M_head;
  __detail::__List_node_base<_Tp> _M_tail;
  size_type _M_size;
};

#include "doubll-impl.hpp"

#endif // _HW7_DOUBLL_HPP_
