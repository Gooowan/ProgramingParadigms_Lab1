#include <stdio.h>
#include <string>
#include <stdlib.h>

using namespace std;

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

public:
    Main_buffer() {}
    void appendLine();
    void appendNewLine();
    void writeIntoFile();
    void readingFromFile();
    void printingBuffer();
    void insertInText();
    void searchInText();
    void clearingConsole();
    void deleteText();
    void insertText();
    void cut();
    void copy();
    void paste();
    char* getPosFromLineAndIndex(int a, int b);
};

void Main_buffer::appendLine() {
    printf("Enter text to append: ");
    fgets(buffer, sizeof(buffer), stdin);
    strtok(buffer, "\n"); // Remove the trailing newline character

    size_t new_len = main_buffer_size + strlen(buffer) + 1;
    main_buffer = (char*) realloc(main_buffer, new_len);
    if (main_buffer == NULL) {
        printf("Memory allocation error\n");
    }
    if (main_buffer_size == 0) {
        strcpy(main_buffer, buffer); // If it is the first allocation, use strcpy
    } else {
        strcat(main_buffer, buffer); // Append buffer to main_buffer
    }
    main_buffer_size = new_len;

    printf("Whole text now: %s\n", main_buffer);
};

void Main_buffer::appendNewLine() {

    strcat(main_buffer, "\n"); // Add a new line if main_buffer is not empty,

    printf("You have successfully started a new line! \n");
};

void Main_buffer::writeIntoFile() {
    printf("Enter your file name to save your buffer: ");
    fgets(fileName, sizeof(fileName), stdin);

    char *newline = strtok(fileName, "\n");
    if (newline != nullptr) {
        strcpy(fileName, newline); // Use strcpy instead of assignment
    }

    inputFile = fopen(fileName, "w");
    if (inputFile != nullptr) {
        fputs(main_buffer, inputFile);
        fclose(inputFile);
        printf("Saved successfully\n");
    } else {
        printf("Error opening file\n");
    }
    if (main_buffer != nullptr){
        memset(main_buffer, 0, sizeof(main_buffer));
    }
};

void Main_buffer::readingFromFile() {
    printf("Enter your file name to open: ");
    fgets(fileName, sizeof(fileName), stdin);
    strtok(fileName, "\n"); // Remove newline character from fileName

    inputFile = fopen(fileName, "r"); // Open file in read mode using the user-provided fileName
    if (inputFile == nullptr) {
        perror("Error opening file");
    } else {
        char tempBuffer[100];
        // Read and append the entire file content
        while (fgets(tempBuffer, sizeof(tempBuffer), inputFile) != nullptr) {
            char *pos;
            if ((pos = strchr(tempBuffer, '\n')) != nullptr)
                *pos = '\n'; // Replacing newline character

            size_t new_len = main_buffer_size + strlen(tempBuffer) + 1; // Adding 1 for the null terminator
            char* new_buffer = (char*) realloc(main_buffer, new_len);
            if (new_buffer == nullptr) {
                printf("Memory allocation error\n");
                fclose(inputFile);
                free(main_buffer); // Free the previously allocated memory
            }

            main_buffer = new_buffer;
            if(main_buffer_size == 0) {
                strcpy(main_buffer, tempBuffer); // Copy buffer to main_buffer since it is the first time
            } else {
                strcat(main_buffer, tempBuffer); // Append buffer to main_buffer for subsequent lines
            }
            main_buffer_size = new_len;
        }
        fclose(inputFile);
        printf("File content appended to main_buffer.\n");
    }
}

void Main_buffer::printingBuffer(){
    printf("Current text: %s\n", main_buffer);
}

void Main_buffer::insertInText(){
    char lineIndex[10];
    int num1, num2;

    printf("Choose line and index (For example: 0 6): ");
    fgets(lineIndex, sizeof(lineIndex), stdin);

    // Parse the line and index from the input
    if (sscanf(lineIndex, "%d %d", &num1, &num2) != 2) {
        printf("Invalid input format. Please enter line and index as two integers.\n");
    } else if (num1 < 0 || num2 < 0) {
        printf("Line and index values must be non-negative.\n");
    } else {
        // Check if line number exceeds the number of lines in main_buffer
        int numLines = 0;
        char *tempBuffer = main_buffer;
        while (numLines < num1 && (tempBuffer = strchr(tempBuffer, '\n'))) {
            tempBuffer++;
            numLines++;
        }

        if (numLines < num1) {
            printf("Line number exceeds the number of lines in the text.\n");
        } else {
            // Insert text at the specified line and index
            if (num2 > strlen(tempBuffer)) {
                printf("Index exceeds the length of the line.\n");
            } else {
                printf("Enter text to insert: ");
                fgets(buffer, sizeof(buffer), stdin);
                strtok(buffer, "\n"); // Remove \n

                // Insert the text at the specified position
                char *insertPosition = main_buffer + (tempBuffer - main_buffer) + num2;
                memmove(insertPosition + strlen(buffer), insertPosition, strlen(insertPosition) + 1);
                strncpy(insertPosition, buffer, strlen(buffer));
                printf("Text inserted.\n");
            }
        }
    }
}

