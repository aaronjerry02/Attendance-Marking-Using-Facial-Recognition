#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_PERSON_NAME_LEN 50
#define RECOGNIZED_NAME_FILE "recognized_name.txt"
#define CSV_FILENAME "attendance_log.csv"
#define PYTHON_SCRIPT_PATH "python facerecognition/recognizer.py"

#define true 1
#define false 0
typedef int boolean;

typedef struct ar {
    char person[MAX_PERSON_NAME_LEN];
    int times_present;
    int total_classes;
    struct ar *next;
} ar;

static int total_classes = 0;


ar* createRecord(const char* name, int present, int total) {
    ar* newRecord = (ar*)malloc(sizeof(ar)); //
    if (newRecord == NULL) {
        perror("Failed to allocate memory");
        return NULL;
    }
    strncpy(newRecord->person, name, MAX_PERSON_NAME_LEN - 1);
    newRecord->times_present = present;
    newRecord->total_classes = total;
    newRecord->next = NULL;
    return newRecord;
}

// Frees all records in the linked list
void freeAttendanceRecords(ar *head) {
    ar *current = head;
    ar *next;
    while (current != NULL) {
        next = current->next;
        free(current);
        current = next;
    }
}

// Reads attendance records from CSV
ar *readAttendanceRecords() {
    FILE *file = fopen(CSV_FILENAME, "r");
    if (file == NULL) {
        total_classes = 0;
        return NULL;
    }

    ar *head = NULL;
    ar *tail = NULL;
    char line[256];

    // Read TOTAL session count
    if (fgets(line, sizeof(line), file) != NULL) {
        if (sscanf(line, "total_classes,%d", &total_classes) != 1) {
           + fprintf(stderr, "Warning: Could not read global session count. Resetting to 0.\n");
            total_classes = 0;
        }
    } else {
        total_classes= 0;
    }

    // Read and discard the header line for data 
    if (fgets(line, sizeof(line), file) == NULL) {
        fclose(file);
        return head;
    }

    while (fgets(line, sizeof(line), file) != NULL) {
        char person[MAX_PERSON_NAME_LEN];
        int times_present;
        int total_classes;
        double dummy_percentage;

        if (sscanf(line, "%49[^,],%d,%d,%lf", person, &times_present, &total_classes, &dummy_percentage) == 4) {
            ar *newRecord = createRecord(person, times_present, total_classes);
            if (newRecord == NULL) {
                freeAttendanceRecords(head);
                fclose(file);
                return NULL;
            }
            if (head == NULL) {
                head = newRecord;
                tail = newRecord;
            } else {
                tail->next = newRecord;
                tail = newRecord;
            }
        } else if (sscanf(line, "%49[^,],%d,%d", person, &times_present, &total_classes) == 3) {
            ar *newRecord = createRecord(person, times_present, total_classes);
            if (newRecord == NULL) {
                freeAttendanceRecords(head);
                fclose(file);
                return NULL;
            }
            if (head == NULL) {
                head = newRecord;
                tail = newRecord;
            } else {
                tail->next = newRecord;
                tail = newRecord;
            }
        } else {
            fprintf(stderr, "Warning: Skipping malformed line: %s", line);
        }
    }
    fclose(file);
    return head;
}

// Writes attendance records to CSV
void writeAttendanceRecords(ar *head) {
    FILE *file = fopen(CSV_FILENAME, "w");
    if (file == NULL) {
        perror("Failed to open CSV file for writing");
        return;
    }
    fprintf(file, "total_classes,%d\n", total_classes);
    fprintf(file, "Person,Times Present,Total Classes,Percentage\n");

    ar *current = head;
    while (current != NULL) {
        double percentage = 0.0;
        if (current->total_classes > 0) {
            percentage = ((double)current->times_present / current->total_classes) * 100.0;
        }
        fprintf(file, "%s,%d,%d,%.2f\n",
                current->person,
                current->times_present,
                current->total_classes,
                percentage);
        current = current->next;
    }
    fclose(file);
}

