#include <stdexcept>
#include <cassert>
#include <initializer_list>
#include <utility>

static int next_power_of_two(int num) {
	int result = 1;
	while (result < num) {
		result *= 2;
	}
	return result;
}

template<typename T>
class Vector {
private:
	T* m_Data;
	size_t m_Size;
	size_t m_Capacity;

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

	void clear();

	bool empty() const;

	size_t size() const;

	size_t capacity() const;

	void resize(size_t newSize);

private:
	void grow();

	template<typename _T>
	static void swap(_T& a, _T& b);
};

template<typename T>
Vector<T>::Vector() {
	m_Data = nullptr;
	m_Size = 0;
	m_Capacity = 0;
}

template<typename T>
Vector<T>::Vector(size_t capacity) {
	m_Data = static_cast<T*>(::operator new(sizeof(T) * capacity));
	m_Size = 0;
	m_Capacity = capacity;
}

template<typename T>
Vector<T>::Vector(const std::initializer_list<T>& list) {
	m_Size = 0;
	m_Capacity = next_power_of_two(list.size());
	m_Data = static_cast<T*>(::operator new(sizeof(T) * m_Capacity));
	for (const auto& v : list) {
		new (&m_Data[m_Size++]) T(v);
	}
}

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

template<typename T>
Vector<T>::Vector(Vector&& other) noexcept {
	m_Data = std::exchange(other.m_Data, nullptr);
	m_Capacity = std::exchange(other.m_Capacity, 0);
	m_Size = std::exchange(other.m_Size, 0);
}

template<typename T>
Vector<T>& Vector<T>::operator=(const Vector& other) {
	if (this == &other) return *this;

	Vector temp(other);
	swap(m_Data, temp.m_Data);
	swap(m_Size, temp.m_Size);
	swap(m_Capacity, temp.m_Capacity);

	return *this;
}

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

template<typename T>
Vector<T>::~Vector() {
	for (size_t i = 0; i < m_Size; ++i) {
		m_Data[i].~T();
	}
	::operator delete(m_Data);
}

template<typename T>
T& Vector<T>::operator[](size_t idx) {
	return m_Data[idx];
}

template<typename T>
const T& Vector<T>::operator[](size_t idx) const {
	return m_Data[idx];
}

template<typename T>
T& Vector<T>::at(size_t idx) {
	if (idx >= m_Size) {
		throw std::out_of_range("Index out of range");
	}
	return m_Data[idx];
}

template<typename T>
const T& Vector<T>::at(size_t idx) const {
	if (idx >= m_Size) {
		throw std::out_of_range("Index out of range");
	}
	return m_Data[idx];
}

template<typename T>
void Vector<T>::push_back(const T& value) {
	if (m_Size + 1 > m_Capacity) {
		grow();
	}
	new (&m_Data[m_Size]) T(value);
	m_Size++;
}

template<typename T>
void Vector<T>::push_back(T&& value) {
	if (m_Size >= m_Capacity) {
		grow();
	}
	new (&m_Data[m_Size]) T(std::move(value));
	m_Size++;
}

template<typename T>
template<typename... Args>
void Vector<T>::emplace_back(Args&&... args) {
	if (m_Size >= m_Capacity) {
		grow();
	}
	new (&m_Data[m_Size]) T(std::forward<Args>(args)...);
	++m_Size;
}

template<typename T>
void Vector<T>::push_front(const T& value) {
	insert(0, value);
}

template<typename T>
void Vector<T>::push_front(T&& value) {
	insert(0, std::move(value));
}

template<typename T>
template<typename... Args>
void Vector<T>::emplace_front(Args&&... args) {
	insert(0, std::forward<Args>(args)...);
}

template<typename T>
void Vector<T>::insert(size_t idx, const T& value) {
	if (idx > m_Size) {
		throw std::out_of_range("Index out of range.");
	}

	if (m_Size >= m_Capacity) {
		grow();
	}

	for (size_t i = m_Size; i > idx; --i) {
		new (&m_Data[i]) T(std::move(m_Data[i - 1]));
		m_Data[i - 1].~T();
	}

	new (&m_Data[idx]) T(value);
	++m_Size;
}

