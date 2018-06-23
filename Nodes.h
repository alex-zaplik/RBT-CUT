#pragma once

/// <summary>
/// All possible inner node colors
/// </summary>
enum Color : char
{
	/// <summary>Node color</summary>
	RED, BLACK, DOUBLE_BLACK
};

/// <summary>
/// Used to add the color parameter to nodes
/// </summary>
struct ColoredNode
{
	/// <summary>The color of a node. Can be black, red or doubly black</summary>
	Color color;

	ColoredNode(Color color) : color{ color } {}
};

/// <summary>
/// General data node structure used to polymorphically
/// link all node types containing a single data value.
/// Also used for in-bucket nodes.
/// </summary>
template<typename T>
struct Node : public ColoredNode
{
	union
	{
		struct
		{
			/// <summary>Used for representing inner nodes and buckets</summary>
			Node *parent, *left, *right;
		};

		struct
		{
			/// <summary>Used for representing nodes inside buckets</summary>
			Node *prev, *next, *bucket;
		};
	};

	/// <summary>Data held inside of a node</summary>
	T data;

	/// <summary>
	/// Initializes the node with given data.
	/// Prev, next and bucket pointers are set to nullptr.
	/// </summary>
	/// <param name="data">Data to be inserted in the node</param>
	Node(T data) : 
		parent{ nullptr }, left{ nullptr }, right{ nullptr },
		data{ data }, ColoredNode(Color::BLACK) {}

	/// <summary>
	/// Initializes the node with given data.
	/// Prev, next and bucket pointers are set to nullptr.
	/// </summary>
	/// <param name="data">Data to be inserted in the node</param>
	/// <param name="color">Color to be to be set to the node</param>
	Node(T data, Color color) : 
		parent{ nullptr }, left{ nullptr }, right{ nullptr },
		data{ data }, ColoredNode(color) {}

	/// <summary>
	/// Initializes the node with given data.
	/// Prev, next and bucket pointers are set to nullptr.
	/// </summary>
	/// <param name="color">Color to be to be set to the node</param>
	Node(Color color) : 
		parent{ nullptr }, left{ nullptr }, right{ nullptr },
		data{ }, ColoredNode(color) {}

	virtual ~Node() {}
};

/// <summary>
/// Inner node structure used to represent a single non-leaf
/// node of a tree
/// </summary>
template<typename T>
struct InnerNode : public Node<T>
{
	/// <summary>
	/// Initializes the node with given data and color.
	/// Parent, left and right pointers are set to nullptr.
	/// </summary>
	/// <param name="data">Data to be inserted in the node</param>
	/// <param name="color">Value to be set as the nodes color</param>
	InnerNode(T data, Color color) :
		Node<T>(data, color) {}

	/// <summary>
	/// Initializes the node with given data.
	/// Color is set to black.
	/// Parent, left and right pointers are set to nullptr.
	/// </summary>
	/// <param name="data">Data to be inserted in the node</param>
	InnerNode(T data) :
		Node<T>(data, Color::BLACK) {}
};

/// <summary>
/// Bucket node structure used to represent a single leaf
/// node of a tree, containing a doubly lined list of data nodes
/// </summary>
template<typename T>
struct BucketNode : public Node<T>
{
	/// <summary>The beging of the data linked list</summary>
	Node<T> *first;
	/// <summary>The middle of the data linked list</summary>
	/// <remarks>Can point to a node above the middle</remarks>
	Node<T> *middle;
	/// <summary>The end of the data linked list</summary>
	Node<T> *last;

	/// <summary>The total size of the list given</summary>
	unsigned int size;
	/// <summary>Number of nodes above the middle one</summary>
	unsigned int up_size;
	/// <summary>Number of nodes below the middle one</summary>
	unsigned int down_size;

	/// <summary>Next bucket to be fixed in global fixing</summary>
	BucketNode* next_bucket;
	/// <summary>Precious bucket in the fixing queue</summary>
	BucketNode* prev_bucket;
	/// <summary>Pointer to the node up to which fixing has been done</summary>
	Node<T>* fix_pointer;

	/// <summary>
	/// Default constructor.
	/// Parent, left and right pointers are set to nullptr.
	/// </summary>
	/// <param name="first">The beging of the data linked list</param>
	/// <param name="middle">The middle of the data linked list or a node above the middle</param>
	/// <param name="last">The end of the data linked list</param>
	/// <param name="size">The total size of the list given</param>
	/// <param name="up_size">Number of nodes above the middle one</param>
	/// <param name="down_size">Number of nodes below the middle one</param>
	BucketNode(Node<T>* first, Node<T>* middle, Node<T>* last, unsigned int size, unsigned int up_size, unsigned int down_size) :
		first{ first }, middle{ middle }, last{ last },
		fix_pointer{ nullptr }, next_bucket{ this }, prev_bucket{ this },
		size{ size }, up_size{ up_size }, down_size{ down_size }, Node<T>(Color::BLACK) {}

	/// <summary>
	/// Initializes the node with a given list.
	/// All pointers are set to nullptr.
	/// All sizes are set to 0.
	/// </summary>
	BucketNode() noexcept :
		first{ nullptr }, middle{ nullptr }, last{ nullptr },
		fix_pointer{ nullptr }, next_bucket{ this }, prev_bucket{ this },
		size{ 0 }, up_size{ 0 }, down_size{ 0 }, Node<T>(Color::BLACK) {}

	/// <summary>
	/// An ostream overload for printing the structure as a string
	/// </summary>
	friend std::ostream& operator<< (std::ostream& stream, const BucketNode& bn) {
		stream << "{";

		Node<T>* dn = bn.first;
		while (dn != nullptr)
		{
			if (bn.first == dn) stream << "<";
			if (bn.middle == dn) stream << "[";
			if (bn.last == dn) stream << "(";

			stream << dn->data;

			if (bn.last == dn) stream << ")";
			if (bn.middle == dn) stream << "]";
			if (bn.first == dn) stream << ">";

			if (dn->next != nullptr)
			{
				stream << ", ";
			}
			dn = dn->next;
		}

		stream << "}";

		return stream;
	}
};
