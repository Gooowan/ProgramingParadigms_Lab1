// exe to render g++ main.cpp -std=c++11 -o program

#include <cstdio>
#include <string>
#include <cstdlib>
#include <stack>
#include <iostream>
#include <dlfcn.h>
#include <fstream>
#include <limits>
#include <stdexcept>
using namespace std;

char* encrypt(const char*, int);
char* decrypt(const char*, int);
const int chunksize = 256;

class IReader {
public:
    virtual ~IReader() noexcept {}
    virtual string read() = 0;
};

class FileReader: public IReader {
private:
    string filepath;

public:
    FileReader(const string& path) : filepath(path) {}

    string read() {
        ifstream file(filepath);
        if (!file.is_open()) {
            throw runtime_error("Error: File does not exist or cannot be opened.");
        }

        string content((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
        file.close();
        return content;
    }
};

class IWriter {
public:
    virtual ~IWriter() noexcept {}
    virtual void write(const string& content) = 0;
};

class FileWriter: public IWriter {
private:
    string filepath;

public:
    FileWriter(const string& path) : filepath(path) {}

    void write(const string& content) {
        ofstream file(filepath, ios::binary);
        if (!file.is_open()) {
            throw runtime_error("Error: File cannot be written.");
        }

        size_t pos = 0;
        while (pos < content.size()) {
            size_t chunk_length = min(chunksize, static_cast<int>(content.size() - pos));
            file.write(content.data() + pos, chunk_length);
            pos += chunk_length;
        }
        file.close();
    }
};

class User_Input {
public:
    static int getIntChoice() {
        char input[10];
        fgets(input, sizeof(input), stdin);
        return atoi(input);  // Convert string to integer
    }

    static void getStringInput(char* buffer, size_t bufferSize) {
        fgets(buffer, bufferSize, stdin);
        strtok(buffer, "\n");  // Remove the trailing newline character
    }

    static void getFileName(char* fileName, size_t fileSize) {
        printf("Enter your file name: ");
        getStringInput(fileName, fileSize);
    }

    static void clearConsole() {
        system("clear");
    }
};

class Main_buffer {
private:
    char buffer[100];
    char* main_buffer = NULL;
    size_t main_buffer_size = 0;
    FILE* inputFile;
    char fileName[30];
    char clipboard[100] = "";

    struct Command {
        enum CommandType {
            Append, // 1
            NewLine, // 2
            Delete, // 9
            Insert, // 6 or 10
            Cut, // 11
            Paste // 13
        } type;
        string data;
        int position;
    };

    stack<Command> undoStack;
    stack<Command> redoStack;

    void* lib = nullptr;
    char* (*encryptFunc)(const char*, int) = nullptr;
    char* (*decryptFunc)(const char*, int) = nullptr;

    bool loadLibrary() {
        lib = dlopen("./caesar_cipher.dylib", RTLD_LAZY);
        if (!lib) {
            cout << "Error loading the library: " << dlerror() << endl;
            return false;
        }

        encryptFunc = reinterpret_cast<char* (*)(const char*, int)>(dlsym(lib, "encrypt"));
        decryptFunc = reinterpret_cast<char* (*)(const char*, int)>(dlsym(lib, "decrypt"));

        if (!encryptFunc || !decryptFunc) {
            cout << "Error loading functions: " << dlerror() << endl;
            dlclose(lib);
            return false;
        }
        return true;
    }


public:


    Main_buffer() {
        if (!loadLibrary()) {
            cout << "Failed to load the encryption library." << endl;
        }
    }

    ~Main_buffer() {
        if (lib) {
            dlclose(lib);
        }
    }

    void appendLine();
    void appendNewLine();
    void writeIntoFile();
    void readingFromFile();
    void printingBuffer();
    void insertInText();
    void searchInText();
    static void clearingConsole();
    void deleteText();
    void insertText();
    void cut();
    void copy();
    void paste();
    char* getPosFromLineAndIndex(int a, int b);
    void undo();
    void redo();
    void encrypt();
    void decrypt();
};

void Main_buffer::appendLine() {
    printf("Enter text to append: ");
    fgets(buffer, sizeof(buffer), stdin);
    strtok(buffer, "\n");

    // Save the state for undo
    undoStack.push({ Command::Append, string(buffer), static_cast<int>(main_buffer_size) });

    size_t new_len = main_buffer_size + strlen(buffer) + 1;
    main_buffer = (char*) realloc(main_buffer, new_len);
    if (main_buffer == NULL) {
        printf("Memory allocation error\n");
        return;
    }

    if (main_buffer_size == 0) {
        strcpy(main_buffer, buffer);  // If it's the first allocation, use strcpy
    } else {
        strcat(main_buffer, buffer);  // Append buffer to main_buffer
    }
    main_buffer_size = new_len;

    printf("Whole text now: %s\n", main_buffer);

    redoStack = {};  // Clear the redo stack
}

void Main_buffer::appendNewLine() {
    // Save the state for undo
    undoStack.push({ Command::NewLine, "\n", static_cast<int>(main_buffer_size) });

    strcat(main_buffer, "\n");  // Add a new line if main_buffer is not empty
    main_buffer_size += 1;

    printf("You have successfully started a new line!\n");

    redoStack = {};  // Clear the redo stack
}

void Main_buffer::writeIntoFile() {
    printf("Enter your file name to save your buffer: ");
    fgets(fileName, sizeof(fileName), stdin);

    char *newline = strtok(fileName, "\n");
    if (newline != nullptr) {
        strcpy(fileName, newline);
    }

    inputFile = fopen(fileName, "w");
    if (inputFile != nullptr) {
        fputs(main_buffer, inputFile);
        fclose(inputFile);
        printf("Saved successfully\n");
    } else {
        printf("Error opening file\n");
    }
    if (main_buffer != nullptr) {
        memset(main_buffer, 0, sizeof(main_buffer));
    }
};

void Main_buffer::readingFromFile() {
    printf("Enter your file name to open: ");
    fgets(fileName, sizeof(fileName), stdin);
    strtok(fileName, "\n");

    inputFile = fopen(fileName, "r");
    if (inputFile == nullptr) {
        perror("Error opening file");
    } else {
        char tempBuffer[100];
        while (fgets(tempBuffer, sizeof(tempBuffer), inputFile) != nullptr) {
            char *pos;
            if ((pos = strchr(tempBuffer, '\n')) != nullptr)
                *pos = '\n';

            size_t new_len = main_buffer_size + strlen(tempBuffer) + 1;
            char* new_buffer = (char*) realloc(main_buffer, new_len);
            if (new_buffer == nullptr) {
                printf("Memory allocation error\n");
                fclose(inputFile);
                free(main_buffer);
            }

            main_buffer = new_buffer;
            if (main_buffer_size == 0) {
                strcpy(main_buffer, tempBuffer);
            } else {
                strcat(main_buffer, tempBuffer);
            }
            main_buffer_size = new_len;
        }
        fclose(inputFile);
        printf("File content appended to main_buffer.\n");
    }
}

void Main_buffer::printingBuffer() {
    printf("Current text: %s\n", main_buffer);
}

void Main_buffer::insertInText() {
    char lineIndex[10];
    int num1, num2;

    printf("Choose line and index (For example: 0 6): ");
    fgets(lineIndex, sizeof(lineIndex), stdin);

    // Parse the line and index from the input
    if (sscanf(lineIndex, "%d %d", &num1, &num2) != 2) {
        printf("Invalid input format. Please enter line and index as two integers.\n");
        return;
    } else if (num1 < 0 || num2 < 0) {
        printf("Line and index values must be non-negative.\n");
        return;
    } else {
        char *insertPos = getPosFromLineAndIndex(num1, num2);
        if (!insertPos) return;

        printf("Enter text to insert: ");
        fgets(buffer, sizeof(buffer), stdin);
        strtok(buffer, "\n");  // Remove the newline character

        // Save the state for undo
        undoStack.push({ Command::Insert, string(buffer), static_cast<int>(insertPos - main_buffer) });

        // Insert the text at the specified position
        memmove(insertPos + strlen(buffer), insertPos, strlen(insertPos) + 1);
        strncpy(insertPos, buffer, strlen(buffer));

        printf("Text inserted.\n");

        redoStack = {};  // Clear the redo stack
    }
}

void Main_buffer::searchInText() {
    printf("Enter word to find: ");
    fgets(buffer, sizeof(buffer), stdin);
    strtok(buffer, "\n");

    char *searchPosition = main_buffer;
    int found = 0;

    while ((searchPosition = strstr(searchPosition, buffer)) != NULL) {
        int lineCount = 1;
        for (char *ptr = main_buffer; ptr < searchPosition; ptr++) {
            if (*ptr == '\n') {
                lineCount++;
            }
        }

        printf("Word found at position: %d, on line: %d\n", (int)(searchPosition - main_buffer), lineCount);
        found = 1;

        searchPosition++;
    }

    if (!found) {
        printf("Word not found in the text.\n");
    }
}

void Main_buffer::clearingConsole() {
    system("clear");
}

void Main_buffer::deleteText() {
    char lineIndex[10];
    int lineNumber, index, numSymbols;

    // Get line number, index, and number of symbols from the user
    printf("Enter line number, index, number of symbols to delete : ");
    fgets(lineIndex, sizeof(lineIndex), stdin);

    if (sscanf(lineIndex, "%d %d %d", &lineNumber, &index, &numSymbols) != 3) {
        printf("Invalid input format. Please enter line and index as two integers.\n");
        return;
    }

    char *startPos = getPosFromLineAndIndex(lineNumber, index);
    if (!startPos) return;

    char *endPos = startPos + numSymbols;
    if (endPos > main_buffer + main_buffer_size) {
        endPos = main_buffer + main_buffer_size;
    }

    // Save the state for undo
    string dataDeleted(startPos, endPos);
    undoStack.push({ Command::Delete, dataDeleted, static_cast<int>(startPos - main_buffer) });

    // Move the text after endPos to the location of startPos
    memmove(startPos, endPos, main_buffer + main_buffer_size - endPos);

    // Adjust the size of the main_buffer
    main_buffer_size -= (endPos - startPos);

    // Null terminate the main_buffer
    main_buffer[main_buffer_size] = '\0';

    printf("Text deleted successfully.\n");
    printf("Whole text now: %s\n", main_buffer);

    redoStack = {};  // Clear the redo stack
}

void Main_buffer::insertText() {
    char lineIndex[10];
    int lineNumber, index;

    // Get line number and index from the user
    printf("Choose line and index: ");
    fgets(lineIndex, sizeof(lineIndex), stdin);

    if (sscanf(lineIndex, "%d %d", &lineNumber, &index) != 2) {
        printf("Invalid input format. Please enter line and index as two integers.\n");
        return;
    }

    char *insertPos = getPosFromLineAndIndex(lineNumber, index);
    if (!insertPos) return;

    printf("Write text: ");
    fgets(buffer, sizeof(buffer), stdin);
    strtok(buffer, "\n"); // Remove trailing newline

    // Save the state for undo
    undoStack.push({ Command::Insert, string(buffer), static_cast<int>(insertPos - main_buffer) });

    // Move the remaining text in main_buffer to make space for the insert
    memmove(insertPos + strlen(buffer), insertPos, main_buffer + main_buffer_size - insertPos);
    strncpy(insertPos, buffer, strlen(buffer));

    printf("Text inserted.\n");

    redoStack = {};  // Clear the redo stack
}

void Main_buffer::cut() {
    char lineIndex[10];
    int lineNumber, index, numSymbols;

    printf("Enter line number, index, number of symbols to cut : ");
    fgets(lineIndex, sizeof(lineIndex), stdin);

    if (sscanf(lineIndex, "%d %d %d", &lineNumber, &index, &numSymbols) != 3) {
        printf("Invalid input format. Please enter line and index as two integers.\n");
        return;
    }

    char *startPos = getPosFromLineAndIndex(lineNumber, index);
    if (!startPos) return;

    strncpy(clipboard, startPos, numSymbols);
    clipboard[numSymbols] = '\0';  // Null-terminate clipboard

    // Save the state for undo
    undoStack.push({ Command::Cut, string(clipboard), static_cast<int>(startPos - main_buffer) });

    // Shift the remaining text in the main_buffer to delete the cut portion
    memmove(startPos, startPos + numSymbols, strlen(startPos + numSymbols) + 1);

    printf("Text cut to clipboard: %s\n", clipboard);

    redoStack = {};  // Clear the redo stack
}

void Main_buffer::copy() {
    char lineIndex[10];
    int lineNumber, index, numSymbols;

    printf("Enter line number, index, number of symbols to copy : ");
    fgets(lineIndex, sizeof(lineIndex), stdin);

    if (sscanf(lineIndex, "%d %d %d", &lineNumber, &index, &numSymbols) != 3) {
        printf("Invalid input format. Please enter line and index as two integers.\n");
        return;
    }

    char *startPos = getPosFromLineAndIndex(lineNumber, index);
    if (!startPos) return;

    strncpy(clipboard, startPos, numSymbols);
    clipboard[numSymbols] = '\0';

    printf("Text copied to clipboard: %s\n", clipboard);
}

void Main_buffer::paste() {
    char lineIndex[10];
    int lineNumber, index;

    printf("Enter line number and index to paste at: ");
    fgets(lineIndex, sizeof(lineIndex), stdin);

    if (sscanf(lineIndex, "%d %d", &lineNumber, &index) != 2) {
        printf("Invalid input format. Please enter line and index as two integers.\n");
        return;
    }

    char *insertPos = getPosFromLineAndIndex(lineNumber, index);
    if (!insertPos) return;

    // Save the state for undo
    undoStack.push({ Command::Paste, string(clipboard), static_cast<int>(insertPos - main_buffer) });

    // Move the remaining text in main_buffer to make space for the paste
    memmove(insertPos + strlen(clipboard), insertPos, strlen(insertPos) + 1);
    strncpy(insertPos, clipboard, strlen(clipboard));

    printf("Text pasted from clipboard: %s\n", clipboard);

    redoStack = {};  // Clear the redo stack
}

char *Main_buffer::getPosFromLineAndIndex(int lineNumber, int index) {
    char *lineStart = main_buffer;
    for (int i = 0; i < lineNumber; i++) {
        lineStart = strchr(lineStart, '\n');
        if (!lineStart) {
            printf("Line number exceeds the number of lines in the text.\n");
            return nullptr;
        }
        lineStart++;
    }

    char *pos = lineStart + index;
    if (pos > main_buffer + main_buffer_size) {
        printf("Specified index is out of bounds.\n");
        return nullptr;
    }

    return pos;
}


void Main_buffer::undo() {
    if (undoStack.empty()) {
        printf("Nothing to undo.\n");
        return;
    }

    Command cmd = undoStack.top();
    undoStack.pop();

    switch (cmd.type) {
        case Command::Append:
        case Command::Insert:
        {
            int len = cmd.data.length();
            main_buffer_size -= len;
            main_buffer[cmd.position] = '\0';
            redoStack.push({ Command::Insert, cmd.data, cmd.position });
        }
            break;

        case Command::Delete:
        {
            int len = cmd.data.length();
            memmove(main_buffer + cmd.position + len, main_buffer + cmd.position, main_buffer_size - cmd.position);
            strncpy(main_buffer + cmd.position, cmd.data.c_str(), len);
            main_buffer_size += len;
            redoStack.push({ Command::Delete, cmd.data, cmd.position });
        }
            break;

        case Command::Cut:
        {
            int len = cmd.data.length();
            memmove(main_buffer + cmd.position + len, main_buffer + cmd.position, main_buffer_size - cmd.position);
            strncpy(main_buffer + cmd.position, cmd.data.c_str(), len);
            main_buffer_size += len;
            redoStack.push({ Command::Cut, cmd.data, cmd.position });
        }
            break;

        case Command::Paste:
        {
            int len = cmd.data.length();
            main_buffer_size -= len;
            main_buffer[cmd.position] = '\0';
            redoStack.push({ Command::Paste, cmd.data, cmd.position });
        }
            break;

        default:
            break;
    }

    printf("Undo executed.\n");

}

void Main_buffer::redo() {
    if (redoStack.empty()) {
        printf("Nothing to redo.\n");
        return;
    }

    Command cmd = redoStack.top();
    redoStack.pop();

    switch (cmd.type) {
        case Command::Insert:
        case Command::Paste:
        {
            int len = cmd.data.length();
            main_buffer = (char*)realloc(main_buffer, main_buffer_size + len + 1);
            memmove(main_buffer + cmd.position + len, main_buffer + cmd.position, main_buffer_size - cmd.position);
            strncpy(main_buffer + cmd.position, cmd.data.c_str(), len);
            main_buffer_size += len;
            // Terminate the string
            main_buffer[main_buffer_size] = '\0';
            undoStack.push({ Command::Delete, cmd.data, cmd.position });
        }
            break;

        case Command::Delete:
        case Command::Cut:
        {
            int len = cmd.data.length();
            main_buffer_size -= len;
            main_buffer[cmd.position] = '\0';
            undoStack.push({ Command::Insert, cmd.data, cmd.position });
        }
            break;

        default:
            break;
    }

    printf("Redo executed.\n");
}

void Main_buffer::encrypt() {
    if (main_buffer != nullptr && encryptFunc){
        int key;
        cout << "Enter an encryption key: ";
        cin >> key;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        char* encryptedText = encryptFunc(main_buffer, key);
        if (encryptedText) {
            strcpy(main_buffer, encryptedText);
            free(encryptedText);  // assuming the library allocates memory
            cout << "Encrypted: " << main_buffer << endl;
        }
    }
    else {
        cout << "Nothing in buffer or encryption function not loaded." << endl;
    }
}

void Main_buffer::decrypt() {
    if (main_buffer != nullptr && decryptFunc){
        int key;
        cout << "Enter an encryption key: ";
        cin >> key;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        char* decryptedText = decryptFunc(main_buffer, key);
        if (decryptedText) {
            strcpy(main_buffer, decryptedText);
            free(decryptedText);  // assuming the library allocates memory
            cout << "Decrypted: " << main_buffer << endl;
        }
    }
}


class Program {
public:
    void run() {
        Main_buffer bufferInstance;
        int choice;

        do {
            printf("Enter the command: ");
            choice = User_Input::getIntChoice();

            if (choice < 1 || choice > 17) {
                printf("Invalid input. Please enter a valid integer between 1 and 9.\n");
                continue;
            }

            switch (choice) {
                case 1:
                    bufferInstance.appendLine();
                    break;
                case 2:
                    bufferInstance.appendNewLine();
                    break;
                case 3:
                    bufferInstance.writeIntoFile();
                    break;
                case 4:
                    bufferInstance.readingFromFile();
                    break;
                case 5:
                    bufferInstance.printingBuffer();
                    break;
                case 6:
                    bufferInstance.insertInText();
                    break;
                case 7:
                    bufferInstance.searchInText();
                    break;
                case 8:
                    bufferInstance.clearingConsole();
                    break;
                case 9:
                    bufferInstance.deleteText();
                    break;
                case 10:
                    bufferInstance.insertText();
                    break;
                case 11:
                    bufferInstance.cut();
                    break;
                case 12:
                    bufferInstance.copy();
                    break;
                case 13:
                    bufferInstance.paste();
                    break;
                case 14:
                    bufferInstance.undo();
                    break;
                case 15:
                    bufferInstance.redo();
                    break;
                case 16:
                    bufferInstance.encrypt();
                    break;
                case 17:
                    bufferInstance.decrypt();
                    break;
                default:
                    break;
            }
        } while (choice != 0);
    }
};

int main() {
    Program program;
    program.run();
    return 0;
};