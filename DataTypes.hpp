/***********************************************************
*						DataTypes.h
* Author: Leo Carroll
* Overview:
*	A header-only custom data type library. It comes with
*	implementations of vectors, arrays, linked lists, and
*	hashmaps.
* Created: 2026-03-02
* Last Modified: 2026-03-03
* Notes:
*	You cannot put templated functions in a .cpp file, which
*	is why it is header-only.
*
*	This specific version was written from scratch on the
*	second of March, 2026. I applied lots of previous
*	knowledge and strategies that I learned from previously
*	implementing a worse version of this program.
************************************************************/

#pragma region Headers And Helpers

#ifndef DATATYPES_H
#define DATATYPES_H

#include <stdexcept>			// Included for std::runtime_error and std::out_of_range.
#include <cassert>				// Included to quickly get rid of annoying Intellisense warnings.
#include <initializer_list>		// Included for the ability to initialize containers using brace-enclosed lists.
#include <utility>				// Included for some helper functions.
#include <type_traits>
#include <tuple>
#include <cstddef>

// next_power_of_two Function
// A function to get the next power of two of a number.
static size_t next_power_of_two(size_t num) {
	size_t result = 1;		// Start the result at one.
	// Double the number until it fits the required number.
	while (result < num) {
		result *= 2;
	}
	return result;
}

// swap Function
// A function to swap the values of two variables.
template<typename T>
static void swap(T& a, T& b) {
	T temp = std::move(a);		// Create a temporary variable to hold a's value.
	a = std::move(b);			// Overwrite the value of a with the moved value of b
	b = std::move(temp);		// Overwrite the value of b with the moved value of the temp.
}

#pragma endregion

// Create a region. These regions allows for me to hide entire sections of code when I don't need to see them.
#pragma region Vector

#pragma region Class Definition
/***********************************************************
*						Vector Class
* Description:
*	A custom vector class that uses features like placement
*	new and pointers as iterators.
************************************************************/
template<typename T>
class Vector {
private:
	T* m_Data;				// A pointer to the dynamically allocated memory.
	size_t m_Size;			// The current number of elements in the vector.
	size_t m_Capacity;		// The total capcity of the vector.

public:
	Vector();

	Vector(size_t capacity);

	Vector(const std::initializer_list<T>& list);

	Vector(const Vector& other);

	Vector(Vector&& other) noexcept;

	Vector& operator=(const Vector& other);

	Vector& operator=(Vector&& other) noexcept;

	~Vector();

	T& operator[](size_t idx);

	const T& operator[](size_t idx) const;

	T& at(size_t idx);

	const T& at(size_t idx) const;

	void push_back(const T& value);

	void push_back(T&& value);

	template<typename... Args>
	void emplace_back(Args&&... args);

	void push_front(const T& value);

	void push_front(T&& value);

	template<typename... Args>
	void emplace_front(Args&&... args);

	void insert(size_t idx, const T& value);

	void insert(size_t idx, T&& value);

	template<typename... Args>
	void emplace(size_t idx, Args&&... args);

	void pop_back();

	void pop_front();

	void erase(size_t idx);

	T& back();

	const T& back() const;

	T& front();

	const T& front() const;

	T* begin() noexcept;

	const T* begin() const noexcept;

	const T* cbegin() const noexcept;

	T* end() noexcept;

	const T* end() const noexcept;

	const T* cend() const noexcept;

	void clear();

	bool empty() const;

	size_t size() const;

	size_t capacity() const;

	void resize(size_t newSize);

	T* data() noexcept;

	const T* data() const noexcept;

	void shrink_to_fit();

	bool operator==(const Vector& other) const;

	bool operator!=(const Vector& other) const;

private:
	void grow();

	void reallocate(size_t newCapacity);

	T* copy_data() const;
};

#pragma endregion

#pragma region Function Definitions
// Default constructor
// Initializes variables to safe default values (nullptr, 0, 0).
template<typename T>
Vector<T>::Vector() {
	// Initialize member variables to default values.
	m_Data = nullptr;
	m_Size = 0;
	m_Capacity = 0;
}

// Capacity constructor
// Initializes the vector with a certain capacity.
template<typename T>
Vector<T>::Vector(size_t capacity) {
	m_Data = static_cast<T*>(::operator new(sizeof(T) * capacity));
	m_Size = 0;
	m_Capacity = capacity;
}

// Initializer List constructor
// Allows for the vector to be initialized with a brace-enclosed list.
template<typename T>
Vector<T>::Vector(const std::initializer_list<T>& list) {
	m_Size = 0;
	m_Capacity = next_power_of_two(list.size());
	m_Data = static_cast<T*>(::operator new(sizeof(T) * m_Capacity));
	for (const auto& v : list) {
		new (&m_Data[m_Size++]) T(v);
	}
}

// Copy constructor
// Allows for a vector to be copied in the constructor to another.
template<typename T>
Vector<T>::Vector(const Vector& other) {
	m_Capacity = other.m_Capacity;
	m_Size = other.m_Size;
	m_Data = static_cast<T*>(::operator new(sizeof(T) * m_Capacity));

	for (size_t i = 0; i < m_Size; ++i) {
		assert(i < m_Capacity);
		new (&m_Data[i]) T(other.m_Data[i]);
	}
}

// Move constructor
// Allows for one vector to be moved to another using std::move().
// It is marked noexcept as is standard for move constructors and equal operators.
template<typename T>
Vector<T>::Vector(Vector&& other) noexcept {
	m_Data = std::exchange(other.m_Data, nullptr);
	m_Capacity = std::exchange(other.m_Capacity, 0);
	m_Size = std::exchange(other.m_Size, 0);
}

