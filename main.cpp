#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main() {
    int choice;
    char buffer[100];
    FILE *inputFile;
    char fileName[30];
    char main_buffer[400] = ""; // Initialize main_buffer as an empty string
    char lineIndex[100]; // Moved declaration here
    int num1, num2; // Moved declaration here
    char searchWord[100]; // Moved declaration here

    memset(buffer, 0, sizeof(buffer));

    do {
        printf("Enter the command: ");

        if (scanf("%d", &choice) != 1 || choice < 1 || choice > 8) {
            while (getchar() != '\n'); // Clear the input buffer
            printf("Invalid input. Please enter a valid integer between 1 and 8.\n");
            continue; // Skip the rest of the loop and ask for input again
        }

        getchar(); // Consume the newline character

        switch (choice) {
            case 1:
                printf("Enter text to append: ");
                fgets(buffer, sizeof(buffer), stdin);

                strtok(buffer, "\n"); // Remove the trailing newline character
                strcat(main_buffer, buffer); // Append buffer to main_buffer
                printf("Whole text now: %s\n", main_buffer);

                memset(buffer, 0, sizeof(buffer));
                break;
            case 2:
                if (main_buffer[0] != '\0') {
                    strcat(main_buffer, "\n"); // Add a new line if main_buffer is not empty
                }
                printf("You have successfully started a new line! \n");
                break;
            case 3:
                printf("Enter your file name to save your buffer: ");
                fgets(fileName, sizeof(fileName), stdin);
                strtok(fileName, "\n"); // Remove the trailing newline character

                inputFile = fopen(fileName, "w");
                if (inputFile != NULL) {
                    fputs(main_buffer, inputFile);
                    fclose(inputFile);
                    printf("Saved successfully\n");
                } else {
                    printf("Error opening file\n");
                }
                memset(main_buffer, 0, sizeof(main_buffer));
                break;
            case 4:
                printf("Enter your file name to open: ");
                fgets(fileName, sizeof(fileName), stdin);
                strtok(fileName, "\n"); // Remove the trailing newline character

                inputFile = fopen(fileName, "r"); // Use "r" for reading
                if (inputFile == NULL) {
                    printf("Error opening file\n");
                } else {
                    // Read and append the entire file content
                    while (fgets(buffer, sizeof(buffer), inputFile) != NULL) {
                        strcat(main_buffer, buffer); // Append each line to main_buffer
                    }
                    fclose(inputFile);
                    printf("File content appended to main_buffer.\n");
                }
                break;
            case 5:
                printf("Current text: %s\n", main_buffer); // Print all info from buffer to console
                break;
            case 6:
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

                memset(buffer, 0, sizeof(buffer));
                break;
            case 7:
                printf("Enter word to find: ");
                fgets(searchWord, sizeof(searchWord), stdin);
                strtok(searchWord, "\n"); // Remove the trailing newline character

                char *searchPosition = main_buffer;
                int found = 0;

                // Loop to search for the word in the main_buffer
                while ((searchPosition = strstr(searchPosition, searchWord)) != NULL) {
                    printf("Word found at position: %d\n", (int) (searchPosition - main_buffer));
                    found = 1;
                    // Move searchPosition to the next character to continue searching
                    searchPosition++;
                }

                if (!found) {
                    printf("Word not found in the text.\n");
                }
                break;

            case 8:
                printf("Exiting the program\n");
                break;
            default:
                printf("Invalid choice\n");
                break;
        }
    } while (choice != 8);

    return 0;
}
