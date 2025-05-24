#include <stdio.h>
#include <stdlib.h>

int main() {
    // Step 1: Run the Python facial recognition script
    int status = system("python facerecognition/recognizer.py");

    if (status != 0) {
        printf("Error running recognizer.py\n");
        return 1;
    }

    // Step 2: Read the output name from file
    FILE *fp = fopen("recognized_name.txt", "r");
    if (fp == NULL) {
        printf("Could not open recognized_name.txt\n");
        return 1;
    }

    char name[100];
    if (fgets(name, sizeof(name), fp) != NULL) {
        printf("Recognized Person: %s\n", name);
    } else {
        printf("No name found in file.\n");
    }

    fclose(fp);
    return 0;
}
