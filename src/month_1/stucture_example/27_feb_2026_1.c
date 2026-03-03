#include <stdio.h>
#include <string.h>

// Define a basic structure
struct Person {
    char name[50];
    int age;
    float height;
};

int main() {
    // Create a structure variable
    struct Person person1; // Single variable of type struct Person can possibly store more than one value (name, age, height)
    struct Person person2; 
    
    // Assign values
    strcpy(person1.name, "Ninrapat");
    person1.age = 25;
    person1.height = 5.9f;

    strcpy(person2.name, "Kwankao");
    person2.age = 30;
    person2.height = 6.1f;

    // Print values
    printf("Name(1) : %s and Name(2) : %s\n", person1.name, person2.name);
    printf("Age(1) : %d and Age(2) : %d\n", person1.age , person2.age);
    printf("Height(1) : %.1f and Height(2) : %.1f\n", person1.height, person2.height);
    
    return 0;
}