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
/// General node structure, used to polymorphically link all node types
/// </summary>
struct Node
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
};

/// <summary>
/// General data node structure used to polymorphically
/// link all node types containing a single data value
/// </summary>
template<typename T>
struct DataNode : public Node
{
	/// <summary>Data held inside of a node</summary>
	T data;
};

/// <summary>
/// Inner node structure used to represent a single non-leaf
/// node of a tree
/// </summary>
template<typename T>
struct InnerNode : public DataNode<T>
{
	/// <summary>The color of a node. Can be black, red or doubly black</summary>
	Color color;

	/// <summary>
	/// Initializes the node with given data and color.
	/// Parent, left and right pointers are set to nullptr.
	/// </summary>
	/// <param name="data">Data to be inserted in the node</param>
	/// <param name="color">Value to be set as the nodes color</param>
	InnerNode(T data, Color color) :
		data{ data }, color{ color }, parent{ nullptr }, left{ nullptr }, right{ nullptr } {}

	/// <summary>
	/// Initializes the node with given data.
	/// Color is set to black.
	/// Parent, left and right pointers are set to nullptr.
	/// </summary>
	/// <param name="data">Data to be inserted in the node</param>
	InnerNode(T data) :
		data{ data }, color{ Color::BLACK }, parent{ nullptr }, left{ nullptr }, right{ nullptr } {}
};

/// <summary>
/// Bucket node structure used to represent a single leaf
/// node of a tree, containing a doubly lined list of data nodes
/// </summary>
template<typename T>
struct BucketNode : public Node
{
	/// <summary>The beging of the data linked list</summary>
	Node *first;
	/// <summary>The middle of the data linked list</summary>
	/// <remarks>Can point to a node above the middle</remarks>
	Node *middle;
	/// <summary>The end of the data linked list</summary>
	Node *last;

	/// <summary>The total size of the list given</summary>
	unsigned int size;
	/// <summary>Number of nodes above the middle one</summary>
	unsigned int up_size;
	/// <summary>Number of nodes below the middle one</summary>
	unsigned int down_size;

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
	BucketNode(Node* first, Node* middle, Node* last, unsigned int size, unsigned int up_size, unsigned int down_size) :
		parent{ nullptr }, left{ nullptr }, right{ nullptr },
		first{ first }, middle{ middle }, last{ last },
		size{ size }, up_size{ up_size }, down_size{ down_size } {}

	/// <summary>
	/// Initializes the node with a given list.
	/// All pointers are set to nullptr.
	/// All sizes are set to 0.
	/// </summary>
	BucketNode() :
		parent{ nullptr }, left{ nullptr }, right{ nullptr },
		first{ nullptr }, middle{ nullptr }, last{ nullptr },
		size{ 0 }, up_size{ 0 }, down_size{ 0 } {}
};
