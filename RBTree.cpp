#include <iostream>
using namespace std;

struct Node { // ���� �����͸� ��� ��� ���� 
	int ID, Size, Price;
	string Name;
	Node* left;
	Node* right;
	Node* parent;
	bool isBlack; // true�� black, false�� red
};

class RBtree {
public:
	Node* root;
	Node* Nil; // Nil�� �ƹ��͵� ���� ������ ��带 �ǹ�. ������ �����ϴ� ���� �ƴϰ� 
	// ���԰������� ����, ���� �Ǻ��ϱ� ���� ����ϴ� Null node�� �ǹ��Ѵ�. 
	// ���� root�� leaf�� �ϳ��� Nil�� ����Ű���� ���� 

	RBtree() {
		Nil = new Node();
		Nil->left = nullptr;
		Nil->right = nullptr;
		Nil->parent = nullptr;
		Nil->isBlack = true;
		root = Nil;
	}

	void leftRotate(Node* node); // node�� �������� ����ȸ��
	void rightRotate(Node* node); // node�� �������� ������ȸ��
	void restructure(Node* node); // retstrucure
	int findDepth(int id); // node�� ���� Ž��

	void insert(int id, string name, int size, int price);
	void search(int id);
	void update(int id, string name, int s, int p);
	void discount(Node* R, int x, int y, double p);

	void preorder(Node* R);
	
};
/*       LEFT ROTATE
   GP!                GP!            GP!
	G                  G              P
   / \                / \   P        / \
  U!  P      -->     U!  X!    -->  G
      /                            / \
     X!	                          U!  X!
���� 1-1. X�� G�� �ڽ�����,		1-2. G�� X�� �θ�� 
���� 2-1. P�� GP�� �ڽ�����,		2-2. GP�� P�� �θ�� 
���� 3-1. G�� P�� �ڽ�����,		3-2. P�� G�� �θ��
!�� Nil�̾ ������� ���,     �θ���� ������ Nil�̸� ���� x
*/
// grand��带 �������� ���� ȸ��
void RBtree::leftRotate(Node* G) {
	if (G->right == Nil) return;
	Node* P = G->right; // P����
	Node* X = P->left; // X ����
	if (X != Nil) {
		X->parent = G; // G�� X�� �θ�� (����1-2)
		G->right = X; // X��带 G�� right�� �Ű��� (���� 1-1)
	}
	else {
		G->right = Nil;
	}
	// P�� �θ���� ���� (���� 2-2)
	Node* GP = G->parent; // GP ����
	if (G == root) { // G�� root���ٸ� P�� root�� ����
		P->parent = nullptr;
		root = P;
	}
	else {
		P->parent = GP; //(2-2)
		// ����G�θ��� �ڽ��� P�� (���� 2-1)
		if (G == GP->left) GP->left = P; // G�� ���� �ڽ��̾��� ���
		else GP->right = P; // G�� ������ �ڽ��̾��� ���
	}
	P->left = G; // G�� P�� �ڽ����� (���� 3-1)
	G->parent = P; // P�� G�� �θ�� (���� 3-2)
}

/*       RIGHT ROTATE
   GP!                GP!            GP!
	G                  G              P
   / \            P   / \            / \
  P   U!      -->    X!  U!  -->        G
   \                                   / \
	X!	                              X!  U!   
���� 1-1. X�� G�� �ڽ�����,		1-2. G�� X�� �θ��
���� 2-1. P�� GP�� �ڽ�����,		2-2. GP�� P�� �θ��
���� 3-1. G�� P�� �ڽ�����,		3-2. P�� G�� �θ��
!�� Nil�̾ ������� ���,     �θ���� ������ Nil�̸� ���� x
*/
void RBtree::rightRotate(Node* G) {
	if (G->left == Nil) return;
	Node* P = G->left; // P����
	Node* X = P->right; // X ����
	
	if (X != Nil) {
		G->left = X; // X��带 G�� left�� �Ű��� (���� 1-1)
		X->parent = G; // G�� X�� �θ�� (����1-2)
	}
	else {
		G->left = Nil;
	}
	// P�� �θ���� ���� (���� 2-2)
	Node* GP = G->parent; // GP ����
	if (G == root) { // G�� root���ٸ� P�� root�� ����
		P->parent = nullptr;
		root = P;
	}
	else {
		P->parent = GP; //(2-2)
		// ����G�θ��� �ڽ��� P�� (���� 2-1)
		if (G == GP->left) GP->left = P; // G�� ���� �ڽ��̾��� ���
		else GP->right = P; // G�� ������ �ڽ��̾��� ���

	}
	P->right = G; // G�� P�� �ڽ����� (���� 3-1)
	G->parent = P; // P�� G�� �θ�� (���� 3-2)
}


