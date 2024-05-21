#pragma once

// struct representing a single linked-list element. in a more advanced implementation, each linked-list element would actually store a block of real data elements to reduce overhead
template <typename T>
struct LinkedListElement
{
	LinkedListElement* next = NULL;
	T data;
};

// class for storing an array of objects as a linked list. templated for your pleasure
template <typename T>
class LinkedList
{
private:
	size_t length;				// number of elements in the array
	LinkedListElement<T>* head; // element -1. this should always exist
	LinkedListElement<T>* tail;	// last element in the array (element length-1)
public:
	// append an element to the end of the array
	void pushBack(T value)
	{
		tail->next = new LinkedListElement<T>{ NULL, value };
		tail = tail->next;
		length++;
	}

	// access an element in the array
	T& operator[](size_t index)
	{
		if (index >= length) throw "index out of range";
		LinkedListElement<T>* element = head->next;
		size_t offset = 0;
		while (offset < index)
		{
			element = element->next;
			offset++;
		}
		return element->data;
	}

	// remove the last item from the end of the array
	T popBack()
	{
		return removeAt(length - 1);
	}

	// remove an item at a specified index
	T removeAt(size_t index)
	{
		if (index >= length) throw "index out of range";
		
		LinkedListElement<T>* item_before = head;
		size_t offset = 0;
		while (offset <= index - 1 && index > 0)
		{
			item_before = item_before->next;
			offset++;
		}

		LinkedListElement<T>* item = item_before->next;
		T value = item->data;
		LinkedListElement<T>* next = item->next;

		delete item;
		item_before->next = next;
		length--;

		if (index == length)
			tail = item_before;

		return value;
	}

	// get the number of elements in the list
	size_t getLength()
	{
		return length;
	}

	// initialiser
	LinkedList()
	{
		length = 0;
		head = new LinkedListElement<T>{ NULL };
		tail = head;
	}

	// other initialisers are deleted, since they would break things

	LinkedList(const LinkedList& other) = delete;
	LinkedList(const LinkedList&& other) = delete;
	void operator=(const LinkedList& other) = delete;
	void operator=(const LinkedList&& other) = delete;

	~LinkedList()
	{
		delete head;
	}
};