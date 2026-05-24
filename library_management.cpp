#include <bits/stdc++.h>
using namespace std;

//  STRUCT: Book
struct Book {
    string id;
    string title;
    string author;
    bool isAvailable;
    Book() : isAvailable(true) {}
    Book(string id, string title, string author)
        : id(id), title(title), author(author), isAvailable(true) {}
};


//  STRUCT: BorrowRecord (Linked List Node)
struct BorrowRecord {
    string bookId;
    string bookTitle;
    time_t borrowDate;
    BorrowRecord* next;
    BorrowRecord(string id, string title)
        : bookId(id), bookTitle(title), next(nullptr) {
        borrowDate = time(0);
    }
};


//  CLASS: User
class User {
public:
    string userId;
    string name;
    BorrowRecord* borrowedList; // head

    User() : borrowedList(nullptr) {}
    User(string uid, string name) : userId(uid), name(name), borrowedList(nullptr) {}

    // Add record at the head of linked list
    void addBorrow(string bookId, string bookTitle) {
        BorrowRecord* newRecord = new BorrowRecord(bookId, bookTitle);
        newRecord->next = borrowedList;
        borrowedList = newRecord;
    }

    //  a borrow record from linked list
    bool removeBorrow(string bookId) {
        BorrowRecord* curr = borrowedList;
        BorrowRecord* prev = nullptr;
        while (curr) {
            if (curr->bookId == bookId) {
                if (prev) prev->next = curr->next;
                else borrowedList = curr->next;
                delete curr;
                return true;
            }
            prev = curr;
            curr = curr->next;
        }
        return false;
    }

    // Get borrow date of a book
    time_t getBorrowDate(string bookId) {
        BorrowRecord* curr = borrowedList;
        while (curr) {
            if (curr->bookId == bookId) return curr->borrowDate;
            curr = curr->next;
        }
        return -1;
    }

    // Display all borrowed books
    void displayBorrowedBooks() {
        if (!borrowedList) {
            cout << "  No books currently borrowed."<<endl;
            return;
        }
        BorrowRecord* curr = borrowedList;
        while (curr) {
            char buf[20];
            strftime(buf, sizeof(buf), "%Y-%m-%d", localtime(&curr->borrowDate));
            cout << "  - [" << curr->bookId << "] " << curr->bookTitle
                 << " (Borrowed: " << buf << ")"<<endl;
            curr = curr->next;
        }
    }

    ~User() {
        BorrowRecord* curr = borrowedList;
        while (curr) {
            BorrowRecord* next = curr->next;
            delete curr;
            curr = next;
        }
    }
};

//  CLASS: Library
class Library {
private:
    unordered_map<string, Book> books;   
    unordered_map<string, User> users; 

    const double FINE_PER_DAY = 2.0; 
    const int MAX_BORROW_DAYS = 14;     

    // ── File paths ──
    const string BOOKS_FILE = "books.txt";
    const string USERS_FILE = "users.txt";

public:
    Library() {
        loadBooks();
        loadUsers();
    }

    ~Library() {
        saveBooks();
        saveUsers();
    }

    //  BOOK OPERATIONS

    void addBook(string id, string title, string author) {
        if (books.count(id)) {
            cout << "  [!] Book ID already exists."<<endl;
            return;
        }
        books[id] = Book(id, title, author);
        cout << "  [✓] Book added: " << title <<endl;
    }

    void removeBook(string id) {
        if (!books.count(id)) {
            cout << "  [!] Book not found."<<endl;
            return;
        }
        if (!books[id].isAvailable) {
            cout << "  [!] Cannot remove — book is currently borrowed."<<endl;
            return;
        }
        books.erase(id);
        cout << "  [✓] Book removed."<<endl;
    }

