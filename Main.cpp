#include <iostream>

template<typename T, size_t N>
class Array {
private:
	T m_Data[N];

public:
	Array() {
		for (size_t i = 0; i < N; ++i) {
			m_Data[i] = {};
		}
	}

	Array(const std::initializer_list<T>& values) {
		size_t idx = 0;
		for (const T& value : values) {
			m_Data[idx++] = value;

			if (idx >= N) {
				break;
			}
		}
	}

	T& operator[](size_t idx) {
		return m_Data[idx];
	}

	const T& operator[](size_t idx) const {
		return m_Data[idx];
	}

	T& At(size_t idx) {
		if (idx >= N) {
			throw std::out_of_range("Array Error: Index given to At() is greater than size of array.");
		}
		else if (idx < 0) {
			throw std::out_of_range("Array Error: Index given to At() is less than zero.");
		}
		return m_Data[idx];
	}

	const T& At(size_t idx) const {
		if (idx >= N) {
			throw std::out_of_range("Array Error: Index given to At() is greater than size of array.");
		}
		else if (idx < 0) {
			throw std::out_of_range("Array Error: Index given to At() is less than zero.");
		}
		return m_Data[idx];
	}

	T& Back() {
		return m_Data[N - 1];
	}

	const T& Back() const {
		return m_Data[N - 1];
	}

	size_t Size() const {
		return N;
	}
};

template<typename T>
class Vector {
private:
	T* m_Data;
	size_t m_Capacity;
	size_t m_Size;

public:
	Vector()
		: m_Data(nullptr), m_Capacity(0), m_Size(0) {
	}

	Vector(const std::initializer_list<T>& values) {
		GrowCapacityToFit(values.size());
		m_Size = values.size();
		m_Data = new T[m_Capacity];
		size_t idx = 0;

		for (const T& value : values) {
			// This will never happen, but intellisense gives C6385 warnings without the check.
			if (idx >= m_Capacity) {
				throw std::out_of_range("Vector Error: Index greater than capacity.");
			}
			m_Data[idx++] = value;
		}
	}

	Vector(size_t capacity)
		: m_Data(new T[capacity]), m_Capacity(capacity), m_Size(0) {
	}

	T& operator[](size_t idx) {
		return m_Data[idx];
	}

	const T& operator[](size_t idx) const {
		return m_Data[idx];
	}

	T& At(size_t idx) {
		if (idx >= m_Size || idx < 0) {
			throw std::out_of_range("Vector Error: Index given to At() is out of bounds.");
		}
		return m_Data[idx];
	}

	const T& At(size_t idx) const {
		if (idx >= m_Size || idx < 0) {
			throw std::out_of_range("Vector Error: Index given to At() is out of bounds.");
		}
		return m_Data[idx];
	}

	void PushBack(const T& value) {
		if (m_Size + 1 > m_Capacity) {
			Grow();
		}
		m_Data[m_Size++] = value;
	}

	void PushFront(const T& value) {
		if (m_Size + 1 > m_Capacity) {
			Grow();
		}
		++m_Size;
		for (size_t i = m_Size - 1; i > 0; --i) {
			m_Data[i] = m_Data[i - 1];
		}
		m_Data[0] = value;
	}

	T PopBack() {
		if (m_Size == 0) {
			throw std::out_of_range("Vector Error: PopBack() called when size is equal to 0.");
		}
		return std::move(m_Data[--m_Size]);
	}

	T PopFront() {
		T value = m_Data[0];
		for (size_t i = 0; i < m_Size - 1; ++i) {
			m_Data[i] = std::move(m_Data[i + 1]);
		}
		--m_Size;
		return value;
	}

	void RemoveAt(size_t idx) {
		if (idx >= m_Size || idx < 0) {
			throw std::out_of_range("Vector Error: Index given to RemoveAt() is out of bounds.");
		}
		for (size_t i = idx; i < m_Size - 1; ++i) {
			m_Data[i] = std::move(m_Data[i + 1]);
		}
		--m_Size;
	}

	T& Back() {
		if (m_Size == 0) {
			throw std::out_of_range("Vector Error: Back() called when size is equal to 0.");
		}
		return m_Data[m_Size - 1];
	}

	const T& Back() const {
		if (m_Size == 0) {
			throw std::out_of_range("Vector Error: Back() called when size is equal to 0.");
		}
		return m_Data[m_Size - 1];
	}

	T& Front() {
		if (m_Size == 0) {
			throw std::out_of_range("Vector Error: Front() called when size is equal to 0.");
		}
		return m_Data[0];
	}

	const T& Front() const {
		if (m_Size == 0) {
			throw std::out_of_range("Vector Error: Front() called when size is equal to 0.");
		}
		return m_Data[0];
	}

	bool Empty() const {
		return m_Size == 0;
	}

	size_t Size() const {
		return m_Size;
	}

