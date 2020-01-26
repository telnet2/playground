# C++ STL Algorithms

### Searching

The `lower_bound(begin(v), end(v), x)`, $l$ is defined as $\arg\max_i v[i] < x  \le v[i+1]$ assuming that $v[-1]$ is always less than $x$. 

The `upper_bound(begin(v), end(v), y)`, $u$ is defined as $\arg\max_j v[j] \le y < v[j+1]$ assuming that $v[i+1]$ is greater than $y$ for $j+1 \ge len(v)$.

The `lower_bound(x) + upper_bound(y)` is equal to the length of the range.

If $x$ doesn't exist in the range $v$, the `lower_bound(x)` equals to `upper_bound(x)`.

The difference betweeen `upper_bound(x)` and `lower_bound(x)` is the number of elements in the range $v$, `upper_bound(x) - lower_bound(x) = occurences(x)`.