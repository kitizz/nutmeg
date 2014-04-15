QKDTree
=======

A ghetto C++/Qt based kd-tree implemented iteratively (i.e. with no recursion.) and with fancy unit tests. I created this tree because I needed a quick way to do nearest neighbor searches.

The kdtree supports storing key/value pairs where the key is a k-dimensional position and the value is anything you can cram into a QVariant.

Features:
* Inserting key/value pairs. O(logn) time.
* Finding nearest neighbor given a key or key/value pair. O(logn) time.
* Querying whether or not the tree contains a key/value pair with a given key. O(logn) time.
* Retrieving a value given a key in O(logn)


Tree does NOT currently support:
* Finding the k nearest neighbors to a key. This would be pretty easy to add.
* Finding all key/values within distance d of a key. This would not be too hard to add.
* Removing keys/values. This would be obnoxious to implement.
* Iterating through all keys/values/pairs. This would be pretty easy to support though.