// Copy = operator
// Allows for a vector to be copied to another using the = operator.
template<typename T>
Vector<T>& Vector<T>::operator=(const Vector& other) {
	if (this == &other) return *this;

	Vector temp(other);
	T* tempData = copy_data();
	m_Data = temp.m_Data;
	temp.m_Data = tempData;

	swap(m_Size, temp.m_Size);
	swap(m_Capacity, temp.m_Capacity);

	return *this;
}

// Move = operator
// Allows for a vector to be moved to another using the = operator.
// It is marked noexcept as is standard for move constructors and equal operators.
template<typename T>
Vector<T>& Vector<T>::operator=(Vector&& other) noexcept {
	if (this == &other) return *this;

	for (size_t i = 0; i < m_Size; ++i) {
		m_Data[i].~T();
	}
	::operator delete(m_Data);

	m_Data = std::exchange(other.m_Data, nullptr);
	m_Capacity = std::exchange(other.m_Capacity, 0);
	m_Size = std::exchange(other.m_Size, 0);

	return *this;
}

// Destructor
// Frees the memory taken by the vector.
template<typename T>
Vector<T>::~Vector() {
	for (size_t i = 0; i < m_Size; ++i) {
		m_Data[i].~T();
	}
	::operator delete(m_Data);
}

// non-const [] operator
// Non-bounds checked index operator. Accesses the data pointer at a certain index.
// Note: It can easily go out of range and access out-of-bounds data.
template<typename T>
T& Vector<T>::operator[](size_t idx) {
	return m_Data[idx];
}

// const [] operator
// Non-bounds checked constant index operator. Accesses the data pointer at a certain index.
// Note: It can easily go out of range and access out-of-bounds data, but cannot alter it.
template<typename T>
const T& Vector<T>::operator[](size_t idx) const {
	return m_Data[idx];
}

// non-const at function
// Bounds checked at function. Accesses the data pointer at a certain index.
template<typename T>
T& Vector<T>::at(size_t idx) {
	// Ensure that the given index is less than the number of elements.
	if (idx >= m_Size) {
		throw std::out_of_range("Index out of range");
	}
	return m_Data[idx];
}

// const at function
// Bounds checked at function. Accesses the data pointer at a certain index.
template<typename T>
const T& Vector<T>::at(size_t idx) const {
	// Ensure that the given index is less than the number of elements.
	if (idx >= m_Size) {
		throw std::out_of_range("Index out of range");
	}
	return m_Data[idx];
}

// Copy push_back function
// Pushes an element to the back of the vector. It is pretty much just an alias for emplace_back.
template<typename T>
void Vector<T>::push_back(const T& value) {
	emplace_back(value);
}

// Move push_back function
// Pushes a moved element to the back of the vector. It is pretty much just an alias for emplace_back.
template<typename T>
void Vector<T>::push_back(T&& value) {
	emplace_back(std::move(value));
}

// emplace_back function
// In-place constructs a new element with the given arguments
// Allows for just the arguments to be passed instead of a created object.
template<typename T>
template<typename... Args>
void Vector<T>::emplace_back(Args&&... args) {
	if (m_Size >= m_Capacity) {
		grow();		// Double the capacity of the vector and reallocate memory.
	}
	// Use placement new and std::forward to construct in place and move the constructed object into the vector.
	new (&m_Data[m_Size]) T(std::forward<Args>(args)...);
	++m_Size;
}

// Copy push_front function
// Pushes an element to the front of the vector.
template<typename T>
void Vector<T>::push_front(const T& value) {
	emplace_front(value);
}

// Move push_front function
// Push a moved value to the front of the vector.
template<typename T>
void Vector<T>::push_front(T&& value) {
	emplace_front(std::move(value));
}

// emplace_front function
// Takes the arguments to construct an in-place object and push it to the front.
template<typename T>
template<typename... Args>
void Vector<T>::emplace_front(Args&&... args) {
	if (m_Size >= m_Capacity) {
		grow();
	}
	for (size_t i = m_Size; i > 0; --i) {
		new (&m_Data[i]) T(std::move(m_Data[i - 1]));
		m_Data[i - 1].~T();
	}
	new (&m_Data[0]) T(std::forward<Args>(args)...);
	++m_Size;
}

// Copy insert function
// Inserts a copied element at the given index.
template<typename T>
void Vector<T>::insert(size_t idx, const T& value) {
	emplace(idx, value);
}

// Move insert function
// Inserts a moved element at the given index.
template<typename T>
void Vector<T>::insert(size_t idx, T&& value) {
	emplace(idx, std::move(value));
}

// emplace function
// Emplaces a constructed in-place element at the given index.
template<typename T>
template<typename... Args>
void Vector<T>::emplace(size_t idx, Args&&... args) {
	// Ensure that the index is within range
	if (idx > m_Size) {
		throw std::out_of_range("Index out of range.");
	}

	if (m_Size >= m_Capacity) {
		grow();
	}

	// Iterate down to the index, moving each element to the right by one.
	for (size_t i = m_Size; i > idx; --i) {
		new (&m_Data[i]) T(std::move(m_Data[i - 1]));
		m_Data[i - 1].~T();
	}

	// Use placement new to move the in-place constructed object into the object.
	new (&m_Data[idx]) T(std::forward<Args>(args)...);
	++m_Size;
}

// pop_back function
// Removes the element at the back of the function.
template<typename T>
void Vector<T>::pop_back() {
	if (m_Size == 0) {
		throw std::runtime_error("pop_back called on empty vector.");
	}

	m_Data[--m_Size].~T();
}

// pop_front function
// Removes the element at the front of the function.
template<typename T>
void Vector<T>::pop_front() {
	if (m_Size == 0) {
		throw std::runtime_error("pop_front called on empty vector.");
	}
	m_Data[0].~T();
	// Iterate over the data, shifting each element to the left by one.
	for (size_t i = 0; i < m_Size - 1; ++i) {
		// Shift the data one to the left.
		new (&m_Data[i]) T(std::move(m_Data[i + 1]));
		m_Data[i + 1].~T();		// Destruct the moved element to prevent memory leaks.
	}
	--m_Size;
}

