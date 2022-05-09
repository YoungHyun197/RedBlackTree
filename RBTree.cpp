#include <iostream>
using namespace std;

struct Node { // 각종 데이터를 담는 노드 생성 
	int ID, Size, Price;
	string Name;
	Node* left;
	Node* right;
	Node* parent;
	bool isBlack; // true면 black, false면 red
};

class RBtree {
public:
	Node* root;
	Node* Nil; // Nil은 아무것도 없는 검은색 노드를 의미. 실제로 존재하는 노드는 아니고 
	// 삽입과정에서 레드, 블랙을 판별하기 위해 사용하는 Null node를 의미한다. 
	// 최초 root와 leaf는 하나의 Nil을 가리키도록 구현 

	RBtree() {
		Nil = new Node();
		Nil->left = nullptr;
		Nil->right = nullptr;
		Nil->parent = nullptr;
		Nil->isBlack = true;
		root = Nil;
	}

	void leftRotate(Node* node); // node를 기준으로 왼쪽회전
	void rightRotate(Node* node); // node를 기준으로 오른쪽회전
	void restructure(Node* node); // retstrucure
	int findDepth(int id); // node의 깊이 탐색

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
관계 1-1. X를 G의 자식으로,		1-2. G를 X의 부모로 
관계 2-1. P를 GP의 자식으로,		2-2. GP를 P의 부모로 
관계 3-1. G를 P의 자식으로,		3-2. P를 G의 부모로
!는 Nil이어도 상관없는 노드,     부모관계 설정시 Nil이면 수행 x
*/
// grand노드를 기준으로 좌측 회전
void RBtree::leftRotate(Node* G) {
	if (G->right == Nil) return;
	Node* P = G->right; // P설정
	Node* X = P->left; // X 설정
	if (X != Nil) {
		X->parent = G; // G를 X의 부모로 (관계1-2)
		G->right = X; // X노드를 G의 right로 옮겨줌 (관계 1-1)
	}
	else {
		G->right = Nil;
	}
	// P의 부모관계 설정 (관계 2-2)
	Node* GP = G->parent; // GP 설정
	if (G == root) { // G가 root였다면 P를 root로 변경
		P->parent = nullptr;
		root = P;
	}
	else {
		P->parent = GP; //(2-2)
		// 기존G부모의 자식을 P로 (관계 2-1)
		if (G == GP->left) GP->left = P; // G가 왼쪽 자식이었을 경우
		else GP->right = P; // G가 오른쪽 자식이었을 경우
	}
	P->left = G; // G를 P의 자식으로 (관계 3-1)
	G->parent = P; // P를 G의 부모로 (관계 3-2)
}

/*       RIGHT ROTATE
   GP!                GP!            GP!
	G                  G              P
   / \            P   / \            / \
  P   U!      -->    X!  U!  -->        G
   \                                   / \
	X!	                              X!  U!   
관계 1-1. X를 G의 자식으로,		1-2. G를 X의 부모로
관계 2-1. P를 GP의 자식으로,		2-2. GP를 P의 부모로
관계 3-1. G를 P의 자식으로,		3-2. P를 G의 부모로
!는 Nil이어도 상관없는 노드,     부모관계 설정시 Nil이면 수행 x
*/
void RBtree::rightRotate(Node* G) {
	if (G->left == Nil) return;
	Node* P = G->left; // P설정
	Node* X = P->right; // X 설정
	
	if (X != Nil) {
		G->left = X; // X노드를 G의 left로 옮겨줌 (관계 1-1)
		X->parent = G; // G를 X의 부모로 (관계1-2)
	}
	else {
		G->left = Nil;
	}
	// P의 부모관계 설정 (관계 2-2)
	Node* GP = G->parent; // GP 설정
	if (G == root) { // G가 root였다면 P를 root로 변경
		P->parent = nullptr;
		root = P;
	}
	else {
		P->parent = GP; //(2-2)
		// 기존G부모의 자식을 P로 (관계 2-1)
		if (G == GP->left) GP->left = P; // G가 왼쪽 자식이었을 경우
		else GP->right = P; // G가 오른쪽 자식이었을 경우

	}
	P->right = G; // G를 P의 자식으로 (관계 3-1)
	G->parent = P; // P를 G의 부모로 (관계 3-2)
}


/*  총 4가지 case 존재
case1-1. P가 왼쪽에 위치하고 X와 삼각형 모양
   삼각형       일직선
      LEFT ROTATE   RIGHT ROTATE      
     G     :      G              X
    / \    :     / \     -->    / \
   P   U   :    X   U          P   G
	\	   :   /                    \
	 X     :  P                      U

case1-2. P가 왼쪽에 위치하고 X와 일직선 모양
   일직선
         RIGHT ROTATE
      G              P
     / \     -->    / \
    P   U          X   G
   /                    \
  X                      U

case2-1. P가 오른쪽쪽에 위치하고 X와 삼각형 모양
   삼각형       일직선
	  RIGHT ROTATE   LEFT ROTATE
	 G     :      G              X
	/ \    :     / \     -->    / \
   U   P   :    U   X          G   P
	  /    :         \        /            
	 X     :          P      U      

case2-2. P가 오른쪽쪽에 위치하고 X와 일직선 모양
   일직선
	     LEFT ROTATE
	 G                  P
	/ \      -->       / \
   U   P              G   X
	    \            /
	     X          U

*/
void RBtree::restructure(Node* node) {
	bool isLeft = (node->parent->parent->left == node->parent); // 부모위치 확인
	//case1. 부모가 왼쪽에 위치
	if (isLeft) {
		if (node == node->parent->right) {// case1-1. 삼각형
			node = node->parent;
			leftRotate(node); // P를 기준으로 leftRotate
		} 
		// case1-2. 일직선
		node->parent->isBlack = true;  // X를 black으로
		node->parent->parent->isBlack = false; // G를 red로 변경
		rightRotate(node->parent->parent); // G를 기준으로 rightRotate
	}
	else {
	//case2. 부모가 오른쪽에 위치
		if (node == node->parent->left) {// case2-1. 삼각형
			node = node->parent;
			rightRotate(node); // P를 기준으로 rightRotate
		}
		// case2-2. 일직선
		node->parent->isBlack = true;  // X를 black으로
		node->parent->parent->isBlack = false; // G를 red로 변경
		leftRotate(node->parent->parent); // G를 기준으로 leftRoate
	}
}



void RBtree::insert(int id, string name, int size, int price) {
	// 새로운 노드 생성
	Node* node = new Node;
	node->ID = id;
	node->Name = name;
	node->Size = size;
	node->Price = price;
	node->left = Nil;
	node->right = Nil;
	node->parent = nullptr;  // parent는 Nil이 아님에 주의
	node->isBlack = false; // 새로운 노드 삽입시 red로 삽입

	// 기존 BST와 같이 삽입할 위치 탐색 및 부모노드 위치 저장
	Node* whereP = nullptr; // 부모노드위치 저장
	Node* cur = root;  // 현재 탐색중인 위치 => 최종 삽입위치 저장

	int depth = 0; // 깊이 저장변수
	bool flag = false; // 이미 삽입된적 있는지 여부를 저장하는 변수

	// 최초 삽입시 root는 Nil 
	if (root == Nil) { //최초 삽입일 경우 root에 삽입후 early return
		root = node;
		root->isBlack = true; // root이므로 black으로 설정
		cout << depth << "\n"; // 0출력 후 종료
		return;
	}
    // 최초 삽입이 아니라면
	while (cur != Nil) { // cur이 Nil을 만날때 까지 반복하며 적절한 삽입위치 탐색
		if (id == cur->ID) { // 이미 존재하는 값이면 중단
			flag = true; // flag 활성화
			break; 
		}
		else if (id < cur->ID) { // 작으면 왼쪽으로 이동
			whereP = cur; // 부모위치 저장
			cur = cur->left;
			depth++;
		}
		else { // 크면 오른쪽으로 이동
			whereP = cur; // 부모위치 저장
			cur = cur->right;
			depth++;
		}
	}
	if (flag) { // 이미 존재하는 값이었다면 깊이만 출력후 종료
		cout << depth << "\n";
		return;
	}
	// 삽입노드에게 부모관계 설정
	node->parent = whereP;
	// 부모노드에게 자식관계 설정
	if (node->ID < whereP->ID) { // 마지막에 작았으면 
		whereP->left = node;
	}
	else { // 마지막에 컸으면
		whereP->right = node;
	}

	if (node->parent->parent == nullptr) { // 부모가 root였다면 단순 삽입후 종료
		cout << depth << "\n";
		return;
	}

	// 부모가 black이면 단순 삽입후 종료
	if (node->parent->isBlack) {
		cout << depth << "\n";
		return;
	}

	// double red 발생
	while (node->parent!= nullptr && !node->isBlack && !node->parent->isBlack) {
		// 부모 위치확인
		bool isLeft = (node->parent == node->parent->parent->left);
		Node* uncle; // uncle 노드 생성 및 초기화
		if (isLeft) uncle = node->parent->parent->right;
		else uncle = node->parent->parent->left;

		if (!uncle->isBlack) { // recoloring일 경우
			// 삼촌, 부모를 black으로, 조부모를 red로 변경
			uncle->isBlack = true;
			node->parent->isBlack = true;
			node->parent->parent->isBlack = false;
			node = node->parent->parent; // node를 조부모로 변경
		}
		else { // restructure일 경우
			restructure(node);
		}
		root->isBlack = true;
	}
	cout << findDepth(id) << "\n";
	return;
}

int RBtree::findDepth(int id) {
	Node* cur = root;  // 현재 탐색중인 위치 => 최종 삽입위치 저장

	int depth = 0;
	while (cur != Nil) { // cur이 Nil을 만날때 까지 반복하며 적절한 위치 탐색
		if (id == cur->ID) { // 이미 존재하는 값이면 중단
			return depth;
		}
		else if (id < cur->ID) { // 작으면 왼쪽으로 이동
			cur = cur->left;
			depth++;
		}
		else { // 크면 오른쪽으로 이동
			cur = cur->right;
			depth++;
		}
	}
	return -1; // -1이 return되면 문제가 있는것. (해당 id 존재 x)
}

void RBtree::search(int key) {
	Node* cur = root;
	int depth = 0;
	// 일반적인 이진탐색트리의 삽입과정
	while (cur != Nil) {
		if (cur->ID == key) { // 삽입할 값이 이미 존재한다면
			cout << depth << " " << cur->Name << " " << cur->Size << " " << cur->Price << "\n";
			return;
		}
		else if (cur->ID > key) { // 삽입할 값이 작으면 왼쪽으로 이동
			cur = cur->left;
			depth++;
		}
		else { // 크면 오른쪽 이동
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
	// 일반적인 이진탐색트리의 삽입과정
	while (cur != Nil) {
		if (cur->ID == id) { // 삽입할 값이 이미 존재한다면
			cur->Name = name; cur->Size = s; cur->Price = p;
			cout << depth << "\n";
			return;
		}
		else if (cur->ID > id) { // 삽입할 값이 작으면 왼쪽으로 이동
			cur = cur->left;
			depth++;
		}
		else { // 크면 오른쪽 이동
			cur = cur->right;
			depth++;
		}
	}
	cout << "NULL" << "\n";
	return;
}
//void RBtree::discount(Node* R, int x, int y, double p) {
//	if (R == Nil) return; // 종료조건
//	if (x < R->ID) { // 현재 ID가 시작범위 보다 큰 경우  
//		discount(R->left, x, y, p); // left 탐색
//	}
//	// 범위 안에 해당 될 때
//	if (x <= R->ID && R->ID <= y) { // p만큼 할인율 적용
//		double tmp = 1.0 - p / 100.0;
//		R->Price = R->Price * tmp;
//	} // 현재 ID가 종료범위보다 작은 경우 
//	if (R->ID < y) discount(R->right, x, y, p); // right 탐색
//}
void RBtree::discount(Node* R, int x, int y, double p) {
	if (R == Nil) return; // 종료조건
	if (x <= R->ID && R->ID <= y) { // p만큼 할인율 적용
		//double tmp = (100.0 - p) / 100.0;
		R->Price = R->Price * (100.0 - p) / 100.0;
	} 
	 discount(R->left, x, y, p); // left 탐색
	 discount(R->right, x, y, p); // right 탐색
}

// 삽입시 출력 확인용 함수
void RBtree::preorder(Node* R) {
	if (R == Nil) return; // 종료조건
	string s = "R";
	if (R->isBlack) s = "B";
	cout << s << " " << R->ID << "\n";
	preorder(R->left); // left 탐색
	preorder(R->right); // right 탐색
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