    // O(1) search using hash map
    void searchBook(string id) {
        if (!books.count(id)) {
            cout << "  [!] Book not found."<<endl;
            return;
        }
        Book& b = books[id];
        cout << "  ID     : " << b.id << ""<<endl;
        cout << "  Title  : " << b.title << ""<<endl;
        cout << "  Author : " << b.author << ""<<endl;
        cout << "  Status : " << (b.isAvailable ? "Available" : "Issued") << ""<<endl;
    }

    void displayAllBooks() {
        if (books.empty()) { cout << "  No books in library."<<endl; return; }
        cout << left << setw(10) << "ID" << setw(35) << "Title"
             << setw(25) << "Author" << "Status"<<endl;
        cout << string(80, '-') << ""<<endl;
        for (auto it = books.begin(); it != books.end(); ++it) {
            Book& b = it->second;
            cout << left << setw(10) << b.id << setw(35) << b.title
                 << setw(25) << b.author
                 << (b.isAvailable ? "Available" : "Issued") << ""<<endl;
        }
    }

    //  USER OPERATIONS

    void addUser(string uid, string name) {
        if (users.count(uid)) {
            cout << "  [!] User ID already exists."<<endl;
            return;
        }
        users[uid] = User(uid, name);
        cout << "  [✓] User registered: " << name << ""<<endl;
    }

    void displayUser(string uid) {
        if (!users.count(uid)) { cout << "  [!] User not found."<<endl; return; }
        User& u = users[uid];
        cout << "  User  : " << u.name << " (ID: " << u.userId << ")"<<endl;
        cout << "  Books : "<<endl;
        u.displayBorrowedBooks();
    }

    //  BORROW & RETURN

    void borrowBook(string uid, string bookId) {
        if (!users.count(uid)) { cout << "  [!] User not found."<<endl; return; }
        if (!books.count(bookId)) { cout << "  [!] Book not found."<<endl; return; }
        if (!books[bookId].isAvailable) {
            cout << "  [!] Book is currently not available."<<endl; return;
        }
        books[bookId].isAvailable = false;
        users[uid].addBorrow(bookId, books[bookId].title);
        cout << "  [✓] \"" << books[bookId].title << "\" issued to "
             << users[uid].name << ". Return within " << MAX_BORROW_DAYS << " days."<<endl;
    }

    void returnBook(string uid, string bookId) {
        if (!users.count(uid)) { cout << "  [!] User not found."<<endl; return; }
        if (!books.count(bookId)) { cout << "  [!] Book not found."<<endl; return; }

        time_t borrowDate = users[uid].getBorrowDate(bookId);
        if (borrowDate == -1) {
            cout << "  [!] This user has not borrowed that book."<<endl; return;
        }

        // Calculate fine
        double secondsElapsed = difftime(time(0), borrowDate);
        int daysElapsed = (int)(secondsElapsed / 86400);
        double fine = 0;
        if (daysElapsed > MAX_BORROW_DAYS)
            fine = (daysElapsed - MAX_BORROW_DAYS) * FINE_PER_DAY;

        books[bookId].isAvailable = true;
        users[uid].removeBorrow(bookId);

        cout << "  [✓] Book returned: " << books[bookId].title << ""<<endl;
        cout << "  Days borrowed : " << daysElapsed << ""<<endl;
        if (fine > 0)
            cout << "  Fine due      : Rs. " << fixed << setprecision(2) << fine << ""<<endl;
        else
            cout << "  No fine. Returned on time!"<<endl;
    }

    //  FILE I/O — Persistence

    void saveBooks() {
        ofstream f(BOOKS_FILE);
        for (auto it = books.begin(); it != books.end(); ++it) {
            Book& b = it->second;
            f << b.id << "|" << b.title << "|" << b.author
              << "|" << b.isAvailable << ""<<endl;
        }
    }

    void loadBooks() {
        ifstream f(BOOKS_FILE);
        if (!f.is_open()) return;
        string line;
        while (getline(f, line)) {
            stringstream ss(line);
            string id, title, author, avail;
            getline(ss, id, '|');
            getline(ss, title, '|');
            getline(ss, author, '|');
            getline(ss, avail, '|');
            if (id.empty()) continue;
            Book b(id, title, author);
            b.isAvailable = (avail == "1");
            books[id] = b;
        }
    }

