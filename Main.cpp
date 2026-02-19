#pragma once

#include <initializer_list>		// Included for std::initializer_list, which is used for the construct.
#include <stdexcept>			// Included to throw std::runtime_error when something goes wrong.
#include <cassert>				// Included to use assert() which is a quick way of throwing an exception.

/**********************************************************************
* Class Name: Vector
* Class Description:
*	A custom dynamic array implementation that is similar to
*	std::vector.
*	Supports copy/move semantics, dynamic resizing, and element access
*	with bounds checking.
**********************************************************************/
template<typename T>
class Vector {
private:
	T* m_Data;				// Pointer to the dynamically allocated array.
	size_t m_Capacity;		// Total capacity of the array.
	size_t m_Size;			// Current number of elements in the array.

public:
	// Default Constructor: Creates an empty Vector
	Vector();

	// Constructor with initializer list
	// Allows for initialization like: Vector<int> vec = { 0, 1, 2, 3 }.
	Vector(const std::initializer_list<T>&);

	// Constructor with size_t argument for capacity
	// Allows for the user to allocate a certain amount of memory before using the Vector.
	Vector(size_t);

	// Copy Constructor
	// Allows for a Vector to be copied to the new one.
	Vector(const Vector&);

	// Move Constructor
	// Allows for a vector to be created with std::move().
	Vector(Vector&&) noexcept;

	// Basic = operator
	// Allows for a vector to be copied using the equal operator.
	Vector& operator=(const Vector&);

	// Move = operator
	// Allows for vectors to be moved using the equal operator.
	Vector& operator=(Vector&&) noexcept;

	// Destructor
	// Prevents memory leaks by deleting the data array.
	~Vector();

	// non-const operator[](size_t)
	// Allows for the Vector to be indexed and edited without bounds checking.
	T& operator[](size_t);

	// const operator[](size_t)
	// Allows for the Vector to be indexed without bounds checking.
	const T& operator[](size_t) const;

	// at(size_t) Function
	// Allows for the Vector to be indexed and edited with bounds checking.
	T& at(size_t);

	// Const at(size_t) Function
	// Allows for const Vector to be index with bounds checking.
	const T& at(size_t) const;

	// push_back(const T&) Function
	// Pushes a new element to the back of the Vector.
	void push_back(const T&);

	// push_back(T&&) Move Function
	// Pushes a moved element to the back of the Vector.
	void push_back(T&&);

	// push_front(const T&) Function
	// Pushes a new element to the front of the Vector.
	void push_front(const T&);

	// push_front(T&&) Move Function
	// Pushes a moved element to the front of the Vector.
	// Note: This function has an O(n) time complexity, and PushBack() should always be preferred for its O(1) time complexity.
	void push_front(T&&);

	// pop_back() Function
	// Pops the element at the back of the Vector and returns the value.
	T pop_back();

	// pop_front() Function
	// Pops the element at the front of the Vector and returns the value.
	// Note: This function has an O(n) time complexity, and PopBack() should always be preferred for its O(1) time complexity.
	T pop_front();

	// remove_at(size_t) Function
	// Remove the element at a given index.
	void remove_at(size_t);

	// back() Function
	// Returns a reference to the element at the back of the Vector.
	T& back();

	// const back() Function
	// Return a const reference to the element at the back of the Vector.
	const T& back() const;

	// front() Function
	// Returns a reference to the element at the front of the Vector.
	T& front();

	// const front() Function
	// Returns a const reference to the element at the front of the vector.
	const T& front() const;

	// find(const T&) Function
	// Finds the index of an element, returns m_Size if it is not found as m_Size is not a valid index.
	// Note that this requires T to have an == operator.
	size_t find(const T&) const;

	// clear() Function
	// Clears the Vector by setting the size to 0.
	void clear();

	// empty() Function
	// Returns if the Vector is empty.
	bool empty() const;