// erase function
// Erases the element at an index.
template<typename T>
void Vector<T>::erase(size_t idx) {
	if (idx >= m_Size) {
		throw std::runtime_error("Index given to erase is out of bounds.");
	}
	for (size_t i = idx; i < m_Size - 1; ++i) {
		m_Data[i] = std::move(m_Data[i + 1]);
	}
	m_Data[--m_Size].~T();
}

// non-const back function
// Returns a reference to the element at the back of the vector.
// Throws an exception if the vector is empty.
template<typename T>
T& Vector<T>::back() {
	if (m_Data == nullptr) {
		throw std::runtime_error("back called on empty vector.");
	}
	return m_Data[m_Size - 1];
}

// const back function
// Returns a constant reference to the element at the back of the vector.
// Throws an exception if the vector is empty.
template<typename T>
const T& Vector<T>::back() const {
	if (m_Data == nullptr) {
		throw std::runtime_error("back called on empty vector.");
	}
	return m_Data[m_Size - 1];
}

// non-const front function
// Returns a reference to the element at the front of the vector.
// Throws an exception if the vector is empty.
template<typename T>
T& Vector<T>::front() {
	if (m_Data == nullptr) {
		throw std::runtime_error("back called on empty vector.");
	}
	return m_Data[0];
}

// const front function
// Returns a const reference to the element at the front of the vector.
// Throws an exception if the vector is empty.
template<typename T>
const T& Vector<T>::front() const {
	if (m_Data == nullptr) {
		throw std::runtime_error("back called on empty vector.");
	}
	return m_Data[0];
}

// non-const begin function
// Returns a pointer to the element at the front of the vector.
// This enables range-based for loops.
template<typename T>
T* Vector<T>::begin() noexcept {
	return m_Data;
}

// const begin function
// Returns a constant pointer to the element at the front of the vector.
// This enables range-based for loops.
template<typename T>
const T* Vector<T>::begin() const noexcept {
	return m_Data;
}

// const cbegin function
// Returns a constant pointer to the element at the front of the vector.
// This enables range-based for loops.
template<typename T>
const T* Vector<T>::cbegin() const noexcept {
	return m_Data;
}

// non-const end function
// Returns a pointer to the element at the end of the vector.
// This acts as the upper range of a range-based for loop.
template<typename T>
T* Vector<T>::end() noexcept {
	return m_Data + m_Size;
}

// const end function
// Returns a constant pointer to the element at the end of the vector.
// This acts as the upper range of a range-based for loop.
template<typename T>
const T* Vector<T>::end() const noexcept {
	return m_Data + m_Size;
}

// const cend function
// Returns a constant pointer to the element at the end of the vector.
// This acts as the upper range of a range-based for loop.
template<typename T>
const T* Vector<T>::cend() const noexcept {
	return m_Data + m_Size;
}

// clear function
// Removes every element in the vector.
template<typename T>
void Vector<T>::clear() {
	for (size_t i = 0; i < m_Size; ++i) {
		m_Data[i].~T();
	}
	m_Size = 0;
}

// empty function
// Returns true if the vector is empty.
template<typename T>
bool Vector<T>::empty() const {
	return m_Size == 0;
}

// size function
// Returns the current number of elements in the vector.
template<typename T>
size_t Vector<T>::size() const {
	return m_Size;
}

// capacity function
// Returns the total capacity of the vector.
template<typename T>
size_t Vector<T>::capacity() const {
	return m_Capacity;
}

// resize function
// Increases the vector's capacity to the next power of two of the next newSize.
template<typename T>
void Vector<T>::resize(size_t newSize) {
	if (newSize > m_Capacity) {
		size_t newCapacity = next_power_of_two(static_cast<int>(newSize));
		reallocate(newCapacity);
	}

	if (newSize > m_Size) {
		for (size_t i = m_Size; i < newSize; ++i) {
			new (&m_Data[i]) T();
		}
	}
	else {
		for (size_t i = newSize; i < m_Size; ++i) {
			m_Data[i].~T();
		}
	}

	m_Size = newSize;
}

// non-const data function
// Returns the data pointer.
template<typename T>
T* Vector<T>::data() noexcept {
	return m_Data;
}

// const data function
// Returns a const pointer to the data pointer.
template<typename T>
const T* Vector<T>::data() const noexcept {
	return m_Data;
}

// shrink_to_fit function
// Shrinks the vector to fit the number of elements in the vector.
template<typename T>
void Vector<T>::shrink_to_fit() {
	size_t newCapacity = next_power_of_two(m_Size);
	T* copy = static_cast<T*>(::operator new(sizeof(T) * newCapacity));
	for (size_t i = 0; i < m_Size; ++i) {
		new (&copy[i]) T(std::move(m_Data[i]));
		m_Data[i].~T();
	}
	::operator delete(m_Data);
	m_Data = copy;
	m_Capacity = newCapacity;
}

// == operator
// Checks if one vector is equal to another.
template<typename T>
bool Vector<T>::operator==(const Vector& other) const {
	if (m_Size != other.m_Size) {
		return false;
	}
	for (size_t i = 0; i < m_Size; ++i) {
		if (m_Data[i] != other.m_Data[i]) {
			return false;
		}
	}
	return true;
}

// != operator
// Checks if one vector is not equal to another.
template<typename T>
bool Vector<T>::operator!=(const Vector& other) const {
	return !(*this == other);
}

// grow function
// Double the capacity and reallocate the memory.
template<typename T>
void Vector<T>::grow() {
	size_t newCapacity = (m_Capacity == 0) ? 1 : m_Capacity * 2;
	reallocate(newCapacity);
}

