function lowerBound(arr, v, l, r) {
    // console.log(l, r, arr[l], arr[r], v);
    if (v <= arr[l]) {
        return l;
    } 
    if (v > arr[r]) {
        return r + 1;
    }
    // invariant: arr[l] < v <= arr[r] and l != r
    let m = parseInt(Math.floor(l + (r - l) / 2));
    if (v <= arr[m]) {
        return lowerBound(arr, v, l, m);
    } else if (arr[m] < v) {
        return lowerBound(arr, v, m + 1, r);
    }
}

Array.prototype.lowerBound = function(e) {
    if (this.length === 0) {
        return 0;
    }
    // arr[0] < e <= arr[this.length-1]
    return lowerBound(this, e, 0, this.length - 1);
};

if (require.main === module) {
    function one() {
        console.log("===================")
        let x = [1];
        console.log(x.lowerBound(0), 0);
        console.log(x.lowerBound(1), 0);
        console.log(x.lowerBound(1.5), 1);
        console.log(x.lowerBound(2), 1);
        console.log("===================")
    }
    one();

    function two() {
        console.log("===================")
        let x = [1, 2];
        console.log(0, x.lowerBound(0), 0);
        console.log(1, x.lowerBound(1), 0);
        console.log(1.5, x.lowerBound(1.5), 1);
        console.log(x.lowerBound(2), 2);
        console.log(x.lowerBound(2.5), 2);
        console.log("===================")
    }
    two();

    function two_dup() {
        console.log("===================")
        let x = [1, 1];
        console.log(0, x.lowerBound(0), 0);
        console.log(1, x.lowerBound(1), 0);
        console.log(1.5, x.lowerBound(1.5), 2);
        console.log(1.5, x.lowerBound(2), 2);
        console.log("===================")
        // console.log(x.lowerBound(2), 1);
        // console.log(x.lowerBound(2.5), 2);
    }
    two_dup();
}

// [] => 0
// [1], 0 => 0
// [1], 1 => 0
// [1], 2 => 1