	// size() Function
	// Returns the size of the Vector.
	size_t size() const;

	// capacity() Function
	// Returns the capacity of the Vector.
	size_t capacity() const;

private:
	// grow() Function
	// Double the capacity, copy the values of the array, and create a new array.
	void grow();
};

// Note: The function names in the comments below are no longer 
// accurate as I switched to snake case as it is what the STL vector uses.

// Default Constructor: Creates an empty Vector
template<typename T>
Vector<T>::Vector() {
	m_Data = nullptr;
	m_Capacity = 0;
	m_Size = 0;
}

// Constructor with initializer list
// Allows for initialization like: Vector<int> vec = { 0, 1, 2, 3 }.
template<typename T>
Vector<T>::Vector(const std::initializer_list<T>& values)
	: Vector(values.size()) {		// Preallocate memory for the list.
	// Iterate over the initializer list.
	for (const T& v : values) {
		PushBack(v);		// Insert each value.
	}
}

// Constructor with size_t argument for capacity
// Allows for the user to allocate a certain amount of memory before using the Vector.
template<typename T>
Vector<T>::Vector(size_t capacity) {
	m_Data = new T[capacity];
	m_Capacity = capacity;
	m_Size = 0;
}

// Copy Constructor
// Allows for a Vector to be copied to the new one.
template<typename T>
Vector<T>::Vector(const Vector<T>& other) {
	// Copy values into the new Vector.
	m_Data = new T[other.m_Capacity];
	m_Capacity = other.m_Capacity;
	m_Size = other.m_Size;

	// Iterate over the other Vector's data array.
	for (size_t i = 0; i < m_Size; ++i) {
		assert(i < m_Capacity);		// Stupid assert to get rid of the nonsense Intellisense warning. This can never happen.
		m_Data[i] = other.m_Data[i];		// Deep copy the other's data to the new Vector's data.
	}
}

// Move Constructor
// Allows for a vector to be created with std::move().
template<typename T>
Vector<T>::Vector(Vector<T>&& other) noexcept {
	// Copy values into the new Vector.
	m_Data = other.m_Data;
	m_Capacity = other.m_Capacity;
	m_Size = other.m_Size;
	// Reset the other vector's values.
	other.m_Data = nullptr;
	other.m_Capacity = 0;
	other.m_Size = 0;
}

// Basic = operator
// Allows for a vector to be copied using the equal operator.
template<typename T>
Vector<T>& Vector<T>::operator=(const Vector& other) {
	if (this == &other) return *this;		// If it is being set to itself, return the dereferenced this pointer instead of creating a whole new array.

	delete[] m_Data;		// Delete the last m_Data.

	m_Capacity = other.m_Capacity;		// Copy the capacity.
	m_Size = other.m_Size;				// Copy the size.
	m_Data = new T[m_Capacity];			// Create a new array with the other Vector's capacity.
	for (size_t i = 0; i < m_Size; ++i) {
		assert(i < m_Capacity);
		m_Data[i] = other.m_Data[i];		// Deep copy each element from the other Vector to the new Vector.
	}
	return *this;		// Return the dereferenced this pointer. This allows for chained equals like vec1 = vec2 = vec3.
}

// Move = operator
// Allows for vectors to be moved using the equal operator.
template<typename T>
Vector<T>& Vector<T>::operator=(Vector&& other) noexcept {
	if (this == &other) return *this;		// If it is being set to itself, return the dereferenced this pointer instead of creating a new array.

	delete[] m_Data;		// Delete the old array.

	m_Data = other.m_Data;		// Directly copy the data pointer as the other Vector's data is set to nullptr.
	m_Capacity = other.m_Capacity;		// Copy the capacity.
	m_Size = other.m_Size;		// Copy the size.

	other.m_Data = nullptr;		// Set the other Vector's data to nullptr to prevent double frees.
	other.m_Capacity = 0;		// Set the other Vector's capacity to zero.
	other.m_Size = 0;		// Set the other Vector's size to zero.

	return *this;		// Return the dereferenced this pointer to allow for chained equals like vec1 = vec2 = std::move(vec3).
}