    void saveUsers() {
        ofstream f(USERS_FILE);
        for (auto it = users.begin(); it != users.end(); ++it) {
            User& u = it->second;
            f << u.userId << "|" << u.name << ""<<endl;
            BorrowRecord* curr = u.borrowedList;
            while (curr) {
                f << "  BORROW|" << curr->bookId << "|"
                  << curr->bookTitle << "|" << curr->borrowDate << ""<<endl;
                curr = curr->next;
            }
        }
    }

    void loadUsers() {
        ifstream f(USERS_FILE);
        if (!f.is_open()) return;
        string line;
        string currentUid = "";
        while (getline(f, line)) {
            if (line.substr(0, 8) == "  BORROW") {
                stringstream ss(line.substr(9));
                string bookId, bookTitle, dateStr;
                getline(ss, bookId, '|');
                getline(ss, bookTitle, '|');
                getline(ss, dateStr, '|');
                BorrowRecord* rec = new BorrowRecord(bookId, bookTitle);
                rec->borrowDate = (time_t)stoll(dateStr);
                rec->next = users[currentUid].borrowedList;
                users[currentUid].borrowedList = rec;
            } else {
                stringstream ss(line);
                string uid, name;
                getline(ss, uid, '|');
                getline(ss, name, '|');
                if (uid.empty()) continue;
                users[uid] = User(uid, name);
                currentUid = uid;
            }
        }
    }
};

//  MAIN MENU

void printMenu() {
   
    cout << "    LIBRARY MANAGEMENT SYSTEM"<<endl;
    cout << "  1. Add Book                     "<<endl;
    cout << "  2. Remove Book                  "<<endl;
    cout << "  3. Search Book by ID            "<<endl;
    cout << "  4. Display All Books            "<<endl;
    cout << "  5. Add User                     "<<endl;
    cout << "  6. View User & Borrowed Books   "<<endl;
    cout << "  7. Borrow Book                  "<<endl;
    cout << "  8. Return Book                  "<<endl;
    cout << "  0. Exit                         "<<endl;
    cout << "  Choice: ";
}

int main() {
    Library lib;
    int choice;

    cout << " Welcome to the Library Management System!"<<endl;
    cout << "  (Data is saved automatically on exit)"<<endl;

    while (true) {
        printMenu();
        cin >> choice;
        cin.ignore();
        cout << ""<<endl;

        if (choice == 0) {
            cout << " Saving data and exiting. Goodbye!"<<endl;
            break;
        }

        string id, title, author, uid, name;

        switch (choice) {
            case 1:
                cout << "  Book ID   : "; getline(cin, id);
                cout << "  Title     : "; getline(cin, title);
                cout << "  Author    : "; getline(cin, author);
                lib.addBook(id, title, author);
                break;

            case 2:
                cout << "  Book ID to remove: "; getline(cin, id);
                lib.removeBook(id);
                break;

            case 3:
                cout << "  Book ID to search: "; getline(cin, id);
                lib.searchBook(id);
                break;

            case 4:
                lib.displayAllBooks();
                break;

            case 5:
                cout << "  User ID : "; getline(cin, uid);
                cout << "  Name    : "; getline(cin, name);
                lib.addUser(uid, name);
                break;

            case 6:
                cout << "  User ID : "; getline(cin, uid);
                lib.displayUser(uid);
                break;

            case 7:
                cout << "  User ID  : "; getline(cin, uid);
                cout << "  Book ID  : "; getline(cin, id);
                lib.borrowBook(uid, id);
                break;

            case 8:
                cout << "  User ID  : "; getline(cin, uid);
                cout << "  Book ID  : "; getline(cin, id);
                lib.returnBook(uid, id);
                break;

            default:
                cout << "  [!] Invalid choice."<<endl;
        }
    }

    return 0;
}
