// Header guard prevents multiple inclusions of this file
#ifndef UTILS_H
#define UTILS_H

// Macro to calculate the number of elements in a static array
// Usage: int arr[10]; int count = ARRAY_LEN(arr); // Returns 10
// sizeof(x) = total bytes, sizeof((x)[0]) = bytes per element
#define ARRAY_LEN(x) (sizeof(x)/sizeof((x)[0]))

#endif // UTILS_H