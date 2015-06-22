#include<string>
#include<iostream>
#include<vector>
#include<stack>
#include<queue>

const int ORDER = 2;			// Maximum number of elements in each node

struct Node {
	int elements[ORDER];		
	int number_of_elements;
	Node* parent;
	Node* children[ORDER + 1];
	int number_of_children;
	Node (int element) : number_of_elements(1), parent(NULL), number_of_children(0) {
		this->elements[0] = element;
		for (int i = 0; i < ORDER + 1; ++i) {
			this->children[i] = NULL;
		}
	}
};

class TwoThreeTree {
	Node* root;
public:
	TwoThreeTree (int first_element);
	void insert (int element);
	void print ();
private:
	void 			insert_element 		(int element, Node* n);
	void 			allocate_children 	(Node* parent, std::vector<Node*> children);
	std::vector<Node*>	get_left_children	(int middle, std::vector<Node*> children);
	std::vector<Node*> 	get_right_children	(int middle, std::vector<Node*> children);
	std::vector<Node*>	get_siblings 		(Node* n);
	void 			split 			(int element, Node* n, std::vector<Node*> children);
	void 			insert_aux 		(int element, Node* n);
	void 			print_aux 		(Node* n);
};

TwoThreeTree::TwoThreeTree (int first_element) {
	this->root = new Node (first_element);
}

void TwoThreeTree::insert (int element) {
	insert_aux (element, root);
}

void TwoThreeTree::insert_element (int element, Node* n) {
	if (n->elements[0] < element) {
		n->elements[1] = element;
		++n->number_of_elements;
	}
	else if (n->elements[0] > element) {
		n->elements[1] = n->elements[0];
		n->elements[0] = element;
		++n->number_of_elements;
	}
}

void TwoThreeTree::allocate_children (Node* n, std::vector<Node*> children) {
	for (int i = 0; i < children.size (); ++i) {
		Node* child = children.at (i);
		int child_largest_element = child->elements[child->number_of_elements - 1];
		if (child_largest_element < n->elements[0]) { 
			n->children[0] = child;
			child->parent = n;
		}
		else {
			if (n->number_of_elements == 1) {
				n->children[2] = child;
				child->parent = n;
			}
			else if (n->number_of_elements == 2) {
				if (child_largest_element < n->elements[1]) {
					n->children[1] = child;
					child->parent = n;
				}
				else {
					n->children[2] = child;
					child->parent = n;
				}
			}
		}
	}
	for (int i = 0; i < ORDER + 1; ++i) {
		if (n->children[i])
			++n->number_of_children;
	}
}

std::vector<Node*> TwoThreeTree::get_left_children (int middle, std::vector<Node*> children) {
	std::vector<Node*> left_children;
	for (int i = 0; i < children.size (); ++i) {
		Node* child = children.at (i);
		int child_largest_element = child->elements[child->number_of_elements - 1];
		if (child_largest_element < middle)
			left_children.push_back (child);
	}
	return left_children;
}

std::vector<Node*> TwoThreeTree::get_right_children (int middle, std::vector<Node*> children) {
	std::vector<Node*> right_children;
	for (int i = 0; i < children.size (); ++i) {
		Node* child = children.at (i);
		int child_smallest_element = child->elements[0];
		if (child_smallest_element > middle)
			right_children.push_back (child);
	}
	return right_children;
}

std::vector<Node*> TwoThreeTree::get_siblings (Node* n) {
	std::vector<Node*> siblings;
	if (n->parent != NULL) { 
		for (int i = 0; i < ORDER + 1; ++i) {
			if (n->parent->children[i] != n && n->parent->children[i] != NULL)
				siblings.push_back (n->parent->children[i]);
		}
	}
	return siblings;
}

void TwoThreeTree::split (int element, Node* n, std::vector<Node*> children) {
	// at the top
	if (n == NULL) {
		root = new Node (element);
		TwoThreeTree::allocate_children (root, children);
		return;
	}
	// encountered an unfilled node
	if (n->number_of_elements < 2) {
		TwoThreeTree::insert_element (element, n);
		TwoThreeTree::allocate_children (n, children);
		return;
	}
	// is not at the top, and the current node is full
	std::vector<Node*> siblings = TwoThreeTree::get_siblings (n);
	int smallest = (element < n->elements[0])? element : n->elements[0];
	int middle = (element < n->elements[0])? n->elements[0] : ((element < n->elements[1])? element : n->elements[1]);
	int largest = (element > n->elements[1])? element : n->elements[1];
//std::cout << "smallest : " << smallest << ", middle : " << middle << ", largest : " << largest << std::endl;
//std::cout << "siblings : " << siblings.size () << std::endl;
	Node* left_node = new Node (smallest);
	Node* right_node = new Node (largest);
	// recursive split 
	if (n->number_of_children != 0) {
		TwoThreeTree::allocate_children (left_node, TwoThreeTree::get_left_children (middle, children));
		TwoThreeTree::allocate_children (right_node, TwoThreeTree::get_right_children (middle, children));
	}
	siblings.push_back (left_node);
	siblings.push_back (right_node);
	TwoThreeTree::split (middle, n->parent, siblings);
}

void TwoThreeTree::insert_aux (int element, Node* n) {
	// if the node is a leaf node
	if (n->number_of_children == 0) {
		// if the leaf node has only one element
		if (n->number_of_elements == 1) {
			insert_element (element, n);
		}
		// if the leaf node has two element
		else if (n->number_of_elements == 2) {
			std::vector<Node*> empty;
			split (element, n, empty);
		}
	} 
	else {
		if (element < n->elements[0])
			insert_aux (element, n->children[0]);
		else {
			if (n->number_of_elements == 1) 
				insert_aux (element, n->children[2]);
			else if (n->number_of_elements == 2) {
				if (element > n->elements[1])
					insert_aux (element, n->children[2]);
				else
					insert_aux (element, n->children[1]);
			}
		}
	}
}

void TwoThreeTree::print_aux (Node* n) {
	std::cout << "Elements:";
	for (int i = 0; i < n->number_of_elements; ++i) {
		std::cout << " " << n->elements[i] << " ";
	}
	if (n->children[0]) {
		std::cout << " left ";
	}
	if (n->children[1]) {
		std::cout << " middle ";
	}
	if (n->children[2]) {
		std::cout << " right ";
	}
	
	std::cout << " number of children: " << n->number_of_children <<  std::endl;
}

void TwoThreeTree::print () {
	std::queue<Node*> q;
	q.push (root);
	while (!(q.empty())) {
		Node* front = q.front ();
		q.pop ();
		TwoThreeTree::print_aux (front);
		for (int i = 0; i < ORDER + 1; ++i) {
			if (front->children[i])
				q.push (front->children[i]);
		}
	}
}

int main () {
	TwoThreeTree* tree = new TwoThreeTree (5);
	tree->insert (6);
	tree->insert (7);
	tree->insert (4);
	tree->insert (3);
	tree->insert (2);
	tree->insert (1);
	tree->insert (8);
	tree->insert (9);
	tree->print ();
	return 0;
}