// Destructor
// Prevents memory leaks by deleting the data array.
template<typename T>
Vector<T>::~Vector() {
	if (m_Data) {		// Make sure data is not nullptr.
		delete[] m_Data;		// Delete all elements in m_Data using delete[].
	}
}

// Non-const [size_t] operator
// Allows for the Vector to be indexed and edited without bounds checking.
template<typename T>
T& Vector<T>::operator[](size_t idx) {
	return m_Data[idx];		// Return the m_Data pointer at idx without bounds checking.
}

// Const [size_t] operator
// Allows for the Vector to be indexed without bounds checking.
template<typename T>
const T& Vector<T>::operator[](size_t idx) const {
	return m_Data[idx];		// Return the m_Data pointer at idx without bounds checking.
}

// At(size_t) Function
// Allows for the Vector to be indexed and edited with bounds checking.
template<typename T>
T& Vector<T>::at(size_t idx) {
	// Check that the Vector's size is less than the index given.
	if (idx >= m_Size) {
		throw std::runtime_error("Vector Error: Index given to At() is out of bounds.");		// Throw an exception with the error message.
	}
	return m_Data[idx];		// Return the m_Data pointer at idx with bounds checking.
}

// Const At(size_t) Function
// Allows for const Vector to be index with bounds checking.
template<typename T>
const T& Vector<T>::at(size_t idx) const {
	// Check that the Vector's size is less than the index given.
	if (idx >= m_Size) {
		throw std::runtime_error("Vector Error: Index given to At() is out of bounds.");		// Throw an exception with the error message.
	}
	return m_Data[idx];		// Return the m_Data pointer at idx with bounds checking.
}

// PushBack(const T&) Function
// Pushes a new element to the back of the Vector.
template<typename T>
void Vector<T>::push_back(const T& value) {
	// Check if the Vector has enough capacity to push back the element.
	if (m_Size + 1 > m_Capacity) {
		grow();		// Call the Grow() function to grow the array.
	}
	m_Data[m_Size++] = value;		// Set m_Data at the previous size to value and increment m_Size.
}

// PushBack(T&&) Move Function
// Pushes a moved element to the back of the Vector.
template<typename T>
void Vector<T>::push_back(T&& value) {
	// Check if the Vector has enough capacity to move the element to the back.
	if (m_Size + 1 > m_Capacity) {
		grow();		// Call the Grow() function to grow the array.
	}
	m_Data[m_Size++] = std::move(value);		// Move the value to m_Data at the previous size and increment m_Size.
}

// PushFront(const T&) Function
// Pushes a new element to the front of the Vector.
template<typename T>
void Vector<T>::push_front(const T& value) {
	// Check if the Vector has enough capacity to push the element to the front.
	if (m_Size + 1 > m_Capacity) {
		grow();		// Call the Grow() function to grow the array.
	}
	++m_Size;		// Increment m_Size.
	// Iterate over the array from the back.
	for (size_t i = m_Size - 1; i > 0; --i) {
		m_Data[i] = m_Data[i - 1];		// Move the m_Data at i one space to the right.
	}
	m_Data[0] = value;		// Set the now free m_Data[0] to the given value.
}

// PushFront(T&&) Move Function
// Pushes a moved element to the front of the Vector.
// Note: This function has an O(n) time complexity, and PushBack() should always be preferred for its O(1) time complexity.
template<typename T>
void Vector<T>::push_front(T&& value) {
	// Check if the Vector has enough capacity to push the element to the front.
	if (m_Size + 1 > m_Capacity) {
		grow();		// Call the Grow() function to grow the array.
	}
	++m_Size;		// Increment m_Size.
	// Iterate over the array from the back.
	for (size_t i = m_Size - 1; i > 0; --i) {
		m_Data[i] = m_Data[i - 1];		// Move the m_Data at i one space to the right.
	}
	m_Data[0] = std::move(value);		// Set the now free m_Data[0] to the moved value.
}

