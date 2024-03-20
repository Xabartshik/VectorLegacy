#include "VectorLegacy.h"
#include <vector>
int main() 
{
	test();
	VectorLegacy<int> arr(5,1);

	VectorLegacy<int> arr2(5, 1);
	arr.print(); // 1 1 1 1 1

	arr.push_back(2);
	arr.print(); // 1 1 1 1 1 2
	cout << arr.pop_back() << endl;
	arr.print();
	arr.clear();
	arr.print();
	arr = { 1,2,3 };
	arr.print();
	arr.pop_front();
	arr.print();
	arr.push_front(2);
	arr.print();
	arr.insert(1, {1, 2, 3});
	arr.print();
	arr.delete_(1, 2);
	arr.print();
	arr.swap(arr2);
	cout << "После свапа" << endl;
	arr2.print();
	arr.print();


	arr = {1, 2, 7, 4, 8, 1, 3, 54, 12, 78, 1, -5, 3, 4, 7};
	cout << arr.seek(4) << endl;
	arr.sort();
	for (size_t i = 0; i < arr.size() - 1; i++) {
		if (arr[i] > arr[i + 1]) {
			cout << "Плохо" << endl;
		}
	}
	cout << "Хорошо" << endl;
	arr.print();
	return 0;

}