# gorage

Generic storage implementation

## Classes

### `Item`

Binary `data` and arbitrary `metadata`

`metadata` should have `.fromJson` and `.toJson` methods (see [example](example.cpp))

### `ItemStorage`

Class for saving/loading items. Uses basic cache

### `Storage`

Abstract class for implementing

* items data storages
* items metadata storages

### `BinaryFileStorage`

Sample items data storage. Uses filesystem

### `FileStorage`

Sample items metadata storage. Uses filesystem