// PopBack() Function
// Pops the element at the back of the Vector and returns the value.
template<typename T>
T Vector<T>::pop_back() {
	// Check if m_Size is zero as there would be nothing to pop.
	if (m_Size == 0) {
		throw std::runtime_error("Vector Error: PopBack() called when size is equal to 0.");
	}
	return std::move(m_Data[--m_Size]);		// Return the moved value of m_Data at the decremented m_Size.
}

// PopFront() Function
// Pops the element at the front of the Vector and returns the value.
// Note: This function has an O(n) time complexity, and PopBack() should always be preferred for its O(1) time complexity.
template<typename T>
T Vector<T>::pop_front() {
	// Check if m_Size is zero as there would be nothing to pop.
	if (m_Size == 0) {
		throw std::runtime_error("Vector Error: PopFront() called when size is equal to 0.");
	}
	T value = std::move(m_Data[0]);		// Save the value of the front of the Vector before overriding it.
	// Iterate over the array from the front.
	for (size_t i = 0; i < m_Size - 1; ++i) {
		m_Data[i] = std::move(m_Data[i + 1]);		// Move the data at i one space to the left.
	}
	--m_Size;
	return value;
}

// RemoveAt(size_t) Function
// Remove the element at a given index.
template<typename T>
void Vector<T>::remove_at(size_t idx) {
	// Check if the index given is greater or equal to m_Size.
	// It is not necessary to check if it is less than 0 as it is unsigned.
	if (idx >= m_Size) {
		throw std::runtime_error("Vector Error: Index given to RemoveAt() is out of bounds.");
	}
	// Iterate over the array from the index given.
	for (size_t i = idx; i < m_Size - 1; ++i) {
		m_Data[i] = std::move(m_Data[i + 1]);		// Move the data at i one space to the left.
	}
	--m_Size;
}

// Back() Function
// Returns a reference to the element at the back of the Vector.
template<typename T>
T& Vector<T>::back() {
	// Check if m_Size is 0 as that would indicate that there is no back to the Vector.
	if (m_Size == 0) {
		throw std::runtime_error("Vector Error: Back() called when size is equal to 0.");
	}
	return m_Data[m_Size - 1];
}

// const Back() Function
// Return a const reference to the element at the back of the Vector.
template<typename T>
const T& Vector<T>::back() const {
	// Check if m_Size is 0 as that would indicate that there is no back to the Vector.
	if (m_Size == 0) {
		throw std::runtime_error("Vector Error: Back() called when size is equal to 0.");
	}
	return m_Data[m_Size - 1];
}

// Front() Function
// Returns a reference to the element at the front of the Vector.
template<typename T>
T& Vector<T>::front() {
	// Check if m_Size is 0 as that would indicate that there is no front of the Vector.
	if (m_Size == 0) {
		throw std::runtime_error("Vector Error: Front() called when size is equal to 0.");
	}
	return m_Data[0];
}

// const Front() Function
// Returns a const reference to the element at the front of the vector.
template<typename T>
const T& Vector<T>::front() const {
	// Check if m_Size is 0 as that would indicate that there is no front of the Vector.
	if (m_Size == 0) {
		throw std::runtime_error("Vector Error: Front() called when size is equal to 0.");
	}
	return m_Data[0];
}

// Find(const T&) Function
// Finds the index of an element, returns m_Size if it is not found as m_Size is not a valid index.
// Note that this requires T to have an == operator.
template<typename T>
size_t Vector<T>::find(const T& required) const {
	for (size_t i = 0; i < m_Size; ++i) {
		if (m_Data[i] == required) {
			return static_cast<int>(i);
		}
	}
	return m_Size;
}