	size_t Capacity() const {
		return m_Capacity;
	}

private:
	void Grow() {
		m_Capacity = (m_Capacity == 0) ? 1 : m_Capacity * 2;
		T* copy = new T[m_Capacity];
		for (size_t i = 0; i < m_Size; ++i) {
			// This will never happen, but intellisense gives C6385 and C6386 warnings without the check.
			if (i >= m_Capacity) {
				throw std::out_of_range("Vector Error: Index greater than capacity.");
			}
			copy[i] = m_Data[i];
		}
		delete[] m_Data;
		m_Data = copy;
	}

	void GrowCapacityToFit(size_t required) {
		if (m_Capacity <= 0 && required > 0) {
			m_Capacity = 1;
		}
		while (m_Capacity < required) {
			m_Capacity *= 2;
		}
	}
};

template<typename KeyT, typename ValT>
class Map {
private:
	Vector<KeyT> m_Keys;
	Vector<ValT> m_Values;

public:
	Map() = default;

	Map(const std::initializer_list<std::pair<KeyT, ValT>>& values) {
		for (const auto& [key, value] : values) {
			m_Keys.PushBack(key);
			m_Values.PushBack(value);
		}
	}

	ValT& operator[](const KeyT& key) {
		size_t keyNum = FindKey(key);
		if (keyNum == m_Keys.Size()) {
			m_Keys.PushBack(key);
			m_Values.PushBack(ValT{});
		}
		return m_Values[FindKey(key)];
	}

	const ValT& operator[](const KeyT& key) const {
		size_t keyNum = FindKey(key);
		if (keyNum == m_Keys.Size()) {
			throw std::runtime_error("Map Error: Attempted to add a value to a constant Map.");
		}
		return m_Values[FindKey(key)];
	}

	ValT& At(const KeyT& key) {
		size_t keyNum = FindKey(key);
		if (keyNum == m_Keys.Size()) {
			m_Keys.PushBack(key);
			m_Values.PushBack(ValT{});
		}
		return m_Values[FindKey(key)];
	}

	const ValT& At(const KeyT& key) const {
		size_t keyNum = FindKey(key);
		if (keyNum == m_Keys.Size()) {
			throw std::runtime_error("Map Error: Attempted to add a value to a constant Map.");
		}
		return m_Values[FindKey(key)];
	}

	ValT& Back() {
		return m_Values[m_Values.Size() - 1];
	}

	const ValT& Back() const {
		return m_Values[m_Values.Size() - 1];
	}

	ValT& Front() {
		return m_Values[0];
	}

	const ValT& Front() const {
		return m_Values[0];
	}

	KeyT GetKey(size_t index) const {
		return m_Keys[index];
	}

	ValT GetValue(size_t index) const {
		return m_Values[index];
	}

	bool Empty() const {
		if (m_Keys.Size() != m_Values.Size()) {
			throw std::runtime_error("Map Error: Key vector size does not match value vector size.");
		}
		return m_Keys.Empty() && m_Values.Empty();
	}

	size_t Size() const {
		if (m_Keys.Size() != m_Values.Size()) {
			throw std::runtime_error("Map Error: Key vector size does not match value vector size.");
		}
		return m_Keys.Size();
	}

private:
	size_t FindKey(const KeyT& key) const {
		for (size_t i = 0; i < m_Keys.Size(); ++i) {
			if (m_Keys[i] == key) {
				return i;
			}
		}
		return m_Keys.Size();
	}
};

template<typename T>
class Stack {
private:
	Vector<T> m_StackData;

public:
	Stack() = default;

	Stack(const std::initializer_list<T>& values) {
		for (const T& value : values) {
			m_StackData.PushBack(value);
		}
	}

	const T& operator[](size_t idx) const {
		return m_StackData[idx];
	}

	const T& At(size_t idx) const {
		if (idx >= m_StackData.Size() || idx < 0) {
			throw std::out_of_range("Stack Error: Index given to At() is out of range.");
		}
		return m_StackData[idx];
	}

	void Push(const T& value) {
		m_StackData.PushBack(value);
	}

	T Pop() {
		return std::move(m_StackData.PopBack());
	}

	bool Empty() const {
		return m_StackData.Empty();
	}

	size_t Size() const {
		return m_StackData.Size();
	}
};

template<typename T>
class Queue {
private:
	Vector<T> m_QueueData;

public:
	Queue() = default;

	Queue(const std::initializer_list<T>& values) {
		for (const T& value : values) {
			m_QueueData.PushBack(value);
		}
	}

	void Push(const T& value) {
		m_QueueData.PushBack(value);
	}

	T Pop() {
		return m_QueueData.PopFront();
	}

	T& Back() {
		return m_QueueData.Back();
	}

	const T& Back() const {
		return m_QueueData.Back();
	}

	T& Front() {
		return m_QueueData.Front();
	}

	const T& Front() const {
		return m_QueueData.Front();
	}

	bool Empty() const {
		return m_QueueData.Empty();
	}

	size_t Size() const {
		return m_QueueData.Size();
	}
};

int main() {

}