// reallocate function
// Reallocates the memory to fit the new capacity.
template<typename T>
void Vector<T>::reallocate(size_t newCapacity) {
	if (newCapacity > m_Capacity) {
		T* copy = static_cast<T*>(::operator new(sizeof(T) * newCapacity));
		for (size_t i = 0; i < m_Size; ++i) {
			new (&copy[i]) T(std::move(m_Data[i]));
			m_Data[i].~T();
		}
		::operator delete(m_Data);
		m_Data = copy;
		m_Capacity = newCapacity;
	}
}

template<typename T>
T* Vector<T>::copy_data() const {
	T* copy = static_cast<T*>(::operator new(sizeof(T) * m_Capacity));
	for (size_t i = 0; i < m_Size; ++i) {
		assert(i < m_Capacity);
		new (&copy[i]) T(m_Data[i]);
	}
	return copy;
}

#pragma endregion

#pragma endregion

#pragma region LinkedList

template<typename T>
class LinkedList {
private:
	struct Node {
		T m_Value;
		Node* m_Prev;
		Node* m_Next;

		Node(T value, Node* prev, Node* next)
			: m_Value(std::move(value)), m_Prev(prev), m_Next(next) {
			if (prev) prev->m_Next = this;
			if (next) next->m_Prev = this;
		}

		void link_nodes(Node* prev, Node* next) {
			m_Prev = prev;
			if (prev) {
				prev->m_Next = this;
			}
			m_Next = next;
			if (next) {
				next->m_Prev = this;
			}
		}
	};

	Node* m_Head;
	Node* m_Tail;

public:
	template<bool IsConst>
	class generic_iterator {
	private:
		using NodePtr = std::conditional_t<IsConst, const Node*, Node*>;
		NodePtr m_Node;

	public:
		using iterator_category = std::bidirectional_iterator_tag;
		using value_t = T;
		using difference_type = std::ptrdiff_t;
		using pointer = std::conditional_t<IsConst, const T*, T*>;
		using reference = std::conditional_t<IsConst, const T&, T&>;

		explicit generic_iterator(NodePtr node) {
			m_Node = node;
		}

		reference operator*() const {
			return m_Node->m_Value;
		}

		pointer operator->() const {
			return &(m_Node->m_Value);
		}

		generic_iterator& operator++() {
			m_Node = m_Node->m_Next;
			return *this;
		}

		generic_iterator operator++(int) {
			generic_iterator temp = *this;
			m_Node = m_Node->m_Next;
			return temp;
		}

		generic_iterator& operator--() {
			m_Node = m_Node->m_Prev;
			return *this;
		}

		generic_iterator operator--(int) {
			generic_iterator temp = *this;
			m_Node = m_Node->m_Prev;
			return temp;
		}

		template<bool O>
		bool operator==(const generic_iterator<O>& other) const {
			return m_Node == other.m_Node;
		}

		template<bool O>
		bool operator!=(const generic_iterator<O>& other) const {
			return m_Node != other.m_Node;
		}

		template<bool> friend class generic_iterator;
	};

	using iterator = generic_iterator<false>;
	using const_iterator = generic_iterator<true>;

public:
	LinkedList();

	LinkedList(const std::initializer_list<T>& list);

	LinkedList(const LinkedList& other);

	LinkedList(LinkedList&& other) noexcept;

	LinkedList& operator=(const LinkedList& other);

	LinkedList& operator=(LinkedList&& other) noexcept;

	~LinkedList();

	T& at(size_t index);

	const T& at(size_t index) const;

	void push_back(const T& value);

	template<typename... Args>
	void emplace_back(Args&&... args);

	void push_front(const T& value);

	template<typename... Args>
	void emplace_front(Args&&... args);

	void insert(size_t index, const T& value);

	template<typename... Args>
	void emplace(size_t index, Args&&... args);

	void pop_back();

	void pop_front();

	void erase(size_t index);

	T& front();

	const T& front() const;

	T& back();

	const T& back() const;

	void swap_values(size_t index1, size_t index2);

	void remove_first_occurrence(const T& value);

	void remove_all_occurrences(const T& value);

	size_t find_first_occurrence(const T& value) const;

	Vector<size_t> find_all_occurrences(const T& value) const;

	void clear();

	size_t size() const;

	bool contains(const T& value) const;

	iterator begin() {
		return iterator(m_Head);
	}

	iterator end() {
		return iterator(nullptr);
	}

	const_iterator begin() const {
		return const_iterator(m_Head);
	}

	const_iterator end() const {
		return const_iterator(nullptr);
	}

	const_iterator cbegin() const {
		return const_iterator(m_Head);
	}

	const_iterator cend() const {
		return const_iterator(nullptr);
	}

private:
	Node* traverse_to(size_t idx) const {
		Node* currentNode = m_Head;
		size_t currentIdx = 0;
		while (currentIdx < idx && currentNode != nullptr) {
			currentNode = currentNode->m_Next;
			currentIdx++;
		}
		if (currentIdx < idx) {
			throw std::out_of_range("Index out of bounds.");
		}
		return currentNode;
	}

	void unlink_node(Node* node) {
		if (node->m_Prev) {
			node->m_Prev->m_Next = node->m_Next;
		}
		else {
			m_Head = node->m_Next;
		}

		if (node->m_Next) {
			node->m_Next->m_Prev = node->m_Prev;
		}
		else {
			m_Tail = node->m_Prev;
		}

		delete node;
	}
};

template<typename T>
LinkedList<T>::LinkedList() {
	m_Head = nullptr;
	m_Tail = nullptr;
}

template<typename T>
LinkedList<T>::LinkedList(const std::initializer_list<T>& list) {
	for (const auto& v : list) {
		push_back(v);
	}
}

template<typename T>
LinkedList<T>::LinkedList(const LinkedList& other) {
	m_Head = nullptr;
	m_Tail = nullptr;

	Node* current = other.m_Head;
	while (current) {
		push_back(current->m_Value);
		current = current->m_Next;
	}
}

