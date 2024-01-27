// Created by MichaelDev11
// Project Title: Student Management System
// Features:
// create student profile
// input courses and grades
// get gpa
// store and keep a db of all students

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>


#define NAME_LEN 30
#define ID_LEN 5

struct courseGrade {
    char course_name[15];
    char grade[3];  // Increased the size to accommodate grades like 'A+'
    struct courseGrade *next;
};

// Structure of the student and all of its important details
struct student {
    char name[NAME_LEN];
    char id[ID_LEN];
    char year[3];
    double gpa;
    struct courseGrade *course_grades;
    struct student *next;
};

// Prototype function definitions
struct student *add_student(struct student *list);
struct student *add_course_grade(struct student *list);
struct student *remove_student(struct student *list);
struct student *restore(struct student *list);
void print_students(struct student *list);
void print_student_info(struct student *list);
void calculateGpa(struct student *list, const char *sID);
void save_and_exit(struct student *list);
int read_line(char str[], int n);
double grade_to_gpa(const char *grade);

int main() {
    struct student *student_list = NULL;
    student_list = restore(student_list);
    char code;

    printf("Operation List: \n(a) add student, (u) add course and grade, (p) print students, (i) print student info, (r) remove student, (s) save and exit.\n");

    for (;;) {
        printf("Enter operation code: ");
        scanf(" %c", &code);
        while (getchar() != '\n'); 

        switch (code) {
            case 'a':
                student_list = add_student(student_list);
                break;
            case 'u':
                student_list = add_course_grade(student_list);
                break;
            case 'p':
                print_students(student_list);
                break;
            case 'i':
                print_student_info(student_list);
                break;
            case 'r':
                student_list = remove_student(student_list);
                break;
            case 's':
                save_and_exit(student_list);
                return 0;
            default:
                printf("Invalid operation code.\n");
        }

        printf("\n");
    }
}

struct student *restore(struct student *list) {
    FILE *file = fopen("studentdb.txt", "r");

    if (file == NULL) {
        perror("Error opening the restoration file.\n");
        exit(EXIT_FAILURE);
    }

    char line[100];  // Adjust the size according to your data size

    while (fgets(line, sizeof(line), file) != NULL) {
        struct student *r_student = (struct student *)malloc(sizeof(struct student));

        if (r_student == NULL) {
            printf("Memory allocation failed.\n");
            exit(EXIT_FAILURE);
        }

        int result = sscanf(line, "%4s | %29[^|] | %2s | %lf",
                            r_student->id, r_student->name, r_student->year, &r_student->gpa);

        // Check if the end of file or an error occurred
        if (result != 4) {
            printf("Error reading student data from file.\n");
            exit(EXIT_FAILURE);
        }

        r_student->course_grades = NULL; // Initialize course grades

        // Read course and grade information
        while (fgets(line, sizeof(line), file) != NULL && line[0] == '\t') {
            struct courseGrade *new_grade = (struct courseGrade *)malloc(sizeof(struct courseGrade));

            if (new_grade == NULL) {
                printf("Memory allocation failed.\n");
                exit(EXIT_FAILURE);
            }

            result = sscanf(line, "\t%14s %2s\n", new_grade->course_name, new_grade->grade);

            // Check if an error occurred
            if (result != 2) {
                printf("Error reading course grade data from file.\n");
                exit(EXIT_FAILURE);
            }

            new_grade->next = r_student->course_grades;
            r_student->course_grades = new_grade;
        }

        // Add the restored student to the list
        r_student->next = list;
        list = r_student;
    }

    fclose(file);
    return list;
}



struct student *add_student(struct student *list) {
    struct student *new_student = (struct student *)malloc(sizeof(struct student));

    // Allocation failure check
    if (new_student == NULL) {
        printf("Memory allocation failed.\n");
        exit(EXIT_FAILURE);
    }

    printf("Enter Student ID: ");
    read_line(new_student->id, ID_LEN);

    // Check to see if the person we are looking for is at the head of the list
    if (list == NULL || strcmp(new_student->id, list->id) < 0) {
        new_student->next = list;
        list = new_student;
    } else {
        struct student *curr = list;
        while (curr->next != NULL && strcmp(new_student->id, curr->next->id) > 0) {
            curr = curr->next;
        }
        new_student->next = curr->next;
        curr->next = new_student;
    }

    // Grab information
    printf("Enter Student Name: ");
    read_line(new_student->name, NAME_LEN);
    printf("Enter Student Year (F, So, Jr, Sr): ");
    read_line(new_student->year, sizeof(new_student->year));
    new_student->course_grades = NULL;

    return list;
}

struct student *add_course_grade(struct student *list) {
    char id[ID_LEN];

    // Check to see if we have students in db
    if (list == NULL) {
        printf("No students in the database.\n");
        return list;
    }

    printf("Enter Student ID: ");
    read_line(id, ID_LEN);