/*  �� 4���� case ����
case1-1. P�� ���ʿ� ��ġ�ϰ� X�� �ﰢ�� ���
   �ﰢ��       ������
      LEFT ROTATE   RIGHT ROTATE      
     G     :      G              X
    / \    :     / \     -->    / \
   P   U   :    X   U          P   G
	\	   :   /                    \
	 X     :  P                      U

case1-2. P�� ���ʿ� ��ġ�ϰ� X�� ������ ���
   ������
         RIGHT ROTATE
      G              P
     / \     -->    / \
    P   U          X   G
   /                    \
  X                      U

case2-1. P�� �������ʿ� ��ġ�ϰ� X�� �ﰢ�� ���
   �ﰢ��       ������
	  RIGHT ROTATE   LEFT ROTATE
	 G     :      G              X
	/ \    :     / \     -->    / \
   U   P   :    U   X          G   P
	  /    :         \        /            
	 X     :          P      U      

case2-2. P�� �������ʿ� ��ġ�ϰ� X�� ������ ���
   ������
	     LEFT ROTATE
	 G                  P
	/ \      -->       / \
   U   P              G   X
	    \            /
	     X          U

*/
void RBtree::restructure(Node* node) {
	bool isLeft = (node->parent->parent->left == node->parent); // �θ���ġ Ȯ��
	//case1. �θ� ���ʿ� ��ġ
	if (isLeft) {
		if (node == node->parent->right) {// case1-1. �ﰢ��
			node = node->parent;
			leftRotate(node); // P�� �������� leftRotate
		} 
		// case1-2. ������
		node->parent->isBlack = true;  // X�� black����
		node->parent->parent->isBlack = false; // G�� red�� ����
		rightRotate(node->parent->parent); // G�� �������� rightRotate
	}
	else {
	//case2. �θ� �����ʿ� ��ġ
		if (node == node->parent->left) {// case2-1. �ﰢ��
			node = node->parent;
			rightRotate(node); // P�� �������� rightRotate
		}
		// case2-2. ������
		node->parent->isBlack = true;  // X�� black����
		node->parent->parent->isBlack = false; // G�� red�� ����
		leftRotate(node->parent->parent); // G�� �������� leftRoate
	}
}



void RBtree::insert(int id, string name, int size, int price) {
	// ���ο� ��� ����
	Node* node = new Node;
	node->ID = id;
	node->Name = name;
	node->Size = size;
	node->Price = price;
	node->left = Nil;
	node->right = Nil;
	node->parent = nullptr;  // parent�� Nil�� �ƴԿ� ����
	node->isBlack = false; // ���ο� ��� ���Խ� red�� ����

	// ���� BST�� ���� ������ ��ġ Ž�� �� �θ��� ��ġ ����
	Node* whereP = nullptr; // �θ�����ġ ����
	Node* cur = root;  // ���� Ž������ ��ġ => ���� ������ġ ����

	int depth = 0; // ���� ���庯��
	bool flag = false; // �̹� ���Ե��� �ִ��� ���θ� �����ϴ� ����

	// ���� ���Խ� root�� Nil 
	if (root == Nil) { //���� ������ ��� root�� ������ early return
		root = node;
		root->isBlack = true; // root�̹Ƿ� black���� ����
		cout << depth << "\n"; // 0��� �� ����
		return;
	}
    // ���� ������ �ƴ϶��
	while (cur != Nil) { // cur�� Nil�� ������ ���� �ݺ��ϸ� ������ ������ġ Ž��
		if (id == cur->ID) { // �̹� �����ϴ� ���̸� �ߴ�
			flag = true; // flag Ȱ��ȭ
			break; 
		}
		else if (id < cur->ID) { // ������ �������� �̵�
			whereP = cur; // �θ���ġ ����
			cur = cur->left;
			depth++;
		}
		else { // ũ�� ���������� �̵�
			whereP = cur; // �θ���ġ ����
			cur = cur->right;
			depth++;
		}
	}
	if (flag) { // �̹� �����ϴ� ���̾��ٸ� ���̸� ����� ����
		cout << depth << "\n";
		return;
	}
	// ���Գ�忡�� �θ���� ����
	node->parent = whereP;
	// �θ��忡�� �ڽİ��� ����
	if (node->ID < whereP->ID) { // �������� �۾����� 
		whereP->left = node;
	}
	else { // �������� ������
		whereP->right = node;
	}

	if (node->parent->parent == nullptr) { // �θ� root���ٸ� �ܼ� ������ ����
		cout << depth << "\n";
		return;
	}

	// �θ� black�̸� �ܼ� ������ ����
	if (node->parent->isBlack) {
		cout << depth << "\n";
		return;
	}

	// double red �߻�
	while (node->parent!= nullptr && !node->isBlack && !node->parent->isBlack) {
		// �θ� ��ġȮ��
		bool isLeft = (node->parent == node->parent->parent->left);
		Node* uncle; // uncle ��� ���� �� �ʱ�ȭ
		if (isLeft) uncle = node->parent->parent->right;
		else uncle = node->parent->parent->left;

		if (!uncle->isBlack) { // recoloring�� ���
			// ����, �θ� black����, ���θ� red�� ����
			uncle->isBlack = true;
			node->parent->isBlack = true;
			node->parent->parent->isBlack = false;
			node = node->parent->parent; // node�� ���θ�� ����
		}
		else { // restructure�� ���
			restructure(node);
		}
		root->isBlack = true;
	}
	cout << findDepth(id) << "\n";
	return;
}

