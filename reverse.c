#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

// Define the node structure for the linked list
typedef struct node {
    char* data;         // Pointer to hold the line of data
    struct node* next;  // Pointer to the next node in the list
} node_t;

int main(int argc, char** argv) {
    FILE *input = stdin, *output = stdout;  // Set default input and output to standard input and output

    // Handle one argument (input file)
    if (argc == 2) {
        input = fopen(argv[1], "r");
        if (input == NULL) {
            fprintf(stderr, "reverse: cannot open file '%s'\n", argv[1]);
            return 1;
        }
    }

    // Handle two arguments (input and output files)
    if (argc == 3) {
        input = fopen(argv[1], "r");
        if (input == NULL) {
            fprintf(stderr, "reverse: cannot open file '%s'\n", argv[1]);
            return 1;
        }

        output = fopen(argv[2], "w");
        if (output == NULL) {
            fprintf(stderr, "reverse: cannot open file '%s'\n", argv[2]);
            fclose(input);
            return 1;
        }

        // Check if input and output files are the same
        struct stat in, out;
        if (fstat(fileno(input), &in) == -1 || fstat(fileno(output), &out) == -1) {
            fprintf(stderr, "reverse: cannot stat file\n");
            fclose(input);
            fclose(output);
            return 1;
        }
        if (in.st_ino == out.st_ino) {
            fprintf(stderr, "reverse: input and output file must differ\n");
            fclose(input);
            fclose(output);
            return 1;
        }
    }

    // Handle too many arguments
    if (argc > 3) {
        fprintf(stderr, "usage: reverse <input> <output>\n");
        return 1;
    }

    node_t* head = NULL;  // Initialize the head of the linked list to NULL
    char* line = NULL;    // Pointer to hold the line read from the file
    size_t len = 0;       // Variable to hold the length of the line
    ssize_t read;         // Variable to hold the number of characters read

    // Read lines from the input file
    while ((read = getline(&line, &len, input)) != -1) {
        node_t* new_node = malloc(sizeof(node_t)); // Allocate memory for a new node

        if (new_node == NULL) { // Check if malloc failed
            fprintf(stderr, "reverse: malloc failed\n");
            free(line);
            // Free the entire linked list before exiting
            while (head != NULL) {
                node_t* temp = head;
                head = head->next;
                free(temp->data);
                free(temp);
            }
            fclose(input);
            fclose(output);
            return 1;
        }

        // Remove the newline character from the line
        line[strcspn(line, "\n")] = 0;

        // Skip empty lines
        if (strlen(line) == 0) {
            free(new_node);
            continue;
        }

        // Duplicate the line and assign to new_node's data
        new_node->data = strdup(line);
        if (new_node->data == NULL) {
            fprintf(stderr, "reverse: strdup failed\n");
            free(new_node);
            free(line);
            // Free the entire linked list before exiting
            while (head != NULL) {
                node_t* temp = head;
                head = head->next;
                free(temp->data);
                free(temp);
            }
            fclose(input);
            fclose(output);
            return 1;
        }

        // Insert the new node at the beginning of the list
        new_node->next = head;
        head = new_node;
    }

    free(line); // Free the line buffer

    // Write the lines in reverse order to the output file
    while (head != NULL) {
        node_t* temp = head;
        fprintf(output, "%s\n", head->data); // Print the data of the current node
        head = head->next; // Move to the next node
        free(temp->data);  // Free the data of the current node
        free(temp);        // Free the current node
    }

    if (input != stdin) { // Close the input file if it's not stdin
        fclose(input);
    }
    if (output != stdout) { // Close the output file if it's not stdout
        fclose(output);
    }

    return 0; // Return success
}