template<typename T>
LinkedList<T>::LinkedList(LinkedList&& other) noexcept {
	m_Head = std::exchange(other.m_Head, nullptr);
	m_Tail = std::exchange(other.m_Tail, nullptr);
}

template<typename T>
LinkedList<T>& LinkedList<T>::operator=(const LinkedList& other) {
	if (this != &other) {
		LinkedList temp(other);
		swap(m_Head, temp.m_Head);
		swap(m_Tail, temp.m_Tail);
	}
	return *this;
}

template<typename T>
LinkedList<T>& LinkedList<T>::operator=(LinkedList&& other) noexcept {
	if (this != &other) {
		while (m_Head != nullptr) {
			pop_front();
		}

		m_Head = std::exchange(other.m_Head, nullptr);
		m_Tail = std::exchange(other.m_Tail, nullptr);
	}
	return *this;
}

template<typename T>
LinkedList<T>::~LinkedList() {
	while (m_Head) {
		pop_front();
	}
}

template<typename T>
T& LinkedList<T>::at(size_t index) {
	return traverse_to(index)->m_Value;
}

template<typename T>
const T& LinkedList<T>::at(size_t index) const {
	return traverse_to(index)->m_Value;
}

template<typename T>
void LinkedList<T>::push_back(const T& value) {
	emplace_back(value);
}

template<typename T>
template<typename... Args>
void LinkedList<T>::emplace_back(Args&&... args) {
	if (m_Head == nullptr) {
		m_Head = m_Tail = new Node(T(std::forward<Args>(args)...), nullptr, nullptr);
		return;
	}
	if (m_Head == m_Tail) {
		m_Tail = m_Tail->m_Next = new Node(T(std::forward<Args>(args)...), m_Head, nullptr);
		return;
	}
	m_Tail = m_Tail->m_Next = new Node(T(std::forward<Args>(args)...), m_Tail, nullptr);
}

template<typename T>
void LinkedList<T>::push_front(const T& value) {
	emplace_front(value);
}

template<typename T>
template<typename... Args>
void LinkedList<T>::emplace_front(Args&&... args) {
	if (m_Tail == nullptr) {
		m_Tail = m_Head = new Node(T(std::forward<Args>(args)...), nullptr, nullptr);
		return;
	}
	if (m_Head == m_Tail) {
		m_Head = m_Head->m_Prev = new Node(T(std::forward<Args>(args)...), nullptr, m_Tail);
		return;
	}
	m_Head = m_Head->m_Prev = new Node(T(std::forward<Args>(args)...), nullptr, m_Head);
}

template<typename T>
void LinkedList<T>::insert(size_t index, const T& value) {
	emplace(index, value);
}

template<typename T>
template<typename... Args>
void LinkedList<T>::emplace(size_t index, Args&&... args) {
	if (index == 0) {
		push_front(T(std::forward<Args>(args)...));
		return;
	}
	if (index == size()) {
		push_back(T(std::forward<Args>(args)...));
		return;
	}

	Node* node = traverse_to(index);
	Node* newNode = new Node(T(std::forward<Args>(args)...), node->m_Prev, node);
	if (node == m_Head) {
		m_Head = newNode;
	}
}

template<typename T>
void LinkedList<T>::pop_back() {
	if (!m_Tail) return;

	Node* temp = m_Tail;
	m_Tail = m_Tail->m_Prev;

	if (m_Tail) {
		m_Tail->m_Next = nullptr;
	}
	else {
		m_Head = nullptr;
	}

	delete temp;
}

template<typename T>
void LinkedList<T>::pop_front() {
	if (!m_Head) return;

	Node* temp = m_Head;
	m_Head = m_Head->m_Next;

	if (m_Head) {
		m_Head->m_Prev = nullptr;
	}
	else {
		m_Tail = nullptr;
	}

	delete temp;
}

template<typename T>
void LinkedList<T>::erase(size_t index) {
	unlink_node(traverse_to(index));
}

template<typename T>
T& LinkedList<T>::front() {
	if (m_Head == nullptr) {
		throw std::runtime_error("front called on empty list.");
	}
	return m_Head->m_Value;
}

template<typename T>
const T& LinkedList<T>::front() const {
	if (m_Head == nullptr) {
		throw std::runtime_error("front called on empty list.");
	}
	return m_Head->m_Value;
}

template<typename T>
T& LinkedList<T>::back() {
	if (m_Tail == nullptr) {
		throw std::runtime_error("back called on empty list.");
	}
	return m_Tail->m_Value;
}

template<typename T>
const T& LinkedList<T>::back() const {
	if (m_Tail == nullptr) {
		throw std::runtime_error("back called on empty list.");
	}
	return m_Tail->m_Value;
}

template<typename T>
void LinkedList<T>::swap_values(size_t index1, size_t index2) {
	Node* node1 = traverse_to(index1);
	Node* node2 = traverse_to(index2);

	T temp = node1->m_Value;
	node1->m_Value = node2->m_Value;
	node2->m_Value = temp;
}

template<typename T>
void LinkedList<T>::remove_first_occurrence(const T& value) {
	Node* current = m_Head;
	while (current != nullptr) {
		if (current->m_Value == value) {
			unlink_node(current);
			return;
		}
		current = current->m_Next;
	}
}

template<typename T>
void LinkedList<T>::remove_all_occurrences(const T& value) {
	Node* current = m_Head;
	while (current != nullptr) {
		Node* next = current->m_Next;
		if (current->m_Value == value) {
			unlink_node(current);
		}
		current = next;
	}
}

template<typename T>
size_t LinkedList<T>::find_first_occurrence(const T& value) const {
	Node* current = m_Head;
	size_t currentIdx = 0;
	while (current != nullptr) {
		if (current->m_Value == value) {
			return currentIdx;
		}
		current = current->m_Next;
		currentIdx++;
	}
	return currentIdx;
}