// Clear() Function
// Clears the Vector by setting the size to 0.
template<typename T>
void Vector<T>::clear() {
	m_Size = 0;
}

// Empty() Function
// Returns if the Vector is empty.
template<typename T>
bool Vector<T>::empty() const {
	return m_Size == 0;
}

// Size() Function
// Returns the size of the Vector.
template<typename T>
size_t Vector<T>::size() const {
	return m_Size;
}

// Capacity() Function
// Returns the capacity of the Vector.
template<typename T>
size_t Vector<T>::capacity() const {
	return m_Capacity;
}

// Grow() Function
// Double the capacity, copy the values of the array, and create a new array.
// It does not have a snake_case alias because it is only referenced locally.
template<typename T>
void Vector<T>::grow() {
	size_t newCapacity = (m_Capacity == 0) ? 1 : m_Capacity * 2;		// Get the new capacity of the Vector, which is 1 if it is already 0.
	T* copy = new T[newCapacity];		// Create a pointer of type T with the size of the new capacity.
	// Iterate over the original array and deep copy each element to the copy array.
	for (size_t i = 0; i < m_Size; ++i) {
		assert(i < m_Capacity);		// Stupid assert to get rid of the nonsense Intellisense warning.
		copy[i] = m_Data[i];
	}
	delete[] m_Data;
	m_Data = copy;		// Set m_Data to the address of copy.
	m_Capacity = newCapacity;
}

/**********************************************************************
* Class Name: Array
* Class Description:
*	A simple array class. This allows the programmer to use fixed-size
*	arrays without having to deal with the headache of sending the
*	size alongside the array.
**********************************************************************/
template <typename T, size_t N>
class Array {
private:
	T m_Data[N];

public:
	Array();

	Array(const std::initializer_list<T>&);

	Array(const Array&);

	Array(Array&&) noexcept;

	Array& operator=(const Array&);

	Array& operator=(Array&&) noexcept;

	T& operator[](size_t);

	const T& operator[](size_t) const;

	T& at(size_t);

	const T& at(size_t) const;

	T& back();

	const T& back() const;

	T& front();

	const T& front() const;

	size_t find(const T& required) const;

	size_t size() const;
};

template<typename T, size_t N>
Array<T, N>::Array() {
	for (size_t i = 0; i < N; ++i) {
		m_Data[i] = {};
	}
}

template<typename T, size_t N>
Array<T, N>::Array(const std::initializer_list<T>& list) {
	size_t idx = 0;
	for (const auto& val : list) {
		if (idx >= N) {
			break;
		}
		m_Data[idx++] = val;
	}
	for (size_t i = idx; i < N; ++i) {
		m_Data[i] = {};
	}
}

template<typename T, size_t N>
Array<T, N>::Array(const Array& other) {
	size_t idx = 0;
	for (idx = 0; idx < other.size(); ++idx) {
		m_Data[idx] = other[idx];
	}
	for (; idx < N; ++idx) {
		m_Data[idx] = {};
	}
}

template<typename T, size_t N>
Array<T, N>::Array(Array&& other) noexcept {
	for (size_t i = 0; i < N; ++i) {
		m_Data[i] = other.m_Data[i];
		other.m_Data[i] = {};
	}
}

template<typename T, size_t N>
Array<T, N>& Array<T, N>::operator=(const Array& other) {
	size_t idx = 0;
	for (idx = 0; idx < other.size(); ++idx) {
		m_Data[idx] = other[idx];
	}
	for (; idx < N; ++idx) {
		m_Data[idx] = {};
	}
}

template<typename T, size_t N>
Array<T, N>& Array<T, N>::operator=(Array&& other) noexcept {
	for (size_t i = 0; i < N; ++i) {
		m_Data[i] = other.m_Data[i];
		other.m_Data[i] = {};
	}
}

template<typename T, size_t N>
T& Array<T, N>::operator[](size_t idx) {
	return m_Data[idx];
}

