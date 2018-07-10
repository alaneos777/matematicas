#include <bits/stdc++.h>
using namespace std;

template<typename T>
struct SegmentTree{
	int N;
	vector<T> ST;

	SegmentTree(int N): N(N){
		ST.assign(N << 1, 0);
	}

	//build from an array in O(n)
	void build(vector<T> & arr){
		for(int i = 0; i < N; ++i)
			ST[N + i] = arr[i];
		for(int i = N - 1; i > 0; --i)
			ST[i] = ST[i << 1] + ST[i << 1 | 1];
	}

	//single element update in i
	void update(int i, T value){
		ST[i += N] = value; //update the element accordingly
		while(i >>= 1)
			ST[i] = ST[i << 1] + ST[i << 1 | 1];
	}

	//range query, [l, r]
	T query(int l, int r){
		T res = 0;
		for(l += N, r += N; l <= r; l >>= 1, r >>= 1){
			if(l & 1) res += ST[l++];
			if(!(r & 1)) res += ST[r--];
		}
		return res;
	}
};

template<typename T>
struct SegmentTreeDin{
	SegmentTreeDin *left, *right;
	int l, r;
	T value, lazy;
 
	SegmentTreeDin(int start, int end, vector<T> & arr): left(NULL), right(NULL), l(start), r(end), value(0), lazy(0){
		if(l == r) value = arr[l];
		else{
			int half = l + ((r - l) >> 1);
			left = new SegmentTreeDin(l, half, arr);
			right = new SegmentTreeDin(half+1, r, arr);
			value = left->value + right->value;
		}
	}
 
	void propagate(T dif){
		value += (r - l + 1) * dif;
		if(l != r){
			left->lazy += dif;
			right->lazy += dif;
		}
	}
 
	T query(int start, int end){
		if(lazy != 0){
			propagate(lazy);
			lazy = 0;
		}
		if(end < l || r < start) return 0;
		if(start <= l && r <= end) return value;
		else return left->query(start, end) + right->query(start, end);
	}
 
	void update(int start, int end, T dif){
		if(lazy != 0){
			propagate(lazy);
			lazy = 0;
		}
		if(end < l || r < start) return;
		if(start <= l && r <= end) propagate(dif);
		else{
			left->update(start, end, dif);
			right->update(start, end, dif);
			value = left->value + right->value;
		}
	}

	void update(int i, T value){
		update(i, i, value);
	}
};

template<typename T>
struct FenwickTree{
	int N;
	vector<T> bit;

	FenwickTree(int N): N(N){
		bit.assign(N, 0);
	}

	void build(vector<T> & arr){
		for(int i = 0; i < arr.size(); ++i){
			update(i, arr[i]);
		}
	}

	//single element increment
	void update(int pos, T value){
		while(pos < N){
			bit[pos] += value;
			pos |= pos + 1;
		}
	}

	//range query, [0, r]
	T query(int r){
		T res = 0;
		while(r >= 0){
			res += bit[r];
			r = (r & (r + 1)) - 1;
		}
		return res;
	}

	//range query, [l, r]
	T query(int l, int r){
		return query(r) - query(l - 1);
	}
};

struct MOquery{
	int l, r, index, S;
	bool operator<(const MOquery & q) const{
		int c_o = l / S, c_q = q.l / S;
		if(c_o == c_q)
			return r < q.r;
		return c_o < c_q;
	}
};

template<typename T>
struct SQRT{
	int N, S;
	vector<T> A, B;

	SQRT(int N): N(N){
		this->S = sqrt(N + .0) + 1;
		A.assign(N, 0);
		B.assign(S, 0);
	}

	void build(vector<T> & arr){
		A = vector<int>(arr.begin(), arr.end());
		for(int i = 0; i < N; ++i) B[i / S] += A[i];
	}

	//single element update
	void update(int pos, T value){
		int k = pos / S;
		A[pos] = value;
		T res = 0;
		for(int i = k * S, end = min(N, (k + 1) * S) - 1; i <= end; ++i) res += A[i];
		B[k] = res;
	}