void Main_buffer::searchInText(){
    printf("Enter word to find: ");
    fgets(buffer, sizeof(buffer), stdin);
    strtok(buffer, "\n"); // Remove the trailing newline character

    char *searchPosition = main_buffer;
    int found = 0;

    // Loop to search for the word in the main_buffer
    while ((searchPosition = strstr(searchPosition, buffer)) != NULL) {
        // Count the line number
        int lineCount = 1;
        for (char *ptr = main_buffer; ptr < searchPosition; ptr++) {
            if (*ptr == '\n') {
                lineCount++;
            }
        }

        printf("Word found at position: %d, on line: %d\n", (int) (searchPosition - main_buffer),
               lineCount);
        found = 1;

        searchPosition++;
    }

    if (!found) {
        printf("Word not found in the text.\n");
    }
}

void Main_buffer::clearingConsole(){
    system("clear");
}

void Main_buffer::deleteText(){
    char lineIndex[10];
    int lineNumber, index, numSymbols;

    // Get line number, index, and number of symbols from the user
    printf("Enter line number, index, number of symbols to delete : ");
    fgets(lineIndex, sizeof(lineIndex), stdin);

    // Parse the line and index from the input
    if (sscanf(lineIndex, "%d %d %d", &lineNumber, &index, &numSymbols) != 3) {
        printf("Invalid input format. Please enter line and index as two integers.\n");
    } else if (lineNumber < 0 || index < 0 || numSymbols < 0) {
        printf("Line and index values must be non-negative.\n");
    } else {

        char *lineStart = getPosFromLineAndIndex(lineNumber, index);
        if (!lineStart) return;

        char *deleteStart = lineStart + index;
        if (deleteStart >= main_buffer + main_buffer_size) {
            printf("Specified index is out of bounds.\n");
            return;
        }

        char *deleteEnd = deleteStart + numSymbols;
        if (deleteEnd > main_buffer + main_buffer_size) {
            deleteEnd = main_buffer + main_buffer_size;
        }

        // Move the text after deleteEnd to the location of deleteStart
        memmove(deleteStart, deleteEnd, main_buffer + main_buffer_size - deleteEnd);

        // Adjust the size of the main_buffer
        main_buffer_size -= (deleteEnd - deleteStart);

        // Null terminate the main_buffer
        main_buffer[main_buffer_size] = '\0';

        printf("Text deleted successfully.\n");

        printf("Whole text now: %s\n", main_buffer);
    }
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

    char *insertStart = getPosFromLineAndIndex(lineNumber, index);
    if (!insertStart) return;

    printf("Write text: ");
    fgets(buffer, sizeof(buffer), stdin);
    strtok(buffer, "\n"); // Remove trailing newline

    // Determine the length of the text that will be replaced
    size_t replaceLength = strchr(insertStart, '\n') ? strchr(insertStart, '\n') - insertStart : strlen(insertStart);

    // Shift the remaining text
    memmove(insertStart + strlen(buffer) - replaceLength, insertStart + replaceLength, main_buffer + main_buffer_size - (insertStart + replaceLength));

    // Insert the new text at the identified location
    strncpy(insertStart, buffer, strlen(buffer));

    printf("Text replaced.\n");
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

    // Shift the remaining text in the main_buffer to delete the cut portion
    memmove(startPos, startPos + numSymbols, strlen(startPos + numSymbols) + 1);
    printf("Text cut to clipboard: %s\n", clipboard);
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
    clipboard[numSymbols] = '\0';  // Null-terminate clipboard

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

    // Move the remaining text in main_buffer to make space for the paste
    memmove(insertPos + strlen(clipboard), insertPos, strlen(insertPos) + 1);
    strncpy(insertPos, clipboard, strlen(clipboard));

    printf("Text pasted from clipboard: %s\n", clipboard);
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


class Program {
public:
    void run() {
        Main_buffer bufferInstance;
        int choice;


        do {
            printf("Enter the command: ");
            choice = User_Input::getIntChoice();

            if (choice < 1 || choice > 13) {
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