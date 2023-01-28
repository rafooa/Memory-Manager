#include <iostream>
#include <string.h>
using namespace std;

class Helper
{
public:
	static int StringLength(const char* str)
	{
		int i;
		for (i = 0; str[i] != '\0'; i++);
		return i;
	}

	static void StringCopy(char*& dest, const char*& src) {
		//Deep Copies src into dest.
		int size = StringLength(src), i;
		for (i = 0; i < size; i++)
			dest[i] = src[i];
		dest[i] = '\0';
	}
};

class block
{
private:
	int start_byte_ID;
	int total_bytes;

public:
	block() // default constructor
	{
		start_byte_ID = 0;
		total_bytes = 0;
	}

	block(int start, int total) // parameterized constructor
	{
		start_byte_ID = start;
		total_bytes = total;
	}

	int getStart() {
		return start_byte_ID;
	}

	int getTotal() {
		return total_bytes;
	}

	void setStart(int s) {
		start_byte_ID = s;
	}

	void setTotal(int t) {
		total_bytes = t;
	}

	void print()
	{
		cout << "Starting Byte ID: \t" << start_byte_ID << endl;
		cout << "Total Bytes: \t\t" << total_bytes << endl << endl;
	}
};

template <class type>
class List
{
private:
	// nested node class
	class Node
	{
	public:
		Node() { next = NULL; }
		Node(type val, Node* nptr = 0) {
			data = val;
			next = nptr;
		}
		type data;
		Node* next;
	};
	class ListIterator {        // Nested List Iterator class
	public:
		ListIterator(Node* ptr = NULL)
		{
			iptr = ptr;
		}

		ListIterator& operator++() {// pre fix 
			if (iptr)
				iptr = iptr->next;
			return (*this);
		}

		ListIterator& operator++(int) {// post fix 
			ListIterator old = *this;
			++(*this);
			return old;
		}

		bool operator==(const ListIterator& l)
		{
			return iptr == l.iptr;
		}

		bool operator!=(const ListIterator& l)
		{
			return !(iptr == l.iptr);
		}

		type& operator*()
		{
			return iptr->data;
		}

		ListIterator& operator+(int num)
		{
			ListIterator old = *this;
			for (int i = 0; i < num; i++)
				old.iptr = old.iptr->next;

			return old;
		}


	private:
		friend class List;
		Node* iptr;
	};
	// list members
	Node* head;
	Node* tail;
	int size;

public:
	typedef ListIterator Iterator;
	Iterator begin()
	{
		Iterator I(head->next);
		return I;
	}

	Iterator end()
	{
		Iterator I(tail);
		return I;
	}

	List()
	{
		size = 0;
		// dummy nodes
		type temp;
		tail = new Node(temp);
		head = new Node(temp, tail);
	}

	void insertAtStart(type& const element)
	{
		head->next = new Node(element, head->next);
		size++;
	}

	void insertAtEnd(type& const element)
	{
		Node* temp = head;
		while (temp->next != tail)
		{
			temp = temp->next;
		}
		temp->next = new Node(element, tail);
		size++;
	}

	void insertIterator(Iterator i)
	{
		Node* temp = head;
		Node* toAdd = i.iptr;

		while (((toAdd->data).getStart() + (toAdd->data).getTotal()) > (temp->next->data).getStart())
		{
			temp = temp->next;
		}
		
		temp->next = new Node(*(i), temp->next);
	}

	void deleteIterator(Iterator& i)
	{
		Node* toDelete = i.iptr;
		Node* temp = head;

		while (temp->next != toDelete)
		{
			temp = temp->next;
		}
		temp->next = toDelete->next;
		delete toDelete;
	}

	void deleteAtStart()
	{
		if (head->next != NULL)
		{
			if (head->next != tail)
			{
				Node* temp = head->next;
				head->next = head->next->next;
				delete temp;
				size--;
			}
		}
		else
			cout << "List is empty.\n";
	}

	~List()
	{
	}
};

class Program
{
private:
	char* ID;
	int size;
	List<block> blocks;

public:
	Program()
	{
		ID = 0;
		size = 0;
	}

	Program(const char* _id, int _size)
	{
		ID = new char[Helper::StringLength(_id)];
		Helper::StringCopy(ID, _id);
		size = _size;
	}

	void print()
	{
		cout << "Program ID: \t\t" << ID << endl;
		cout << "Total Program Size: \t" << size << endl;
		cout << "Program List of Blocks: \n";
		List<block>::Iterator it;

		for (it = blocks.begin(); it != blocks.end(); it++)
		{
			(*(it)).print();
		}
	}

	List<block>& returnBlocks()
	{
		List<block>* toReturn = new List<block>;
		*toReturn = this->blocks;
		return *toReturn;
	}

	void addBlockToProgram(block& temp)
	{
		blocks.insertAtEnd(temp);
		size = size + temp.getTotal();
	}

	const char* getID() {
		return ID;
	}

	bool operator==(const char* rhs)
	{
		return strcmp(ID, rhs);
	}
};

class MemoryManagementSystem
{
private:
	List<block> pool;
	List<Program> progs;
	int sizeOfMemory;
	bool strategy;

public:
	MemoryManagementSystem(int memSize, bool strat)
	{
		sizeOfMemory = memSize;
		strategy = strat;
	}

	void addBlockToPool(int start, int total)
	{
		block temp(start, total);
		pool.insertAtEnd(temp);
	}

