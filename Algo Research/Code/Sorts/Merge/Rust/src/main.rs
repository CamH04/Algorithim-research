fn merge(arr: &mut Vec<i32>, left: usize, mid: usize, right: usize) {
    let temp = arr[left..=right].to_vec();
    let (mut i, mut j, mut k) = (0, mid - left + 1, left);
    while i <= mid - left && j <= right - left {
        if temp[i] <= temp[j] {
            arr[k] = temp[i];
            i += 1;
        } else {
            arr[k] = temp[j];
            j += 1;
        }
        k += 1;
    }
    while i <= mid - left {
        arr[k] = temp[i];
        i += 1;
        k += 1;
    }
    while j <= right - left {
        arr[k] = temp[j];
        j += 1;
        k += 1;
    }
}

fn merge_sort(arr: &mut Vec<i32>, left: usize, right: usize) {
    if left < right {
        let mid = left + (right - left) / 2;
        merge_sort(arr, left, mid);
        merge_sort(arr, mid + 1, right);
        merge(arr, left, mid, right);
    }
}

fn print_vec(arr: &Vec<i32>) {
    for &num in arr.iter() {
        print!("{} ", num);
    }
    println!();
}

fn main() {
    let mut arr = vec![12, 11, 13, 5, 6, 7];
    let arr_len = arr.len() -1;
    println!("Input Vec");
    print_vec(&arr);

    merge_sort(&mut arr, 0, arr_len);
    println!("=======================");
    println!("Sorted Vec is");
    print_vec(&arr);
}