template<typename T>
void Vector<T>::insert(size_t idx, T&& value) {
	if (idx > m_Size) {
		throw std::out_of_range("Index out of range.");
	}

	if (m_Size >= m_Capacity) {
		grow();
	}

	for (size_t i = m_Size; i > idx; --i) {
		new (&m_Data[i]) T(std::move(m_Data[i - 1]));
		m_Data[i - 1].~T();
	}

	new (&m_Data[idx]) T(std::move(value));
	++m_Size;
}

template<typename T>
template<typename... Args>
void Vector<T>::emplace(size_t idx, Args&&... args) {
	insert(idx, std::forward<Args>(args)...);
}

template<typename T>
void Vector<T>::pop_back() {
	if (m_Size == 0) {
		throw std::runtime_error("pop_back called on empty vector.");
	}

	m_Data[--m_Size].~T();
}

template<typename T>
void Vector<T>::pop_front() {
	if (m_Size == 0) {
		throw std::runtime_error("pop_front called on empty vector.");
	}
	m_Data[0].~T();
	for (size_t i = 0; i < m_Size - 1; ++i) {
		new (&m_Data[i]) T(std::move(m_Data[i + 1]));
		m_Data[i + 1].~T();
	}
	--m_Size;
}

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

template<typename T>
void Vector<T>::clear() {
	for (size_t i = 0; i < m_Size; ++i) {
		m_Data[i].~T();
	}
	m_Size = 0;
}

template<typename T>
bool Vector<T>::empty() const {
	return m_Size == 0;
}

template<typename T>
size_t Vector<T>::size() const {
	return m_Size;
}

template<typename T>
size_t Vector<T>::capacity() const {
	return m_Capacity;
}

template<typename T>
void Vector<T>::resize(size_t newSize) {
	if (newSize > m_Capacity) {
		grow();
	}
	for (size_t i = m_Size; i < newSize; ++i) {
		new (&m_Data[i]) T();
	}
	m_Size = newSize;
}

template<typename T>
void Vector<T>::grow() {
	size_t newCapacity = (m_Capacity == 0) ? 1 : m_Capacity * 2;
	T* copy = static_cast<T*>(::operator new(sizeof(T) * newCapacity));
	for (size_t i = 0; i < m_Size; ++i) {
		assert(i < m_Capacity);
		new (&copy[i]) T(std::move(m_Data[i]));
		m_Data[i].~T();
	}
	::operator delete(m_Data);
	m_Data = copy;
	m_Capacity = newCapacity;
}

template<typename T>
template<typename _T>
static void Vector<T>::swap(_T& a, _T& b) {
	_T temp = a;
	a = b;
	b = temp;
}

template<typename T, size_t N>
class Array {
private:
	T m_Array[N];

public:
	Array();

	Array(const std::initializer_list<T>& list);

	Array(const T& defVal);

	Array(const Array& other);

	Array(Array&& other) noexcept;

	Array& operator=(const Array& other);

	Array& operator=(Array&& other) noexcept;

	T& operator[](size_t index);

	const T& operator[](size_t index) const;

	T& at(size_t index);

	const T& at(size_t index) const;

	T& back();

	const T& back() const;

	T& front();

	const T& front() const;

	void fill(const T& value);

	T* data() const;

	size_t size() const;
};

template<typename T, size_t N>
Array<T, N>::Array() {
	for (size_t i = 0; i < N; ++i) {
		m_Array[i] = {};
	}
}

template<typename T, size_t N>
Array<T, N>::Array(const std::initializer_list<T>& list) {
	size_t i = 0;
	for (const T& element : list) {
		if (i < N) {
			m_Array[i++] = element;
		}
	}
}

