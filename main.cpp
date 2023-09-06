#include <stdio.h>
#include <string.h>

int main() {
    int choice;
    char buffer[100];
    char main_buffer[400] = ""; // Initialize main_buffer as an empty string

    do {
        printf("Enter the command: ");

        if (scanf("%d", &choice) != 1 || choice < 1 || choice > 8) {
            // If scanf returns a value other than 1 or if choice is outside the range, it's invalid input
            while (getchar() != '\n'); // Clear the input buffer
            printf("Invalid input. Please enter a valid integer between 1 and 8.\n");
            continue; // Skip the rest of the loop and ask for input again
        }

        switch (choice) {
            case 1:
                printf("Enter text to append: ");
                scanf("%99s", buffer); // Read a string into buffer
                strcat(main_buffer, buffer); // Append buffer to main_buffer
                printf("Whole text now: %s\n", main_buffer);
                break;
            case 2:
                printf("You chose Option 2\n");
                break;
            case 3:
                printf("You chose Option 3\n");
                break;
            case 4:
                printf("You chose Option 4\n");
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