template<typename T, size_t N>
const T& Array<T, N>::operator[](size_t idx) const {
	return m_Data[idx];
}

template<typename T, size_t N>
T& Array<T, N>::at(size_t idx) {
	if (idx >= N) {
		throw std::out_of_range("Array Error: at() given index greater than number of elements.");
	}
	return m_Data[idx];
}

template<typename T, size_t N>
const T& Array<T, N>::at(size_t idx) const {
	if (idx >= N) {
		throw std::out_of_range("Array Error: at() given index greater than number of elements.");
	}
	return m_Data[idx];
}

template<typename T, size_t N>
T& Array<T, N>::back() {
	return m_Data[N - 1];
}

template<typename T, size_t N>
const T& Array<T, N>::back() const {
	return m_Data[N - 1];
}

template<typename T, size_t N>
T& Array<T, N>::front() {
	return m_Data[0];
}

template<typename T, size_t N>
const T& Array<T, N>::front() const {
	return m_Data[0];
}

template<typename T, size_t N>
size_t Array<T, N>::find(const T& required) const {
	for (size_t i = 0; i < N; ++i) {
		if (m_Data[i] == required) {
			return i;
		}
	}
	return N;
}

template<typename T, size_t N>
size_t Array<T, N>::size() const {
	return N;
}

/**********************************************************************
* Class Name: Map
* Class Description:
*	An unordered map that allows a value of type KeyT to access a
*	value of type ValT.
*	Note that this implementation does not use hashing, making it
*	wildly inefficient compared to std::unordered_map.
**********************************************************************/
template<typename KeyT, typename ValT>
class Map {
private:
	Vector<KeyT> m_Keys;		// The vector of keys to the corresponding values.
	Vector<ValT> m_Values;		// The vector of values. This is aligned by index with m_Keys.

public:
	Map();

	Map(const std::initializer_list<std::pair<KeyT, ValT>>&);

	ValT& operator[](const KeyT&);

	const ValT& operator[](const KeyT&) const;

	ValT& At(const KeyT&);
	ValT& at(const KeyT&);

	const ValT& At(const KeyT&) const;
	const ValT& at(const KeyT&) const;

	ValT& Back();
	ValT& back();

	const ValT& Back() const;
	const ValT& back() const;

	ValT& Front();
	ValT& front();

	const ValT& Front() const;
	const ValT& front() const;

	KeyT GetKey(size_t) const;
	KeyT get_key(size_t) const;

	ValT GetValue(size_t) const;
	ValT get_value(size_t) const;

	bool Empty() const;
	bool empty() const;

	size_t Size() const;
	size_t size() const;

private:
	size_t FindKey(const KeyT&) const;
};

template<typename KeyT, typename ValT>
Map<KeyT, ValT>::Map() = default;

template<typename KeyT, typename ValT>
Map<KeyT, ValT>::Map(const std::initializer_list<std::pair<KeyT, ValT>>& values) {
	for (const auto& [key, value] : values) {
		m_Keys.PushBack(key);
		m_Values.PushBack(value);
	}
}

template<typename KeyT, typename ValT>
ValT& Map<KeyT, ValT>::operator[](const KeyT& key) {
	size_t keyNum = FindKey(key);
	if (keyNum == m_Keys.Size()) {
		m_Keys.PushBack(key);
		m_Values.PushBack(ValT{});
	}
	return m_Values[FindKey(key)];
}

template<typename KeyT, typename ValT>
const ValT& Map<KeyT, ValT>::operator[](const KeyT& key) const {
	size_t keyNum = FindKey(key);
	if (keyNum == m_Keys.Size()) {
		throw std::runtime_error("Map Error: Attempted to add a value to a constant Map.");
	}
	return m_Values[FindKey(key)];
}