template<typename T>
Vector<size_t> LinkedList<T>::find_all_occurrences(const T& value) const {
	Node* current = m_Head;
	size_t currentIdx = 0;
	Vector<size_t> indices;
	while (current != nullptr) {
		if (current->m_Value == value) {
			indices.push_back(currentIdx);
		}
		current = current->m_Next;
		currentIdx++;
	}
	return indices;
}

template<typename T>
void LinkedList<T>::clear() {
	while (m_Head) {
		pop_back();
	}
}

template<typename T>
size_t LinkedList<T>::size() const {
	Node* current = m_Head;
	size_t size = 0;
	while (current != nullptr) {
		size++;
		current = current->m_Next;
	}
	return size;
}

template<typename T>
bool LinkedList<T>::contains(const T& value) const {
	return find_first_occurrence(value) != size();
}

#pragma endregion

#pragma region Array

#pragma region Class Definition

template<typename T, size_t N>
class Array {
private:
	T m_Array[N];

public:
	Array();

	Array(const T& defaultValue);

	Array(const std::initializer_list<T>& list);

	Array(const Array& other);

	Array(Array&& other) noexcept;

	Array& operator=(const Array& other);

	Array& operator=(Array&& other) noexcept;

	~Array();

	T& operator[](size_t index);

	const T& operator[](size_t index) const;

	T& at(size_t index);

	const T& at(size_t index) const;

	T& back();

	const T& back() const;

	T& front();

	const T& front() const;

	size_t size() const;

	T* data();

	const T* data() const;

	T* begin() noexcept;

	const T* begin() const noexcept;

	const T* cbegin() const noexcept;

	T* end() noexcept;

	const T* end() const noexcept;

	const T* cend() const noexcept;
};

template<typename T, size_t N>
Array<T, N>::Array() {
	for (size_t i = 0; i < N; ++i) {
		m_Array[i] = {};
	}
}

template<typename T, size_t N>
Array<T, N>::Array(const T& defaultValue) {
	for (size_t i = 0; i < N; ++i) {
		m_Array[i] = defaultValue;
	}
}

template<typename T, size_t N>
Array<T, N>::Array(const std::initializer_list<T>& list) 
	: m_Array({}) {
	if (list.size() > N) {
		throw std::out_of_range("Initializer list too large");
	}
	size_t idx = 0;
	for (const auto& value : list) {
		m_Array[idx++] = value;
	}
	for (; idx < N; ++idx) {
		m_Array[idx] = T();
	}
}

template<typename T, size_t N>
Array<T, N>::Array(const Array& other) {
	for (size_t i = 0; i < N; ++i) {
		m_Array[i] = other.m_Array[i];
	}
}

template<typename T, size_t N>
Array<T, N>::Array(Array&& other) noexcept {
	for (size_t i = 0; i < N; ++i) {
		m_Array[i] = std::move(other.m_Array[i]);
	}
}

template<typename T, size_t N>
Array<T, N>& Array<T, N>::operator=(const Array& other) {
	if (this != &other) {
		for (size_t i = 0; i < N; ++i) {
			m_Array[i] = other.m_Array[i];
		}
	}
	return *this;
}

template<typename T, size_t N>
Array<T, N>& Array<T, N>::operator=(Array&& other) noexcept {
	if (this != &other) {
		for (size_t i = 0; i < N; ++i) {
			m_Array[i] = std::move(other.m_Array[i]);
		}
	}
	return *this;
}

template<typename T, size_t N>
Array<T, N>::~Array() = default;

template<typename T, size_t N>
T& Array<T, N>::operator[](size_t index) {
	return m_Array[index];
}

template<typename T, size_t N>
const T& Array<T, N>::operator[](size_t index) const {
	return m_Array[index];
}

template<typename T, size_t N>
T& Array<T, N>::at(size_t index) {
	if (index >= N) {
		throw std::out_of_range("Index out of bounds");
	}
	return m_Array[index];
}

template<typename T, size_t N>
const T& Array<T, N>::at(size_t index) const {
	if (index >= N) {
		throw std::out_of_range("Index out of bounds");
	}
	return m_Array[index];
}

template<typename T, size_t N>
T& Array<T, N>::back() {
	return m_Array[N - 1];
}

template<typename T, size_t N>
const T& Array<T, N>::back() const {
	return m_Array[N - 1];
}

template<typename T, size_t N>
T& Array<T, N>::front() {
	return m_Array[0];
}

template<typename T, size_t N>
const T& Array<T, N>::front() const {
	return m_Array[0];
}

template<typename T, size_t N>
size_t Array<T, N>::size() const {
	return N;
}

template<typename T, size_t N>
T* Array<T, N>::data() {
	return m_Array;
}

template<typename T, size_t N>
const T* Array<T, N>::data() const {
	return m_Array;
}

template<typename T, size_t N>
T* Array<T, N>::begin() noexcept {
	return m_Array;
}

template<typename T, size_t N>
const T* Array<T, N>::begin() const noexcept {
	return m_Array;
}

template<typename T, size_t N>
const T* Array<T, N>::cbegin() const noexcept {
	return m_Array;
}

template<typename T, size_t N>
T* Array<T, N>::end() noexcept {
	return m_Array + N;
}

template<typename T, size_t N>
const T* Array<T, N>::end() const noexcept {
	return m_Array + N;
}

template<typename T, size_t N>
const T* Array<T, N>::cend() const noexcept {
	return m_Array + N;
}

#pragma endregion

#pragma endregion

#pragma region HashMap

constexpr size_t DEFAULT_NUM_BUCKETS = 8;

template<typename KeyT, typename ValT, class Hash = std::hash<KeyT>>
class HashMap {
private:
	using PairT = std::pair<KeyT, ValT>;

	Vector<LinkedList<PairT>> m_Buckets;
	size_t m_NumBuckets;
	size_t m_Size;