	//range query, [l, r]
	T query(int l, int r){
		T res = 0;
		int c_l = l / S, c_r = r / S;
		if(c_l == c_r){
			for(int i = l; i <= r; ++i) res += A[i];
		}else{
			for(int i = l, end = (c_l + 1) * S - 1; i <= end; ++i) res += A[i];
			for(int i = c_l + 1; i <= c_r - 1; ++i) res += B[i];
			for(int i = c_r * S; i <= r; ++i) res += A[i];
		}
		return res;
	}

	//range queries offline using MO's algorithm
	vector<T> MO(vector<MOquery> & queries){
		vector<T> ans(queries.size());
		sort(queries.begin(), queries.end());
		T current = 0;
		int prevL = 0, prevR = -1;
		int i, j;
		for(const MOquery & q : queries){
			for(i = prevL, j = min(prevR, q.l - 1); i <= j; ++i){
				//remove from the left
				current -= A[i];
			}
			for(i = prevL - 1; i >= q.l; --i){
				//add to the left
				current += A[i];
			}
			for(i = max(prevR + 1, q.l); i <= q.r; ++i){
				//add to the right
				current += A[i];
			}
			for(i = prevR; i >= q.r + 1; --i){
				//remove from the right
				current -= A[i];
			}
			prevL = q.l, prevR = q.r;
			ans[q.index] = current;
		}
		return ans;
	}
};

template<typename T>
struct AVLNode
{
	AVLNode<T> *left, *right;
	short int height;
	int size;
	T value;

	AVLNode(T value = 0): left(NULL), right(NULL), value(value), height(1), size(1){}

	inline short int balance(){
		return (right ? right->height : 0) - (left ? left->height : 0);
	}

	inline void update(){
		height = 1 + max(left ? left->height : 0, right ? right->height : 0);
		size = 1 + (left ? left->size : 0) + (right ? right->size : 0);
	}

	AVLNode *maxLeftChild(){
		AVLNode *ret = this;
		while(ret->left) ret = ret->left;
		return ret;
	}
};

template<typename T>
struct AVLTree
{
	AVLNode<T> *root;

	AVLTree(): root(NULL){}

	inline int nodeSize(AVLNode<T> *& pos){return pos ? pos->size: 0;}

	int size(){return nodeSize(root);}

	void leftRotate(AVLNode<T> *& x){
		AVLNode<T> *y = x->right, *t = y->left;
		y->left = x, x->right = t;
		x->update(), y->update();
		x = y;
	}

	void rightRotate(AVLNode<T> *& y){
		AVLNode<T> *x = y->left, *t = x->right;
		x->right = y, y->left = t;
		y->update(), x->update();
		y = x;
	}

	void updateBalance(AVLNode<T> *& pos){
		short int bal = pos->balance();
		if(bal > 1){
			if(pos->right->balance() < 0)
				rightRotate(pos->right), leftRotate(pos);
			else
				leftRotate(pos);
		}else if(bal < -1){
			if(pos->left->balance() > 0)
				leftRotate(pos->left), rightRotate(pos);
			else
				rightRotate(pos);
		}
	}

	void insert(AVLNode<T> *&pos, T & value){
		if(pos){
			value < pos->value ? insert(pos->left, value) : insert(pos->right, value);
			pos->update();
			updateBalance(pos);
		}else{
			pos = new AVLNode<T>(value);
		}
	}

	AVLNode<T> *search(T & value){
		AVLNode<T> *pos = root;
		while(pos){
			if(value == pos->value) break;
			pos = (value < pos->value ? pos->left : pos->right);
		}
		return pos;
	}

	void erase(AVLNode<T> *&pos, T & value){
		AVLNode<T> *tmp, *next;
		if(!pos) return;
		if(value < pos->value) erase(pos->left, value);
		else if(value > pos->value) erase(pos->right, value);
		else{
			if(!pos->left) pos = pos->right;
			else if(!pos->right) pos = pos->left;
			else{
				next = pos->right->maxLeftChild();
				pos->value = next->value;
				erase(pos->right, pos->value);
			}
		}
		if(pos) pos->update(), updateBalance(pos);
	}

