#pragma once

template <typename T>
struct LinkedListElement
{
	LinkedListElement* next = NULL;
	T data;
};

template <typename T>
class LinkedList
{
private:
	size_t length;
	LinkedListElement<T>* head;
	LinkedListElement<T>* tail;
public:
	void pushBack(T value)
	{
		tail->next = new LinkedListElement<T>{ NULL, value };
		tail = tail->next;
		length++;
	}

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

	T popBack()
	{
		return removeAt(length - 1);
	}

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

	size_t getLength()
	{
		return length;
	}

	LinkedList()
	{
		length = 0;
		head = new LinkedListElement<T>{ NULL };
		tail = head;
	}

	LinkedList(const LinkedList& other) = delete;
	LinkedList(const LinkedList&& other) = delete;
	void operator=(const LinkedList& other) = delete;
	void operator=(const LinkedList&& other) = delete;
};