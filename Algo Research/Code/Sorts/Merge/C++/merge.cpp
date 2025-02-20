#include <bits/stdc++.h>
using namespace std;

void merge(vector<int>& arr, int left, int mid, int right){
    int n1 = mid - left + 1;
    int n2 = right - mid;
    int i = left, j = mid + 1, k = left;
    vector<int> temp(arr.begin(), arr.end());
    while (i <= mid && j <= right) {
        if (temp[i] <= temp[j]) {
            arr[k] = temp[i];
            i++;
        } else {
            arr[k] = temp[j];
            j++;
        }
        k++;
    }
    while (i <= mid) {
        arr[k] = temp[i];
        i++;
        k++;
    }
    while (j <= right) {
        arr[k] = temp[j];
        j++;
        k++;
    }
}

void mergeSort(vector<int>& arr, int left, int right){
    if (left < right) {
        int mid = left + (right - left) / 2;
        mergeSort(arr, left, mid);
        mergeSort(arr, mid + 1, right);
        merge(arr, left, mid, right);
    }
}

void printVector(const vector<int>& arr){
    for (int num : arr) cout << num << " ";
    cout << endl;
}

int main(){
    vector<int> arr = { 12, 11, 13, 5, 6, 7 };
    cout << "Input Vec \n";
    printVector(arr);

    mergeSort(arr, 0, arr.size() - 1);
    cout << "=======================";
    cout << "Sorted Vec is \n";
    printVector(arr);
    return 0;
}