	void insert(T value){insert(root, value);}

	void erase(T value){erase(root, value);}

	void updateVal(T old, T New){
		if(search(old))
			erase(old), insert(New);
	}

	T kth(int i){
		if(i < 0 || i >= nodeSize(root)) return -1;
		AVLNode<T> *pos = root;
		while(i != nodeSize(pos->left)){
			if(i < nodeSize(pos->left)){
				pos = pos->left;
			}else{
				i -= nodeSize(pos->left) + 1;
				pos = pos->right;
			}
		}
		return pos->value;
	}

	int lessThan(T & x){
		int ans = 0;
		AVLNode<T> *pos = root;
		while(pos){
			if(x > pos->value){
				ans += nodeSize(pos->left) + 1;
				pos = pos->right;
			}else{
				pos = pos->left;
			}
		}
		return ans;
	}

	int lessThanOrEqual(T & x){
		int ans = 0;
		AVLNode<T> *pos = root;
		while(pos){
			if(x < pos->value){
				pos = pos->left;
			}else{
				ans += nodeSize(pos->left) + 1;
				pos = pos->right;
			}
		}
		return ans;
	}

	int greaterThan(T & x){
		int ans = 0;
		AVLNode<T> *pos = root;
		while(pos){
			if(x < pos->value){
				ans += nodeSize(pos->right) + 1;
				pos = pos->left;
			}else{
				pos = pos->right;
			}
		}
		return ans;
	}

	int greaterThanOrEqual(T & x){
		int ans = 0;
		AVLNode<T> *pos = root;
		while(pos){
			if(x > pos->value){
				pos = pos->right;
			}else{
				ans += nodeSize(pos->right) + 1;
				pos = pos->left;
			}
		}
		return ans;
	}

	int equalTo(T & x){
		return lessThanOrEqual(x) - lessThan(x);
	}

	void build(AVLNode<T> *& pos, vector<T> & arr, int i, int j){
		if(i > j) return;
		int m = i + ((j - i) >> 1);
		pos = new AVLNode<T>(arr[m]);
		build(pos->left, arr, i, m - 1);
		build(pos->right, arr, m + 1, j);
		pos->update();
	}

	void build(vector<T> & arr){
		size = arr.size();
		build(root, arr, 0, size - 1);
	}

	void output(AVLNode<T> *pos, vector<T> & arr, int & i){
		if(pos){
			output(pos->left, arr, i);
			arr[++i] = pos->value;
			output(pos->right, arr, i);
		}
	}

	void output(vector<T> & arr){
		int i = -1;
		output(root, arr, i);
	}
};

struct Treap{
	Treap *left, *right;
	int value;
	int key, size;

	Treap(int value = 0): value(value), key(rand()), size(1), left(NULL), right(NULL){}

	inline void update(){
		size = 1 + (left ? left->size : 0) + (right ? right->size : 0);
	}
};

inline int nodeSize(Treap* &pos){
	return pos ? pos->size: 0;
}

void merge(Treap* &T, Treap* T1, Treap* T2){
	if(!T1) T = T2;
	else if(!T2) T = T1;
	else if(T1->key > T2->key)
		merge(T1->right, T1->right, T2), T = T1;
	else
		merge(T2->left, T1, T2->left), T = T2;
	if(T) T->update();
}

void split(Treap* T, int x, Treap* &T1, Treap* &T2){
	if(!T)
		return void(T1 = T2 = NULL);
	if(x < T->value)
		split(T->left, x, T1, T->left), T2 = T;
	else
		split(T->right, x, T->right, T2), T1 = T;
	if(T) T->update();
}

Treap* search(Treap* T, int x){
	while(T){
		if(x == T->value) break;
		T = (x < T->value ? T->left : T->right);
	}
	return T;
}