template<typename T, size_t N>
Array<T, N>::Array(const T& defVal) {
	for (size_t i = 0; i < N; ++i) {
		m_Array[i] = defVal;
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
void Array<T, N>::fill(const T& value) {
	for (size_t i = 0; i < N; ++i) {
		m_Array[i] = value;
	}
}

template<typename T, size_t N>
T* Array<T, N>::data() const {
	return m_Array;
}

template<typename T, size_t N>
size_t Array<T, N>::size() const {
	return N;
}

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
	LinkedList();
	
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

	void remove_first(const T& value);

	void remove_all(const T& value);

	size_t find_first(const T& value) const;

	Vector<size_t> find_all(const T& value) const;

	size_t size() const;

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
};

template<typename T>
LinkedList<T>::LinkedList() {
	m_Head = nullptr;
	m_Tail = nullptr;
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
	if (m_Head == nullptr) {
		m_Head = m_Tail = new Node(value, nullptr, nullptr);
		return;
	}
	if (m_Head == m_Tail) {
		m_Tail = m_Tail->m_Next = new Node(value, m_Head, nullptr);
		return;
	}
	m_Tail = m_Tail->m_Next = new Node(value, m_Tail, nullptr);
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
	if (m_Tail == nullptr) {
		m_Tail = m_Head = new Node(value, nullptr, nullptr);
		return;
	}
	if (m_Head == m_Tail) {
		m_Head = m_Head->m_Prev = new Node(value, nullptr, m_Tail);
		return;
	}
	m_Head = m_Head->m_Prev = new Node(value, nullptr, m_Head);
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
	if (index == 0) {
		push_front(value);
		return;
	}

	Node* node = traverse_to(index);
	Node* newNode = new Node(value, node->m_Prev, node);
	if (node == m_Head) {
		m_Head = newNode;
	}
}

template<typename T>
template<typename... Args>
void LinkedList<T>::emplace(size_t index, Args&&... args) {
	if (index == 0) {
		push_front(T(std::forward<Args>(args)...));
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
	Node* node = traverse_to(index);

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
void LinkedList<T>::remove_first(const T& value) {
	Node* current = m_Head;
	size_t currentIdx = 0;
	while (current != nullptr) {
		if (current->m_Value == value) {
			erase(currentIdx);
			return;
		}
		current = current->m_Next;
		currentIdx++;
	}
}

template<typename T>
void LinkedList<T>::remove_all(const T& value) {
	Node* current = m_Head;
	size_t currentIdx = 0;
	while (current != nullptr) {
		Node* next = current->m_Next;
		if (current->m_Value == value) {
			erase(currentIdx);
		}
		else {
			currentIdx++;
		}
		current = next;
	}
}

template<typename T>
size_t LinkedList<T>::find_first(const T& value) const {
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
Vector<size_t> LinkedList<T>::find_all(const T& value) const {
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
size_t LinkedList<T>::size() const {
	Node* current = m_Head;
	size_t size = 0;
	while (current != nullptr) {
		size++;
		current = current->m_Next;
	}
	return size;
}

constexpr size_t DEFAULT_NUM_BUCKETS = 8;

template<typename KeyT, typename ValT>
class HashMap {
private:
	Vector<LinkedList<std::pair<KeyT, ValT>>> m_Buckets;
	size_t m_NumBuckets;
	size_t m_Size;

public:
	HashMap();

	HashMap(const std::initializer_list<std::pair<KeyT, ValT>>& list);

	ValT& operator[](const KeyT& key);

	const ValT& operator[](const KeyT& key) const;

	std::pair<KeyT, ValT>& insert(const KeyT& key, const ValT& val);

	void clear();

	bool empty() const;

	size_t num_buckets() const;

	size_t size() const;

private:
	float get_load_factor() const;

	Vector<std::pair<KeyT, ValT>> get_values() const;

	void rehash();

	size_t get_key_index(const KeyT& key) const;

	size_t get_key_bucket(const KeyT& key) const;
};

template<typename KeyT, typename ValT>
HashMap<KeyT, ValT>::HashMap()
	: m_Buckets(DEFAULT_NUM_BUCKETS) {
	m_Buckets.resize(DEFAULT_NUM_BUCKETS);
	m_NumBuckets = DEFAULT_NUM_BUCKETS;
	m_Size = 0;
}

template<typename KeyT, typename ValT>
HashMap<KeyT, ValT>::HashMap(const std::initializer_list<std::pair<KeyT, ValT>>& list)
	: m_Buckets(next_power_of_two(list.size())), m_NumBuckets(next_power_of_two(list.size())) {
	m_Size = list.size();
	m_Buckets.resize(m_NumBuckets);
	for (auto& pair : list) {
		size_t bucketIdx = std::hash<KeyT>{}(pair.first) % m_NumBuckets;
		m_Buckets[bucketIdx].emplace_back(pair.second);
	}
}

template<typename KeyT, typename ValT>
ValT& HashMap<KeyT, ValT>::operator[](const KeyT& key) {
	return insert(key, {}).second;
}

template<typename KeyT, typename ValT>
const ValT& HashMap<KeyT, ValT>::operator[](const KeyT& key) const {
	size_t bucketIdx = std::hash<KeyT>{}(key) % m_NumBuckets;
	auto& bucket = m_Buckets[bucketIdx];
	size_t bucketSize = bucket.size();
	size_t foundIdx = bucketSize;
	for (size_t i = 0; i < bucketSize; ++i) {
		if (bucket.at(i).first == key) {
			foundIdx = i;
			break;
		}
	}
	if (foundIdx == bucketSize) {
		throw std::runtime_error("Attempted to insert into constant hashmap.");
	}
	return bucket.at(foundIdx).second;
}

template<typename KeyT, typename ValT>
std::pair<KeyT, ValT>& HashMap<KeyT, ValT>::insert(const KeyT& key, const ValT& val) {
	size_t bucketIdx = get_key_bucket(key);
	size_t keyIdx = get_key_index(key);
	if (keyIdx == m_Buckets[bucketIdx].size()) {
		m_Size++;
		m_Buckets[bucketIdx].emplace_back(std::pair<KeyT, ValT>{ key, val });
		rehash();
		return m_Buckets[bucketIdx].back();
	}
	return m_Buckets[bucketIdx].at(keyIdx);
}

template<typename KeyT, typename ValT>
void HashMap<KeyT, ValT>::clear() {
	m_Buckets.clear();
	m_NumBuckets = DEFAULT_NUM_BUCKETS;
	m_Buckets.resize(m_NumBuckets);
	rehash();
}

template<typename KeyT, typename ValT>
bool HashMap<KeyT, ValT>::empty() const {
	return m_Size == 0;
}

template<typename KeyT, typename ValT>
size_t HashMap<KeyT, ValT>::num_buckets() const {
	return m_NumBuckets;
}


template<typename KeyT, typename ValT>
size_t HashMap<KeyT, ValT>::size() const {
	return m_Size;
}

template<typename KeyT, typename ValT>
float HashMap<KeyT, ValT>::get_load_factor() const {
	size_t total = 0;
	for (size_t i = 0; i < m_Buckets.size(); ++i) {
		total += m_Buckets[i].size();
	}
	return static_cast<float>(total) / static_cast<float>(m_NumBuckets);
}

template<typename KeyT, typename ValT>
Vector<std::pair<KeyT, ValT>> HashMap<KeyT, ValT>::get_values() const {
	Vector<std::pair<KeyT, ValT>> values;
	for (size_t i = 0; i < m_NumBuckets; ++i) {
		for (size_t j = 0; j < m_Buckets[i].size(); ++j) {
			std::pair<KeyT, ValT> pair = m_Buckets[i].at(j);
			values.emplace_back(pair.first, pair.second);
		}
	}
	return values;
}

template<typename KeyT, typename ValT>
void HashMap<KeyT, ValT>::rehash() {
	float loadFactor = get_load_factor();
	if (loadFactor > 1.0) {
		Vector<std::pair<KeyT, ValT>> values = get_values();
		m_NumBuckets *= 2;

		m_Buckets.clear();
		m_Buckets.resize(m_NumBuckets);

		for (size_t i = 0; i < values.size(); ++i) {
			size_t bucketIdx = std::hash<KeyT>{}(values[i].first) % m_NumBuckets;
			m_Buckets[bucketIdx].emplace_back(values[i].first, values[i].second);
		}
	}
	else if (loadFactor < 0.5) {
		Vector<std::pair<KeyT, ValT>> values = get_values();
		m_NumBuckets /= 2;

		m_Buckets.clear();
		m_Buckets.resize(m_NumBuckets);

		for (size_t i = 0; i < values.size(); ++i) {
			size_t bucketIdx = std::hash<KeyT>{}(values[i].first) % m_NumBuckets;
			m_Buckets[bucketIdx].emplace_back(values[i].first, values[i].second);
		}
	}
}

template<typename KeyT, typename ValT>
size_t HashMap<KeyT, ValT>::get_key_bucket(const KeyT& key) const {
	return std::hash<KeyT>{}(key) % m_NumBuckets;
}

template<typename KeyT, typename ValT>
size_t HashMap<KeyT, ValT>::get_key_index(const KeyT& key) const {
	size_t bucketIdx = get_key_bucket(key);
	size_t bucketSize = m_Buckets[bucketIdx].size();
	for (size_t i = 0; i < bucketSize; ++i) {
		if (m_Buckets[bucketIdx].at(i).first == key) {
			return i;
		}
	}
	return bucketSize;
}
