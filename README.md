# 📚 Library Management System

A console-based Library Management System built in C++ using core Data Structures and OOP concepts.

## ✨ Features
- 🔍 Add, remove, and search books in O(1) time using Hash Maps
- 📖 Track borrowed books per user using Linked Lists
- 💸 Fine calculation at Rs. 2/day after 14-day return period
- 💾 Persistent storage — data saved to files automatically on exit

## 🧠 Concepts Used
- Hash Map (unordered_map) for O(1) book lookup
- Linked List for borrow record tracking per user
- Object-Oriented Programming — Book, User, Library classes
- File I/O for persistent data storage
- STL — unordered_map, vector, string

## ⚙️ How to Compile and Run
```bash
g++ library_management.cpp -o library_management
./library_management
```

## 🗂️ Menu Options
1. Add Book
2. Remove Book
3. Search Book by ID
4. Display All Books
5. Add User
6. View User and Borrowed Books
7. Borrow Book
8. Return Book
0. Exit

## 🛠️ Tech Stack
- Language: C++
- Concepts: DSA, OOP, File I/O, STL