void insert(Treap* &T, Treap* x){
	if(!T) T = x;
	else if(x->key > T->key)
		split(T, x->value, x->left, x->right), T = x;
	else
		insert(x->value < T->value ? T->left : T->right, x);
	if(T) T->update();
}

void insert(Treap* &T, int x){insert(T, new Treap(x));}

void erase(Treap* &T, int x){
	if(!T) return;
	if(T->value == x)
		merge(T, T->left, T->right);
	else
		erase(x < T->value ? T->left : T->right, x);
	if(T) T->update();
}

Treap* updateVal(Treap* &T, int old, int New){
	if(search(T, old))
		erase(T, old), insert(T, New);
}

int lessThan(Treap* T, int x){
	int ans = 0;
	while(T){
		if(x > T->value){
			ans += nodeSize(T->left) + 1;
			T = T->right;
		}else{
			T = T->left;
		}
	}
	return ans;
}

int kth(Treap* T, int i){
	if(i < 0 || i >= nodeSize(T)) return -1;
	while(i != nodeSize(T->left)){
		if(i < nodeSize(T->left)){
			T = T->left;
		}else{
			i -= nodeSize(T->left) + 1;
			T = T->right;
		}
	}
	return T->value;
}

int main(){
	int n, t, pos, value, l, r;
	cin >> n;
	vector<int> a(n);
	for(int i = 0; i < n; i++) cin >> a[i];
	SegmentTreeDin<int> *st = new SegmentTreeDin<int>(0, n-1, a);
	while(cin >> t && t != -1){
		if(t == 1){ //update single element
			cin >> pos >> value;
			st->update(pos, value);
		}else if(t == 2){ //query
			cin >> l >> r;
			cout << st->query(l, r) << "\n";
		}else if(t == 3){ //update range with element
			cin >> l >> r >> value;
			st->update(l, r, value);
		}
	}
	return 0;
}

/*int main(){
	int n, q, l, r;
	cin >> n;
	vector<int> a(n);
	for(int i = 0; i < n; i++) cin >> a[i];
	SQRT<int> *s = new SQRT<int>(n);
	s->build(a);
	cin >> q;
	vector<MOquery> queries(q);
	for(int i = 0; i < q; ++i){
		cin >> l >> r;
		queries[i] = {l, r, i, s->S};
	}
	vector<int> ans = s->MO(queries);
	for(int & x : ans){
		cout << x << "\n";
	}
}
*/

/*int main(){
	int t, n, m;
	AVLTree<int> *avl = new AVLTree<int>;
	while(cin >> t && t != -1){
		cin >> n;
		if(t == 0){ //insert
			avl->insert(n);
		}else if(t == 1){ //search
			AVLNode<int> *pos = avl->search(n);
			if(pos) cout << "Found\n";
			else cout << "Not found\n";
		}else if(t == 2){ //delete
			avl->erase(n);
		}else if(t == 3){ //update
			cin >> m;
			avl->updateVal(n, m);
		}else if(t == 4){ //lessThanOrEqual
			cout << avl->lessThan(n) << " " << avl->lessThanOrEqual(n) << " " << avl->greaterThan(n) << " " << avl->greaterThanOrEqual(n) << " " << avl->equalTo(n) << "\n";
		}else if(t == 5){ //get nth element
			cout << avl->kth(n) << "\n";
		}
	}
	return 0;
}*/

/*int main(){
	srand(time(NULL));
	int t, n, m;
	Treap *T = NULL;
	while(cin >> t && t != -1){
		cin >> n;
		if(t == 0){ //insert
			insert(T, n);
		}else if(t == 1){ //search
			Treap *pos = search(T, n);
			if(pos) cout << "Found\n";
			else cout << "Not found\n";
		}else if(t == 2){ //delete
			erase(T, n);
		}else if(t == 3){ //update
			cin >> m;
			updateVal(T, n, m);
		}else if(t == 4){ //lessThanOrEqual
			cout << lessThan(T, n) << "\n";
		}else if(t == 5){ //get nth element
			cout << kth(T, n) << "\n";
		}
	}
	return 0;
}*/