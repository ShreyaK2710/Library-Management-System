#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <memory>
using namespace std;

// Base class for Book
class Book {
protected:
    string title;
    string author;
    int bookID;
    bool isIssued;

public:
    Book(string title, string author, int bookID)
        : title(title), author(author), bookID(bookID), isIssued(false) {}

    virtual void displayInfo() {
        cout << "Book ID: " << bookID << "\nTitle: " << title << "\nAuthor: " << author << endl;
    }

    virtual void issueBook() = 0;  // Polymorphic method for issuing books
    virtual void returnBook() {
        isIssued = false;
        cout << "Book returned successfully." << endl;
    }

    bool getIssuedStatus() const {
        return isIssued;
    }

    int getBookID() const {
        return bookID;
    }

    virtual ~Book() = default;
};

// Derived class for EBook
class EBook : public Book {
public:
    EBook(string title, string author, int bookID)
        : Book(title, author, bookID) {}

    void displayInfo() override {
        cout << "E-Book - ";
        Book::displayInfo();
    }

    void issueBook() override {
        if (!isIssued) {
            isIssued = true;
            cout << "E-Book issued successfully." << endl;
        } else {
            cout << "E-Book is already issued." << endl;
        }
    }
};

// Derived class for PrintedBook
class PrintedBook : public Book {
public:
    PrintedBook(string title, string author, int bookID)
        : Book(title, author, bookID) {}

    void displayInfo() override {
        cout << "Printed Book - ";
        Book::displayInfo();
    }

    void issueBook() override {
        if (!isIssued) {
            isIssued = true;
            cout << "Printed Book issued successfully." << endl;
        } else {
            cout << "Printed Book is already issued." << endl;
        }
    }
};

// Factory class for creating Book objects
class BookFactory {
public:
    static unique_ptr<Book> createBook(const string& type, string title, string author, int bookID) {
        if (type == "EBook") {
            return make_unique<EBook>(title, author, bookID);
        } else if (type == "PrintedBook") {
            return make_unique<PrintedBook>(title, author, bookID);
        } else {
            return nullptr;
        }
    }
};

// Member class
class Member {
private:
    string name;
    int memberID;
    vector<int> borrowedBooks;

public:
    Member(string name, int memberID) : name(name), memberID(memberID) {}

    void borrowBook(Book& book) {
        if (!book.getIssuedStatus()) {
            book.issueBook();
            borrowedBooks.push_back(book.getBookID());
            cout << "Book borrowed by Member: " << name << endl;
        } else {
            cout << "Book is already issued to another member." << endl;
        }
    }

    void returnBook(Book& book) {
        book.returnBook();
        borrowedBooks.erase(std::remove(borrowedBooks.begin(), borrowedBooks.end(), book.getBookID()), borrowedBooks.end());
        cout << "Book with ID " << book.getBookID() << " has been returned by " << name << "." << endl;
    }

    void displayBorrowedBooks() {
        cout << "Member " << name << " has borrowed the following books: ";
        for (int id : borrowedBooks) {
            cout << id << " ";
        }
        cout << endl;
    }

    string getName() const {
        return name;
    }
};

// Librarian class
class Librarian {
private:
    string name;
    vector<unique_ptr<Book>> books;
    vector<unique_ptr<Member>> members;

public:
    Librarian(string name) : name(name) {}

    void addBook(const string& type, string title, string author, int bookID) {
        unique_ptr<Book> book = BookFactory::createBook(type, title, author, bookID);
        if (book) {
            books.push_back(move(book));
            cout << "Book added to library by Librarian: " << name << endl;
        }
    }

    void displayBooks() {
        cout << "\nLibrary Books:\n";
        for (const auto& book : books) {
            book->displayInfo();
            cout << "Issued: " << (book->getIssuedStatus() ? "Yes" : "No") << endl;
        }
    }

    Book* findBookByID(int bookID) {
        for (auto& book : books) {
            if (book->getBookID() == bookID) {
                return book.get();
            }
        }
        return nullptr;
    }

    Member* findOrCreateMember(const string& name) {
        for (auto& member : members) {
            if (member->getName() == name) {
                return member.get();
            }
        }
        int newID = members.size() + 1;
        members.push_back(make_unique<Member>(name, newID));
        cout << "New member created: " << name << " with ID " << newID << endl;
        return members.back().get();
    }

    void displayMembers() {
        cout << "\nLibrary Members:\n";
        for (const auto& member : members) {
            cout << "Member: " << member->getName() << endl;
            member->displayBorrowedBooks();
        }
    }
};

// Main function with menu
int main() {
    Librarian librarian("Alice");
    int choice;

    do {
        cout << "\n--- Library Management System ---\n";
        cout << "1. Add Book\n";
        cout << "2. Display All Books\n";
        cout << "3. Borrow Book\n";
        cout << "4. Return Book\n";
        cout << "5. Display Members\n";
        cout << "6. Exit\n";
        cout << "Enter your choice: ";
        cin >> choice;

        switch (choice) {
        case 1: {
            string type, title, author;
            int bookID;
            cout << "Enter Book Type (EBook/PrintedBook): ";
            cin >> type;
            cout << "Enter Book Title: ";
            cin.ignore();
            getline(cin, title);
            cout << "Enter Book Author: ";
            getline(cin, author);
            cout << "Enter Book ID: ";
            cin >> bookID;
            librarian.addBook(type, title, author, bookID);
            break;
        }
        case 2:
            librarian.displayBooks();
            break;
        case 3: {
            string memberName;
            int bookID;
            cout << "Enter Member Name: ";
            cin.ignore();
            getline(cin, memberName);
            cout << "Enter Book ID to borrow: ";
            cin >> bookID;
            Book* book = librarian.findBookByID(bookID);
            if (book) {
                Member* member = librarian.findOrCreateMember(memberName);
                member->borrowBook(*book);
            } else {
                cout << "Book not found." << endl;
            }
            break;
        }
        case 4: {
            string memberName;
            int bookID;
            cout << "Enter Member Name: ";
            cin.ignore();
            getline(cin, memberName);
            cout << "Enter Book ID to return: ";
            cin >> bookID;
            Book* book = librarian.findBookByID(bookID);
            if (book) {
                Member* member = librarian.findOrCreateMember(memberName);
                member->returnBook(*book);
            } else {
                cout << "Book not found." << endl;
            }
            break;
        }
        case 5:
            librarian.displayMembers();
            break;
        case 6:
            cout << "Exiting the system." << endl;
            break;
        default:
            cout << "Invalid choice. Please try again." << endl;
        }
    } while (choice != 6);

    return 0;
}
