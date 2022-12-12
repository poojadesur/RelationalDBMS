# Data Systems Project Phase 0

Team - 58

Ritvik Aryan Kalra

2019115002

Pooja Desur

2019101112

## Part 2

**Page Layout**

Each page of a matrix stores a submatrix. We first calculate the total number of pages required given the maximum page size allowed and the size of the given matrix. We then calculate the maximum value N of a submatrix of size N * N that can fit into one page. Then each page is initialized with an empty 2d vector of N*N zeroes.

Each submatrix is calculated row wise - the maximum number of rows and columns is taken from the matrix. If there are lesser number of rows and columns in a submatrix than the maximum allowed size in a page, those values will remain as zeros. As a design choice, the extra rows will not be saved as zeros. While reading, we ensure we read only the required rows and columns and we can achieve this as we know the page index value.

For example, given the submatrix - 

$\begin{bmatrix}
1 & 2 & 3 & 4 & 5 \\
6 & 7 & 8 & 9 & 10 \\
11 & 12 & 13 & 14 & 15 \\
16 & 17 & 18 & 19 & 20 \\
21 & 22 & 23 & 24 & 25 \\
\end{bmatrix}$

and with max block size only allowing 9 integers (size = 9*4 = 36 kb), then each block/page can store only a submatrix of maximum size 3*3. Thus 4 pages are required to store this entire matrix. 

This submatrix will be split into the following pages - 

```
// Page 0
1 2 3
6 7 8
11 12 13
```

```
// Page 1
4 5 0
9 10 0
14 15 0
```

```
// Page 2
16 17 18
21 22 23
```

```
// Page 3
19 20 0
24 25 0
```

**Cross Transpose** 

1. Copy the submatrix of the 1st matrix to `ogMatrix`. If the matrix has rows less than that of the maximum no. of rows that can be stored in the block, then we add padding of $0$’s to this submatrix.

2. Transpose the `ogMatrix` into `transposedMatrix`

3. Copy the submatrix of the 2nd matrix to `ogMatrix`. If the matrix has rows less than that of the maximum no. of rows that can be stored in the block, then we add padding of $0$’s to this submatrix.

4. Copy the `transposedMatrix` into the 2nd matrix, in the location from where the submatrix of the 2nd matrix is taken.

5. Transpose the `ogMatrix` into `transposedMatrix`

6. Copy the `transposedMatrix` into the 1st matrix, in the location from where the submatrix of the 1st matrix is taken

## Part 3

**Compression Technique**

Since sparse matrices have many zeroes and only a few values, storing the matrices how we stored them in Part 2 would be a waste of storage and take up an unecessary amount of pages. By storing only the location of the values of the matrices (non-zero) values we can save space and also make the operations (Load, print etc) more time efficient.

Each value (non-zero value) in a sparse matrix can be represented as the row and column it belongs to in the matrix - 

$value = (row\_index, column\_index, value)$

where row_index and column_index are the corresponding row and column indices in the sparse matrix.

**Page Layout**

The first page (Page_0) that the sparse matrix is stored in will need to store the size of the matrix. The first line can store the number of rows and number of columns. The remaining lines (as many as the block size allows) can store each non-zero value by saving its row index, column index and value as space separated integers.

For example, for the sparse matrix - 

$\begin{bmatrix}
0 & 0 & 1 \\
3 & 0 & 0 \\
0 & 0 & 4 \\
\end{bmatrix}$ 

Page 0 would look like - 

```
3 3 // number of rows and columns
0 2 1 // row index column index (zero indexed) value
1 0 3
2 2 4
```

**Cross Transpose**

In a cross transpose of a matrix, each non zero value simply needs to have its row and column index swapped. Therefore each page can be iterated over and the first two values swapped (row and column indexes) would give the cross transpose of the matrix.

For example, matrix after cross transpose - 

$\begin{bmatrix}
0 & 0 & 1 \\
3 & 0 & 0 \\ 
0 & 0 & 4 \\
\end{bmatrix}
\rightarrow 
\begin{bmatrix}
0 & 3 & 0 \\
0 & 0 & 0 \\ 
1 & 0 & 4 \\
\end{bmatrix}$

Page 0 would now look like - 

```
3 3 // number of rows and columns
0 1 3 // row index column index (zero indexed) value
2 0 1
2 2 4

```
