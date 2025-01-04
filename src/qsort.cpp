#include <iostream>
#include <vector>

int partition(std::vector<int> &arr, int start, int end) {
  int pivot = arr[end];
  int pivotIndex = start;

  for (int i = start; i < end; i++) {
    if (arr[i] <= pivot) {
      std::swap(arr[pivotIndex], arr[i]);
      pivotIndex++;
    }
  }

  std::swap(arr[pivotIndex], arr[end]);
  return pivotIndex;
}

void quickSort(std::vector<int> &arr, int start, int end) {
  if (start >= end) {
    return;
  }

  int pivotIndex = partition(arr, start, end);
  quickSort(arr, start, pivotIndex - 1);
  quickSort(arr, pivotIndex + 1, end);
}

int main(int argc, char *argv[]) {
  std::vector<int> array = {5, 1, 7, 2, 4,  9,    3,    3, 3,    3,
                            3, 3, 3, 3, 10, 1000, -100, 5, 69420};

  quickSort(array, 0, array.size() - 1);

  for (int item : array) {
    std::cout << item << " ";
  }
  std::cout << std::endl;
}