int RBtree::findDepth(int id) {
	Node* cur = root;  // ���� Ž������ ��ġ => ���� ������ġ ����

	int depth = 0;
	while (cur != Nil) { // cur�� Nil�� ������ ���� �ݺ��ϸ� ������ ��ġ Ž��
		if (id == cur->ID) { // �̹� �����ϴ� ���̸� �ߴ�
			return depth;
		}
		else if (id < cur->ID) { // ������ �������� �̵�
			cur = cur->left;
			depth++;
		}
		else { // ũ�� ���������� �̵�
			cur = cur->right;
			depth++;
		}
	}
	return -1; // -1�� return�Ǹ� ������ �ִ°�. (�ش� id ���� x)
}

void RBtree::search(int key) {
	Node* cur = root;
	int depth = 0;
	// �Ϲ����� ����Ž��Ʈ���� ���԰���
	while (cur != Nil) {
		if (cur->ID == key) { // ������ ���� �̹� �����Ѵٸ�
			cout << depth << " " << cur->Name << " " << cur->Size << " " << cur->Price << "\n";
			return;
		}
		else if (cur->ID > key) { // ������ ���� ������ �������� �̵�
			cur = cur->left;
			depth++;
		}
		else { // ũ�� ������ �̵�
			cur = cur->right;
			depth++;
		}
	}
	cout << "NULL" << "\n";
	return;
}
void RBtree::update(int id, string name, int s, int p) {
	Node* cur = root;

	int depth = 0;
	// �Ϲ����� ����Ž��Ʈ���� ���԰���
	while (cur != Nil) {
		if (cur->ID == id) { // ������ ���� �̹� �����Ѵٸ�
			cur->Name = name; cur->Size = s; cur->Price = p;
			cout << depth << "\n";
			return;
		}
		else if (cur->ID > id) { // ������ ���� ������ �������� �̵�
			cur = cur->left;
			depth++;
		}
		else { // ũ�� ������ �̵�
			cur = cur->right;
			depth++;
		}
	}
	cout << "NULL" << "\n";
	return;
}
//void RBtree::discount(Node* R, int x, int y, double p) {
//	if (R == Nil) return; // ��������
//	if (x < R->ID) { // ���� ID�� ���۹��� ���� ū ���  
//		discount(R->left, x, y, p); // left Ž��
//	}
//	// ���� �ȿ� �ش� �� ��
//	if (x <= R->ID && R->ID <= y) { // p��ŭ ������ ����
//		double tmp = 1.0 - p / 100.0;
//		R->Price = R->Price * tmp;
//	} // ���� ID�� ����������� ���� ��� 
//	if (R->ID < y) discount(R->right, x, y, p); // right Ž��
//}
void RBtree::discount(Node* R, int x, int y, double p) {
	if (R == Nil) return; // ��������
	if (x <= R->ID && R->ID <= y) { // p��ŭ ������ ����
		//double tmp = (100.0 - p) / 100.0;
		R->Price = R->Price * (100.0 - p) / 100.0;
	} 
	 discount(R->left, x, y, p); // left Ž��
	 discount(R->right, x, y, p); // right Ž��
}

// ���Խ� ��� Ȯ�ο� �Լ�
void RBtree::preorder(Node* R) {
	if (R == Nil) return; // ��������
	string s = "R";
	if (R->isBlack) s = "B";
	cout << s << " " << R->ID << "\n";
	preorder(R->left); // left Ž��
	preorder(R->right); // right Ž��
}

int main() {
	int N;
	cin >> N;
	RBtree tree;
	while (N--) {
		char query;
		cin >> query;
		int id, size, price;
		string name;
		if (query == 'I') {
			cin >> id >> name >> size >> price;
			tree.insert(id, name, size, price);
			//cout << "--------preorder start-------------" << "\n";
			//tree.preorder(tree.root);
			//cout << "####### preorder finish #########" << "\n";
		}
		else if (query == 'F') {
			cin >> id;
			tree.search(id);
		}
		else if (query == 'R') {
			cin >> id >> name >> size >> price;
			tree.update(id, name, size, price);
		}
		else if (query == 'D') {
			int x, y, p;
			cin >> x >> y >> p;
			tree.discount(tree.root, x, y, p);
		}
	}
}