	bool getMem(const char* progID, int memSize)
	{
		bool available = true, found = false;
		int start = -1;
		List<block>::Iterator it1, temp;
		List<Program>::Iterator it2;

		if (memSize <= sizeOfMemory)
		{
			if (strategy == 1) // find starting byte in first fit case
			{
				for (it1 = pool.begin(); it1 != pool.end(); it1++)
				{
					if ((*(it1)).getTotal() >= memSize)
					{
						start = (*(it1)).getStart();
						break;
					}
				}
				if (start == -1) // terminate if no space found
					return false;

				//Update Pool Block values
				(*(it1)).setStart((*(it1)).getStart() + memSize);
				(*(it1)).setTotal((*(it1)).getTotal() - memSize);

			}
			else if (strategy == 0) // find starting byte in best fit case
			{
				start = sizeOfMemory;
				int total = start;
				for (it1 = pool.begin(); it1 != pool.end(); it1++)
				{
					if ((*(it1)).getTotal() >= memSize && (*(it1)).getTotal() <= total)
					{
						start = (*(it1)).getStart();
						total = (*(it1)).getTotal();
						temp = it1;
					}
				}
				if (start == sizeOfMemory) //terminate if no space found
					return false;

				//Update Pool Block values
				it1 = temp;
				(*(it1)).setStart((*(it1)).getStart() + memSize);
				(*(it1)).setTotal((*(it1)).getTotal() - memSize);


			}

			// create new block based on starting byte that was found
			block temp(start, memSize);

			// if program already exists
			for (it2 = progs.begin(); it2 != progs.end(); it2++)
			{
				if (!(strcmp((*(it2)).getID(), progID)))
				{
					(*(it2)).addBlockToProgram(temp);
					found = true;
					break;
				}
			}

			// if program is brand new
			if (found == false)
			{
				Program prog(progID, 0);
				prog.addBlockToProgram(temp);
				progs.insertAtEnd(prog);
			}
		}
		else
			available = false;

		return available;
	}

	bool DeleteProgram(const char* progID)
	{
		bool found = false;
		List<Program>::Iterator it;
		
		// traverse progs to find program
		for (it = progs.begin(); it != progs.end(); it++)
		{
			if (!(strcmp((*(it)).getID(), progID)))
			{
				found = true;
				break;
			}
		}

		if (found == false)
			return found;
		else
		{
			// reallocate memory to pool first, then delete the program
			List<block> temp = (*(it)).returnBlocks();
			progs.deleteIterator(it);
			List<block>::Iterator it1;
			// Insert returned blocks into appropriate positions of pool
			for (it1 = temp.begin(); it1 != temp.end(); it1++)
			{
				pool.insertIterator(it1);
			}
			// merge adjacent blocks in pool
			for (it1 = pool.begin(); it1 != pool.end();)
			{
				if (((*(it1)).getStart() + (*(it1)).getTotal()) == (*(it1 + 1)).getStart())
				{
					(*(it1)).setTotal((*(it1)).getTotal() + (*(it1 + 1)).getTotal());
					pool.deleteIterator(it1 + 1);
				}
				else
					it1++;
			}
		}
	}

	void printMemoryPool()
	{
		cout << "Current Pool: \n";
		List<block>::Iterator it;

		for (it = pool.begin(); it != pool.end(); it++)
		{
			(*(it)).print();
		}
	}

	void printProgs()
	{
		cout << "Current Progs: \n";
		List<Program>::Iterator it;

		for (it = progs.begin(); it != progs.end(); it++)
		{
			(*(it)).print();
		}
	}
};

int main()
{
	bool strategy = false;
	bool run = true;
	int RAM = 0;
	int input = 0;

	cout << "Enter True (1) for First Come Strategy && False (0) for Best Fit Strategy" << endl;
	cin >> strategy;
	cout << endl;

	cout << "Enter RAM size" << endl;
	cin >> RAM;

	MemoryManagementSystem object(RAM, strategy);
	object.addBlockToPool(0, RAM);

	while (run == true)
	{
		cout << endl;
		cout << "Press 1 to enter a program" << endl;
		cout << "Press 2 to delete a program" << endl;
		cout << "Press 3 to view the Pool (Memory of free blocks)" << endl;
		cout << "Press 4 to view the Programs in Memory" << endl;
		cout << "Press 5 to exit the program" << endl;
		cout << endl;

		cin >> input;

		if (input == 1)
		{
			char* programId = new char[8];
			int memoryRequired = 0;
			cout << "Enter Program ID" << endl;
			cin >> programId;
			cout << "Enter memory required for the program" << endl;
			cin >> memoryRequired;
			if (object.getMem(programId, memoryRequired))
			{
				cout << "Memory succesfully allocated" << endl;
			}
			else
			{
				cout << "Memory not available" << endl;
			}
		}
		else if (input == 2)
		{
			char* programId = new char[8];
			cout << "Enter Program ID which you wish to delete for eg P1" << endl;
			cin >> programId;
			if (object.DeleteProgram(programId))
			{
				cout << "Memory succesfully deallocated" << endl;
			}
			else
			{
				cout << "Memory can't be deallocated. Program does not exist" << endl;
			}
		}
		else if (input == 3)
		{
			object.printMemoryPool();
		}
		else if (input == 4)
		{
			object.printProgs();
		}
		else if (input == 5)
		{
			run = false;
			cout << "Program exited." << endl;
		}
		else
		{
			cout << "Invalid Input. Enter again" << endl;
		}
	}
}