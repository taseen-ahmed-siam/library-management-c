#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
#include <windows.h> // For colored text on Windows
#endif

struct Book {
    int id;
    char title[50];
    char author[50];
};

// Function prototypes
void addBook();
void displayBooks();
void searchBook();
void deleteBook();
void updateBook();
void printHeader();
void setColor(int ForgC);

const char *filename = "library.dat";

int main() {
    int choice;

#ifdef _WIN32
    system("color 0F"); // Black background, white text
#endif

    while (1) {
        printHeader();
        setColor(10); // Light green text
        printf("1. Add Book\n");
        printf("2. Display Books\n");
        printf("3. Search Book\n");
        printf("4. Delete Book\n");
        printf("5. Update Book\n");
        printf("6. Exit\n");
        setColor(15); // Reset color
        printf("\nEnter your choice: ");
        scanf("%d", &choice);
        getchar(); // consume newline

        switch (choice) {
            case 1: addBook(); break;
            case 2: displayBooks(); break;
            case 3: searchBook(); break;
            case 4: deleteBook(); break;
            case 5: updateBook(); break;
            case 6:
                setColor(14); // Yellow text
                printf("\nThank you for using the Library Management System!\n");
                setColor(15);
                exit(0);
            default:
                setColor(12); // Red text
                printf("\nInvalid choice! Try again.\n");
                setColor(15);
        }

        printf("\nPress Enter to continue...");
        getchar();
#ifdef _WIN32
        system("cls"); // Clear screen on Windows
#else
        system("clear"); // Clear screen on Linux/Mac
#endif
    }

    return 0;
}

// ================== Functions ======================

void setColor(int ForgC) {
#ifdef _WIN32
    WORD wColor;
    HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO csbi;

    if(GetConsoleScreenBufferInfo(hStdOut, &csbi)) {
        wColor = (csbi.wAttributes & 0xF0) + (ForgC & 0x0F);
        SetConsoleTextAttribute(hStdOut, wColor);
    }
#endif
}

void printHeader() {
    setColor(11); // Cyan
    printf("===========================================\n");
    printf("        LIBRARY MANAGEMENT SYSTEM          \n");
    printf("===========================================\n\n");
    setColor(15);
}

void addBook() {
    struct Book b;
    FILE *fp = fopen(filename, "ab");
    if (!fp) {
        printf("Unable to open file!\n");
        return;
    }

    printf("\nEnter Book ID: ");
    scanf("%d", &b.id);
    getchar();

    printf("Enter Book Title: ");
    fgets(b.title, sizeof(b.title), stdin);
    b.title[strcspn(b.title, "\n")] = '\0';

    printf("Enter Author Name: ");
    fgets(b.author, sizeof(b.author), stdin);
    b.author[strcspn(b.author, "\n")] = '\0';

    fwrite(&b, sizeof(b), 1, fp);
    fclose(fp);

    setColor(10);
    printf("\nBook added successfully!\n");
    setColor(15);
}

void displayBooks() {
    struct Book b;
    FILE *fp = fopen(filename, "rb");
    if (!fp) {
        printf("No books found.\n");
        return;
    }

    printf("\n%-5s %-30s %-20s\n", "ID", "Title", "Author");
    printf("-----------------------------------------------------------\n");

    while (fread(&b, sizeof(b), 1, fp)) {
        printf("%-5d %-30s %-20s\n", b.id, b.title, b.author);
    }

    fclose(fp);
}

void searchBook() {
    struct Book b;
    int choice;
    int found = 0;
    FILE *fp = fopen(filename, "rb");
    if (!fp) {
        printf("No books found.\n");
        return;
    }

    printf("\nSearch by:\n1. ID\n2. Title\nEnter choice: ");
    scanf("%d", &choice);
    getchar();

    if (choice == 1) {
        int id;
        printf("Enter Book ID: ");
        scanf("%d", &id);

        while (fread(&b, sizeof(b), 1, fp)) {
            if (b.id == id) {
                setColor(10);
                printf("\nBook Found!\nID: %d\nTitle: %s\nAuthor: %s\n", b.id, b.title, b.author);
                setColor(15);
                found = 1;
                break;
            }
        }

    } else if (choice == 2) {
        char title[50];
        printf("Enter Book Title: ");
        fgets(title, sizeof(title), stdin);
        title[strcspn(title, "\n")] = '\0';

        while (fread(&b, sizeof(b), 1, fp)) {
            if (strcasecmp(b.title, title) == 0) {
                setColor(10);
                printf("\nBook Found!\nID: %d\nTitle: %s\nAuthor: %s\n", b.id, b.title, b.author);
                setColor(15);
                found = 1;
                break;
            }
        }

    } else {
        printf("Invalid choice!\n");
    }

    if (!found) {
        setColor(12);
        printf("Book not found!\n");
        setColor(15);
    }

    fclose(fp);
}

void deleteBook() {
    struct Book b;
    int id;
    int found = 0;
    FILE *fp = fopen(filename, "rb");
    FILE *temp = fopen("temp.dat", "wb");

    if (!fp || !temp) {
        printf("File error!\n");
        return;
    }

    printf("Enter Book ID to delete: ");
    scanf("%d", &id);

    while (fread(&b, sizeof(b), 1, fp)) {
        if (b.id == id) {
            found = 1;
            continue; // skip writing this book
        }
        fwrite(&b, sizeof(b), 1, temp);
    }

    fclose(fp);
    fclose(temp);

    remove(filename);
    rename("temp.dat", filename);

    if (found) {
        setColor(10);
        printf("Book deleted successfully!\n");
        setColor(15);
    } else {
        setColor(12);
        printf("Book not found!\n");
        setColor(15);
    }
}

void updateBook() {
    struct Book b;
    int id;
    int found = 0;
    FILE *fp = fopen(filename, "r+b");

    if (!fp) {
        printf("No books found.\n");
        return;
    }

    printf("Enter Book ID to update: ");
    scanf("%d", &id);
    getchar();

    while (fread(&b, sizeof(b), 1, fp)) {
        if (b.id == id) {
            found = 1;

            printf("Enter new Title: ");
            fgets(b.title, sizeof(b.title), stdin);
            b.title[strcspn(b.title, "\n")] = '\0';

            printf("Enter new Author: ");
            fgets(b.author, sizeof(b.author), stdin);
            b.author[strcspn(b.author, "\n")] = '\0';

            fseek(fp, -sizeof(b), SEEK_CUR);
            fwrite(&b, sizeof(b), 1, fp);

            setColor(10);
            printf("Book updated successfully!\n");
            setColor(15);
            break;
        }
    }

    if (!found) {
        setColor(12);
        printf("Book not found!\n");
        setColor(15);
    }

    fclose(fp);
}
