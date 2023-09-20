#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int choice;
char buffer[100];
FILE *inputFile;
char fileName[30];
char* main_buffer = NULL; // Initialize main_buffer as a NULL pointer
size_t main_buffer_size = 0; // Initialize the main_buffer size

int case1() {
    printf("Enter text to append: ");
    fgets(buffer, sizeof(buffer), stdin);
    strtok(buffer, "\n"); // Remove the trailing newline character

    size_t new_len = main_buffer_size + strlen(buffer) + 1;
    main_buffer = (char*) realloc(main_buffer, new_len);
    if (main_buffer == NULL) {
        printf("Memory allocation error\n");
        return -1;
    }
    if (main_buffer_size == 0) {
        strcpy(main_buffer, buffer); // If it is the first allocation, use strcpy
    } else {
        strcat(main_buffer, buffer); // Append buffer to main_buffer
    }
    main_buffer_size = new_len;

    printf("Whole text now: %s\n", main_buffer);
    return 0;
};

int case2() {
    if (main_buffer[0] != '\0') {
        strcat(main_buffer, "\n"); // Add a new line if main_buffer is not empty,
    }
    printf("You have successfully started a new line! \n");
    return 0;
};

int case3() {
    printf("Enter your file name to save your buffer: ");
    fgets(fileName, sizeof(fileName), stdin);

    char *newline = strtok(fileName, "\n");
    if (newline != NULL) {
        strcpy(fileName, newline); // Use strcpy instead of assignment
    }

    inputFile = fopen(fileName, "w");
    if (inputFile != NULL) {
        fputs(main_buffer, inputFile);
        fclose(inputFile);
        printf("Saved successfully\n");
    } else {
        printf("Error opening file\n");
    }
    if (main_buffer != NULL){
        memset(main_buffer, 0, sizeof(main_buffer));
    }
    return 0;
};

int case4() {
    printf("Enter your file name to open: ");
    fgets(fileName, sizeof(fileName), stdin);
    strtok(fileName, "\n"); // Remove newline character from fileName

    inputFile = fopen(fileName, "r"); // Open file in read mode using the user-provided fileName
    if (inputFile == NULL) {
        perror("Error opening file");
        return -1;
    } else {
        char tempBuffer[100];
        // Read and append the entire file content
        while (fgets(tempBuffer, sizeof(tempBuffer), inputFile) != NULL) {
            char *pos;
            if ((pos = strchr(tempBuffer, '\n')) != NULL)
                *pos = '\n'; // Replacing newline character

            size_t new_len = main_buffer_size + strlen(tempBuffer) + 1; // Adding 1 for the null terminator
            char* new_buffer = (char*) realloc(main_buffer, new_len);
            if (new_buffer == NULL) {
                printf("Memory allocation error\n");
                fclose(inputFile);
                free(main_buffer); // Free the previously allocated memory
                return -1;
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
    return 0;
}





int case6(){
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
    return 0;
}

int case7(){
    printf("Enter word to find: ");
    fgets(buffer, sizeof(buffer), stdin);
    strtok(buffer, "\n"); // Remove the trailing newline character

    char *searchPosition = main_buffer;
    int found = 0;

    // Loop to search for the word in the main_buffer
    while ((searchPosition = strstr(searchPosition, buffer)) != NULL) {
        // Count the line number
        int lineCount = 1; // Start counting from 1
        for (char *ptr = main_buffer; ptr < searchPosition; ptr++) {
            if (*ptr == '\n') {
                lineCount++;
            }
        }

        printf("Word found at position: %d, on line: %d\n", (int) (searchPosition - main_buffer),
               lineCount);
        found = 1;
        // Move searchPosition to the next character to continue searching
        searchPosition++;
    }

    if (!found) {
        printf("Word not found in the text.\n");
    }
    return 0;
}

int main() {


    memset(buffer, 0, sizeof(buffer)); // Clear a buffer

    do {
        printf("Enter the command: ");

        if (scanf("%d", &choice) != 1 || choice < 1 || choice > 9) { // first - len, next two - about limit of commands
            while (getchar() != '\n');
            printf("Invalid input. Please enter a valid integer between 1 and 8.\n");
            continue; // Skip the rest of the loop, input again
        }

        getchar(); // Consume the newline character

        switch (choice) {
            case 1:
                case1(); //Appending a text to the end
                break;
            case 2:
                case2(); // New line
                break;
            case 3:
                case3(); //writing in file
                break;
            case 4:
                case4(); //reading from file
                break;
            case 5:
                printf("Current text: %s\n", main_buffer); // Print all info from buffer to console
                break;
            case 6:
                case6();
                break;
            case 7: {
                case7();
            }
                break;


            case 8:{
                system("clear");
            }
                break;

            default:
                break;
        }
    } while (choice != 9);

    return 0;
}