	template<bool IsConst>
	class generic_iterator {
	private:
		using MapPtr = std::conditional_t<IsConst, const HashMap*, HashMap*>;
		using ListIter = std::conditional_t<IsConst,
			typename LinkedList<PairT>::const_iterator,
			typename LinkedList<PairT>::iterator
		>;

		MapPtr m_Map;
		size_t m_Idx;
		ListIter m_ListIter;

		void advance() {
			while (m_Idx < m_Map->m_Buckets.size() && m_ListIter == m_Map->m_Buckets[m_Idx].end()) {
				if (++m_Idx < m_Map->m_Buckets.size()) {
					m_ListIter = m_Map->m_Buckets[m_Idx].begin();
				}
			}
		}

	public:
		using iterator_category = std::forward_iterator_tag;
		using value_type = PairT;
		using reference = std::conditional_t<IsConst, const PairT&, PairT&>;
		using pointer = std::conditional_t<IsConst, const PairT*, PairT*>;

		generic_iterator(MapPtr map, size_t idx) : m_Map(map), m_Idx(idx), m_ListIter(nullptr) {
			if (idx < map->m_Buckets.size()) {
				m_ListIter = m_Map->m_Buckets[idx].begin();
				advance();
			}
		}

		reference operator*() const {
			return *m_ListIter;
		}

		pointer operator->() const {
			return &(*m_ListIter);
		}

		generic_iterator& operator++() {
			++m_ListIter;
			advance();
			return *this;
		}

		generic_iterator operator++(int) {
			auto temp = *this;
			++(*this);
			return temp;
		}

		bool operator==(const generic_iterator& other) const {
			return m_Idx == other.m_Idx && (m_Idx == m_Map->m_Buckets.size() || m_ListIter == other.m_ListIter);
		}

		bool operator!=(const generic_iterator& other) const {
			return !(*this == other);
		}
	};

public:
	using iterator = generic_iterator<false>;
	using const_iterator = generic_iterator<true>;

	HashMap();

	HashMap(const std::initializer_list<PairT>& list);

	HashMap(const HashMap& other);

	HashMap(HashMap&& other) noexcept;

	HashMap& operator=(const HashMap& other);

	HashMap& operator=(HashMap&& other) noexcept;

	ValT& operator[](const KeyT& key);

	const ValT& operator[](const KeyT& key) const;

	ValT* find(const KeyT& key);

	const ValT* find(const KeyT& key) const;

	std::pair<KeyT, ValT>& insert(const KeyT& key, const ValT& val);

	void clear();

	bool empty() const;

	bool contains(const KeyT& key) const;

	size_t num_buckets() const;

	size_t size() const;

	iterator begin() {
		return iterator(this, 0);
	}

	const_iterator begin() const {
		return const_iterator(this, 0);
	}

	iterator end() {
		return iterator(this, m_Buckets.size());
	}

	const_iterator end() const {
		return const_iterator(this, m_Buckets.size());
	}

	const_iterator cbegin() const {
		return const_iterator(this, 0);
	}

	const_iterator cend() const {
		return const_iterator(this, m_Buckets.size());
	}

private:
	float get_load_factor() const;

	Vector<std::pair<KeyT, ValT>> get_values() const;

	void rehash();

	size_t get_key_index(const KeyT& key) const;

	size_t get_key_bucket(const KeyT& key) const;
};

template<typename KeyT, typename ValT, class Hash>
HashMap<KeyT, ValT, Hash>::HashMap()
	: m_Buckets(DEFAULT_NUM_BUCKETS) {
	m_Buckets.resize(DEFAULT_NUM_BUCKETS);
	m_NumBuckets = DEFAULT_NUM_BUCKETS;
	m_Size = 0;
}

template<typename KeyT, typename ValT, class Hash>
HashMap<KeyT, ValT, Hash>::HashMap(const std::initializer_list<std::pair<KeyT, ValT>>& list)
	: m_Buckets(next_power_of_two(list.size())), m_NumBuckets(next_power_of_two(list.size())) {
	m_Size = list.size();
	m_Buckets.resize(m_NumBuckets);
	for (auto& pair : list) {
		size_t bucketIdx = Hash{}(pair.first) % m_NumBuckets;
		m_Buckets[bucketIdx].emplace_back(pair.first, pair.second);
	}
}

template<typename KeyT, typename ValT, class Hash>
HashMap<KeyT, ValT, Hash>::HashMap(const HashMap& other) {
	m_Buckets = other.m_Buckets;
	m_NumBuckets = other.m_NumBuckets;
	m_Size = other.m_Size;
}

template<typename KeyT, typename ValT, class Hash>
HashMap<KeyT, ValT, Hash>::HashMap(HashMap&& other) noexcept {
	m_Buckets = std::move(other.m_Buckets);
	m_NumBuckets = std::exchange(other.m_NumBuckets, 0);
	m_Size = std::exchange(other.m_Size, 0);
}

template<typename KeyT, typename ValT, class Hash>
HashMap<KeyT, ValT, Hash>& HashMap<KeyT, ValT, Hash>::operator=(const HashMap& other) {
	if (this != &other) {
		HashMap temp(other);
		Vector<LinkedList<std::pair<KeyT, ValT>>> tempBuckets = m_Buckets;
		m_Buckets = temp.m_Buckets;
		temp.m_Buckets = tempBuckets;
		swap(m_NumBuckets, temp.m_NumBuckets);
		swap(m_Size, temp.m_Size);
	}
	return *this;
}

template<typename KeyT, typename ValT, class Hash>
HashMap<KeyT, ValT, Hash>& HashMap<KeyT, ValT, Hash>::operator=(HashMap&& other) noexcept {
	if (this != &other) {
		m_Buckets = std::move(other.m_Buckets);
		m_NumBuckets = std::exchange(other.m_NumBuckets, 0);
		m_Size = std::exchange(other.m_Size, 0);
	}
	return *this;
}

