#include <stdio.h>
#include <string.h>

int main() {
    int choice;
    char buffer[100];
    FILE *inputFile;
    char fileName[30];
    char main_buffer[400] = ""; // Initialize main_buffer as an empty string

    memset(buffer, 0, sizeof(buffer));

    do {
        printf("Enter the command: ");

        if (scanf("%d", &choice) != 1 || choice < 1 || choice > 8) {
            // If scanf returns a value other than 1 or if choice is outside the range, it's invalid input
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
                inputFile = fopen(fileName, "w");
                if (inputFile != NULL)
                {
                    fputs(main_buffer, inputFile);
                    fclose(inputFile);
                    printf("Saved successfully\n");
                }
                else
                {
                    printf("Error opening file\n");
                }
                memset(main_buffer, 0, sizeof(main_buffer));
                break;
            case 4:
                printf("Enter your file name to open: ");
                fgets(fileName, sizeof(fileName), stdin);
                inputFile = fopen(fileName, "r"); // Use "r" for reading
                if (inputFile == NULL)
                {
                    printf("Error opening file\n");
                }
                else
                {
                    // Read and append the entire file content
                    while (fgets(buffer, sizeof(buffer), inputFile) != NULL)
                    {
                        strcat(main_buffer, buffer); // Append each line to main_buffer
                    }
                    fclose(inputFile);
                    printf("File content appended to main_buffer.\n");

                    //strtok(buffer, "\n"); // Remove the trailing newline character  **in case i used 1 line
                }
                break;
            case 5:
                printf("You chose Option 5\n");
                break;
            case 6:
                printf("You chose Option 6\n");
                break;
            case 7:
                printf("You chose Option 7\n");
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