// Processes attendance for a recognized person or an unknown session
void processAttendance(const char *person_name, boolean was_recognized_and_known) {
    ar *head = readAttendanceRecords();

    total_classes++;
    printf("Total session count incremented to: %d\n", total_classes);

    ar *current = head;
    while (current != NULL) {
        current->total_classes = total_classes;
        current = current->next;
    }

    boolean found = false;
    current = head;
    while (current != NULL) {
        if (strcmp(current->person, person_name) == 0) {
            if (was_recognized_and_known) {
                current->times_present++;
                printf("Attendance logged for: %s (Times Present: %d)\n", person_name, current->times_present);
            } else {
                current->times_present++;
                printf("Record not found for %s \n", person_name);
            }
            found = true;
            break;
        }
        current = current->next;
    }

    if (!found) {
        ar *newRecord = createRecord(
            person_name,
            was_recognized_and_known ? 1 : 0,
            total_classes);

        if (newRecord == NULL) {
            freeAttendanceRecords(head);
            return;
        }
        if (head == NULL) {
            head = newRecord;
        } else {
            current = head;
            while (current->next != NULL) {
                current = current->next;
            }
            current->next = newRecord;
        }
        if (was_recognized_and_known) {
            printf("New record created and attendance logged for: %s (Times Present: %d)\n", person_name, newRecord->times_present);
        } else {
            printf("New record initialized for: %s (Times Present: %d)\n", person_name, newRecord->times_present);
        }
    }
    writeAttendanceRecords(head);
    freeAttendanceRecords(head);
}

// Displays all attendance records in tabular format
void displayAttendanceRecords() {
    ar *head = readAttendanceRecords();
    printf("\n--- Attendance Records ---\n");
    printf("%-20s %-15s %-15s %-15s\n", "Person", "Times Present", "Total Classes", "Percentage");
    printf("----------------------------------------------------------------------------------\n");

    if (head == NULL) {
        printf("No attendance records found.\n");
    } else {
        ar *current = head;
        while (current != NULL) {
            double percentage = 0.0;
            if (current->total_classes > 0) {
                percentage = ((double)current->times_present / current->total_classes) * 100.0;
            }
            printf("%-20s %-15d %-15d %-14.2f%%\n",
                   current->person,
                   current->times_present,
                   current->total_classes,
                   percentage);
            current = current->next;
        }
    }
    printf("----------------------------------------------------------------------------------\n");
    freeAttendanceRecords(head);
}

boolean getRecognizedName(char *recognized_name_buffer, size_t buffer_size) {
    int system_status = system(PYTHON_SCRIPT_PATH);
    if (system_status != 0) {
        fprintf(stderr, "Error: Python script failed with status %d.\n", system_status);
        return false;
    }

    FILE *file = fopen(RECOGNIZED_NAME_FILE, "r");
    if (file == NULL) {
        perror("Error: Could not open recognized name file");
        return false;
    }

    if (fgets(recognized_name_buffer, buffer_size, file) != NULL) {
        recognized_name_buffer[strcspn(recognized_name_buffer, "\n")] = '\0';
        fclose(file);
        printf("Recognized name: '%s'\n", recognized_name_buffer);
        return true;
    } else {
        fprintf(stderr, "Error: Could not read recognized name or file is empty.\n");
        fclose(file);
        return false;
    }
}

int main() {
    char recognized_name[MAX_PERSON_NAME_LEN];

    printf("--- Facial Recognition Attendance System ---\n");

    while (true) {
        char choice;
        printf("\nMenu:\n");
        printf(" 'r' - Run facial recognition\n");
        printf(" 'd' - Display all records\n");
        printf(" 'q' - Quit\n");
        printf("Enter choice: ");
        scanf(" %c", &choice);

        if (choice == 'r' || choice == 'R') {
            if (getRecognizedName(recognized_name, sizeof(recognized_name))) {
                if (strcmp(recognized_name, "UNKNOWN") == 0 || strcmp(recognized_name, "") == 0) {
                    printf("Recognition result: UNKNOWN PERSON. Session counted, no individual attendance.\n");
                    processAttendance("UNKNOWN_SESSION", false);
                } else {
                    processAttendance(recognized_name, true);
                }
            } else {
                fprintf(stderr, "Failed to get recognized name.\n");
            }
        } else if (choice == 'd' || choice == 'D') {
            displayAttendanceRecords();
        } else if (choice == 'q' || choice == 'Q') {
            printf("Quitting program.\n");
            break;
        } else {
            printf("Invalid choice. Please try again.\n");
        }
    }

    printf("\n--- Final Attendance Summary from CSV ---\n");
    displayAttendanceRecords();

    return 0;
}