template<typename KeyT, typename ValT, class Hash>
ValT& HashMap<KeyT, ValT, Hash>::operator[](const KeyT& key) {
	size_t bucketIndex = get_key_bucket(key);
	size_t keyIndex = get_key_index(key);

	if (keyIndex == m_Buckets[bucketIndex].size()) {
		return insert(key, {}).second;
	}

	return m_Buckets[bucketIndex].at(keyIndex).second;
}

template<typename KeyT, typename ValT, class Hash>
const ValT& HashMap<KeyT, ValT, Hash>::operator[](const KeyT& key) const {
	size_t keyIndex = get_key_index(key);
	size_t bucketIndex = get_key_bucket(key);
	auto& bucket = m_Buckets[bucketIndex];
	size_t bucketSize = bucket.size();
	if (keyIndex == bucketSize) {
		throw std::runtime_error("Attempted to insert into constant hashmap.");
	}
	return bucket.at(keyIndex).second;
}

template<typename KeyT, typename ValT, class Hash>
ValT* HashMap<KeyT, ValT, Hash>::find(const KeyT& key) {
	size_t bucketIdx = get_key_bucket(key);
	auto& bucket = m_Buckets[bucketIdx];
	for (auto it = bucket.begin(); it != bucket.end(); ++it) {
		if (it->first == key) {
			return &it->second;
		}
	}
	return nullptr;
}

template<typename KeyT, typename ValT, class Hash>
const ValT* HashMap<KeyT, ValT, Hash>::find(const KeyT& key) const {
	size_t bucketIdx = get_key_bucket(key);
	auto& bucket = m_Buckets[bucketIdx];
	for (auto it = bucket.begin(); it != bucket.end(); ++it) {
		if (it->first == key) {
			return &it->second;
		}
	}
	return nullptr;
}

template<typename KeyT, typename ValT, class Hash>
std::pair<KeyT, ValT>& HashMap<KeyT, ValT, Hash>::insert(const KeyT& key, const ValT& val) {
	size_t bucketIdx = get_key_bucket(key);
	size_t keyIdx = get_key_index(key);
	if (keyIdx == m_Buckets[bucketIdx].size()) {
		m_Size++;
		if (get_load_factor() > 1.0f) {
			rehash();
		}
		bucketIdx = get_key_bucket(key);
		m_Buckets[bucketIdx].emplace_back(key, val);
		return m_Buckets[bucketIdx].back();
	}
	return m_Buckets[bucketIdx].at(keyIdx);
}

template<typename KeyT, typename ValT, class Hash>
void HashMap<KeyT, ValT, Hash>::clear() {
	m_Buckets.clear();
	m_NumBuckets = DEFAULT_NUM_BUCKETS;
	m_Buckets.resize(m_NumBuckets);
	m_Size = 0;
}

template<typename KeyT, typename ValT, class Hash>
bool HashMap<KeyT, ValT, Hash>::empty() const {
	return m_Size == 0;
}

template<typename KeyT, typename ValT, class Hash>
bool HashMap<KeyT, ValT, Hash>::contains(const KeyT& key) const {
	return find(key) != nullptr;
}

template<typename KeyT, typename ValT, class Hash>
size_t HashMap<KeyT, ValT, Hash>::num_buckets() const {
	return m_NumBuckets;
}


template<typename KeyT, typename ValT, class Hash>
size_t HashMap<KeyT, ValT, Hash>::size() const {
	return m_Size;
}

template<typename KeyT, typename ValT, class Hash>
float HashMap<KeyT, ValT, Hash>::get_load_factor() const {
	return static_cast<float>(m_Size) / m_NumBuckets;
}

template<typename KeyT, typename ValT, class Hash>
Vector<std::pair<KeyT, ValT>> HashMap<KeyT, ValT, Hash>::get_values() const {
	Vector<std::pair<KeyT, ValT>> values;
	for (size_t i = 0; i < m_NumBuckets; ++i) {
		for (auto it = m_Buckets[i].begin(); it != m_Buckets[i].end(); ++it) {
			values.emplace_back(it->first, it->second);
		}
	}
	return values;
}

template<typename KeyT, typename ValT, class Hash>
void HashMap<KeyT, ValT, Hash>::rehash() {
	float loadFactor = get_load_factor();
	if (loadFactor > 1.0) {
		Vector<std::pair<KeyT, ValT>> values = get_values();
		m_NumBuckets *= 2;

		m_Buckets.clear();
		m_Buckets.resize(m_NumBuckets);

		for (size_t i = 0; i < values.size(); ++i) {
			size_t bucketIdx = Hash{}(values[i].first) % m_NumBuckets;
			m_Buckets[bucketIdx].emplace_back(values[i].first, values[i].second);
		}
	}
	else if (m_NumBuckets > DEFAULT_NUM_BUCKETS && loadFactor < 0.5) {
		Vector<std::pair<KeyT, ValT>> values = get_values();
		m_NumBuckets /= 2;

		m_Buckets.clear();
		m_Buckets.resize(m_NumBuckets);

		for (size_t i = 0; i < values.size(); ++i) {
			size_t bucketIdx = Hash{}(values[i].first) % m_NumBuckets;
			m_Buckets[bucketIdx].emplace_back(values[i].first, values[i].second);
		}
	}
}

template<typename KeyT, typename ValT, class Hash>
size_t HashMap<KeyT, ValT, Hash>::get_key_bucket(const KeyT& key) const {
	return Hash{}(key) % m_NumBuckets;
}

template<typename KeyT, typename ValT, class Hash>
size_t HashMap<KeyT, ValT, Hash>::get_key_index(const KeyT& key) const {
	size_t bucketIdx = get_key_bucket(key);
	size_t idx = 0;
	for (auto it = m_Buckets[bucketIdx].begin(); it != m_Buckets[bucketIdx].end(); ++it) {
		if (it->first == key) {
			return idx;
		}
		idx++;
	}
	return idx;
}

#pragma endregion

#endif