    struct student *curr = list;
    while (curr != NULL && strcmp(id, curr->id) != 0) {
        curr = curr->next;
    }

    if (curr != NULL) {
        printf("Student Found.\n");
        struct courseGrade *new_grade = (struct courseGrade *)malloc(sizeof(struct courseGrade));

        if (new_grade == NULL) {
            printf("Memory allocation failed.\n");
            exit(EXIT_FAILURE);
        }

        printf("Enter course name: ");
        read_line(new_grade->course_name, sizeof(new_grade->course_name));
        printf("Enter letter grade: ");
        read_line(new_grade->grade, sizeof(new_grade->grade));

        new_grade->next = curr->course_grades;
        curr->course_grades = new_grade;

        calculateGpa(list, id);
    } else {
        printf("Student not found.\n");
    }

    return list;
}

struct student *remove_student(struct student *list) {
    char id[ID_LEN];

    if (list == NULL) {
        printf("Student database is empty.\n");
        return list;
    }

    printf("Enter ID of the student to remove: ");
    read_line(id, ID_LEN);

    struct student *curr = list;
    struct student *prev = NULL;

    if (strcmp(id, curr->id) == 0) {
        list = curr->next;
        free(curr);
        printf("Student removed.\n");
        return list;
    } else {
        while (curr != NULL && strcmp(id, curr->id) != 0) {
            prev = curr;
            curr = curr->next;
        }

        if (curr != NULL) {
            prev->next = curr->next;
            free(curr);
            printf("Student removed.\n");
        } else {
            printf("Student does not exist in the database.\n");
        }
    }

    return list;
}

void print_students(struct student *list) {
    if (list != NULL) {
        struct student *curr = list;

        while (curr != NULL) {
            printf("ID: %s | Name: %s | Year: %s\n", curr->id, curr->name, curr->year);
            curr = curr->next;
        }
    } else {
        printf("Student database is empty.\n");
    }
}

void print_student_info(struct student *list) {
    if (list == NULL) {
        printf("Student database is empty.\n");
        return;
    }

    char id[ID_LEN];
    printf("Enter student ID: ");
    read_line(id, ID_LEN);

    struct student *curr = list;
    while (curr != NULL && strcmp(id, curr->id) != 0) {
        curr = curr->next;
    }

    if (curr != NULL) {
        printf("Student information:\n");
        printf("Name: %s\nStudent ID: %s\nYear: %s\nGPA: %.2f\n", curr->name, curr->id, curr->year, curr->gpa);
        printf("---Course Information---\n");

        struct courseGrade *current_grade = curr->course_grades;
        while (current_grade != NULL) {
            printf("%-15s%-2s\n", current_grade->course_name, current_grade->grade);
            current_grade = current_grade->next;
        }
    } else {
        printf("Student not in database.\n");
    }
}

void calculateGpa(struct student *list, const char *sID) {
    if (list == NULL) {
        printf("Student database is empty.\n");
        return;
    }

    struct student *curr = list;
    while (curr != NULL && strcmp(sID, curr->id) != 0) {
        curr = curr->next;
    }

    if (curr != NULL) {
        double quality_points = 0;
        int total_courses = 0;

        struct courseGrade *current_grade = curr->course_grades;
        while (current_grade != NULL) {
            quality_points += grade_to_gpa(current_grade->grade);
            total_courses++;
            current_grade = current_grade->next;
        }

        if (total_courses > 0) {
            curr->gpa = quality_points / total_courses;
        } else {
            printf("No courses found for GPA calculation.\n");
        }
    } else {
        printf("Student not found.\n");
    }
}

void save_and_exit(struct student *list) {
    printf("Saved information to database.\n");

    const char *filename_o = "studentdb.txt";
    FILE *file = fopen(filename_o, "w");

    if (file == NULL) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    struct student *curr = list;

    while (curr != NULL) {
        fprintf(file, "%s | %s | %s | %.2f\n",
                curr->id, curr->name, curr->year, curr->gpa);

        struct courseGrade *current_grade = curr->course_grades;
        while (current_grade != NULL) {
            fprintf(file, "\t%s %s\n", current_grade->course_name, current_grade->grade);
            current_grade = current_grade->next;
        }

        fprintf(file, "\n");
        curr = curr->next;
    }

    fclose(file);
}

double grade_to_gpa(const char *grade) {
    switch (toupper(grade[0])) {
        case 'A':
            return 4.0;
        case 'B':
            return 3.0;
        case 'C':
            return 2.0;
        case 'D':
            return 1.0;
        default:
            return 0.0;
    }
}

int read_line(char str[], int n) {
    int ch, i = 0;

    // Skip leading white spaces
    while (isspace(ch = getchar()));

    // Read characters until newline or end of string
    while (ch != '\n' && ch != EOF) {
        // Check if there's space in the buffer
        if (i < n - 1) {
            str[i++] = ch;
        }
        ch = getchar();
    }

    // Null-terminate the string
    str[i] = '\0';

    return i;
}