template<typename KeyT, typename ValT>
ValT& Map<KeyT, ValT>::At(const KeyT& key) {
	size_t keyNum = FindKey(key);
	if (keyNum == m_Keys.Size()) {
		m_Keys.PushBack(key);
		m_Values.PushBack(ValT{});
	}
	return m_Values[FindKey(key)];
}

template<typename KeyT, typename ValT>
ValT& Map<KeyT, ValT>::at(const KeyT& key) {
	return At(key);
}

template<typename KeyT, typename ValT>
const ValT& Map<KeyT, ValT>::At(const KeyT& key) const {
	size_t keyNum = FindKey(key);
	if (keyNum == m_Keys.Size()) {
		throw std::runtime_error("Map Error: Attempted to add a value to a constant Map.");
	}
	return m_Values[FindKey(key)];
}

template<typename KeyT, typename ValT>
const ValT& Map<KeyT, ValT>::at(const KeyT& key) const {
	return At(key);
}

template<typename KeyT, typename ValT>
ValT& Map<KeyT, ValT>::Back() {
	if (Size() == 0) {
		throw std::runtime_error("Back() called on empty Map.");
	}
	return m_Values[m_Values.Size() - 1];
}

template<typename KeyT, typename ValT>
ValT& Map<KeyT, ValT>::back() {
	return Back();
}

template<typename KeyT, typename ValT>
const ValT& Map<KeyT, ValT>::Back() const {
	if (Size() == 0) {
		throw std::runtime_error("Back() called on empty Map.");
	}
	return m_Values[m_Values.Size() - 1];
}

template<typename KeyT, typename ValT>
const ValT& Map<KeyT, ValT>::back() const {
	return Back();
}

template<typename KeyT, typename ValT>
ValT& Map<KeyT, ValT>::Front() {
	if (Size() == 0) {
		throw std::runtime_error("Front() called on empty Map.");
	}
	return m_Values[0];
}

template<typename KeyT, typename ValT>
ValT& Map<KeyT, ValT>::front() {
	return Front();
}

template<typename KeyT, typename ValT>
const ValT& Map<KeyT, ValT>::Front() const {
	if (Size() == 0) {
		throw std::runtime_error("Front() called on empty Map.");
	}
	return m_Values[0];
}

template<typename KeyT, typename ValT>
const ValT& Map<KeyT, ValT>::front() const {
	return Front();
}

template<typename KeyT, typename ValT>
KeyT Map<KeyT, ValT>::GetKey(size_t index) const {
	return m_Keys[index];
}

template<typename KeyT, typename ValT>
KeyT Map<KeyT, ValT>::get_key(size_t index) const {
	return GetKey(index);
}

template<typename KeyT, typename ValT>
ValT Map<KeyT, ValT>::GetValue(size_t index) const {
	return m_Values[index];
}

template<typename KeyT, typename ValT>
ValT Map<KeyT, ValT>::get_value(size_t index) const {
	return GetValue(index);
}

template<typename KeyT, typename ValT>
bool Map<KeyT, ValT>::Empty() const {
	if (m_Keys.Size() != m_Values.Size()) {
		throw std::runtime_error("Map Error: Key vector size does not match value vector size.");
	}
	return m_Keys.Empty() && m_Values.Empty();
}

template<typename KeyT, typename ValT>
bool Map<KeyT, ValT>::empty() const {
	return Empty();
}

template<typename KeyT, typename ValT>
size_t Map<KeyT, ValT>::Size() const {
	if (m_Keys.Size() != m_Values.Size()) {
		throw std::runtime_error("Map Error: Key vector size does not match value vector size.");
	}
	return m_Keys.Size();
}

template<typename KeyT, typename ValT>
size_t Map<KeyT, ValT>::size() const {
	return Size();
}

template<typename KeyT, typename ValT>
size_t Map<KeyT, ValT>::FindKey(const KeyT& key) const {
	for (size_t i = 0; i < m_Keys.Size(); ++i) {
		if (m_Keys[i] == key) {
			return i;
		}
	}
	return m_Keys.Size();
}
