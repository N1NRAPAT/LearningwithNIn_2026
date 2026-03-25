#include <stdio.h>
#include <string.h>
#include <math.h>

struct Joint
{
    float angle ; 
    float length ;
    float x , y ; // current position of the joint end (x, y) calculated from the angle and length
} ;

struct Arm
{
    struct Joint joints[2] ; // 2D robot arm with 2 joints
    float base_x , base_y ; // base position of the robot arm
    float max_reach ; // maximum reach of the robot arm
    float min_reach ; // minimum reach of the robot arm
    int speed ; // Speed of the robot arm movement (steps per frame)
    int count_steps ; 
} ;

struct Target
{
    float x , y ; // target position for the robot arm to reach
    int is_reached ; // flag to indicate if the target is reached
    float distance_to_target ; // distance from the end effector to the target

} ;

void Arm_setup(struct Arm *arm) ;
float validate_float(char *prompt, float min, float max) ;
int validate_int(char *prompt, int min, int max) ;
void get_target_position(struct Target *target , struct Arm arm) ;
int check_target_reached(struct Target *target, struct Arm arm) ;

int main()
{
    struct Arm robot_arm ;
    struct Target target ;


    Arm_setup(&robot_arm) ; // Initialize the robot arm parameters

    do
    {
        get_target_position(&target, robot_arm) ; // Get target position from user input
        if(!target.is_reached) 
        {
            printf("Please enter a new target.\n");
        }
        
    }while (!target.is_reached) ; // Loop until the target is reached
    



    return 0 ; 
}


void Arm_setup(struct Arm *arm)
{
    printf("Setting up robot arm parameters...\n");

    arm->joints[0].length = validate_float("Enter length of joint 1", 1.0f, 50.0f) ;
    arm->joints[1].length = validate_float("Enter length of joint 2", 1.0f, 50.0f) ;
    arm->joints[0].angle  = 0.0f; // Initial angle of joint 1
    arm->joints[1].angle  = 0.0f; // Initial angle of joint 2
    arm->max_reach        = arm->joints[0].length + arm->joints[1].length ; // Maximum reach is the sum of both segments
    arm->min_reach        = arm->joints[0].length - arm->joints[1].length ; // Minimum reach is the difference of both segments 
    
    if (arm->min_reach < 0) arm->min_reach = -arm->min_reach ; // Ensure minimum reach is non-negative
    
    arm->base_x           = 0.0f; // Base position x
    arm->base_y           = 0.0f; // Base position y

    int choice = validate_int("Select speed (1=slow, 2=normal, 3=fast)", 1, 3);
    arm->speed = choice;

    if(choice == 1)      arm->count_steps = 30;
    else if(choice == 2) arm->count_steps = 20;
    else                 arm->count_steps = 10;
    
}

float validate_float(char *prompt, float min, float max){
    float value;
    int result;
    
    do {
        printf("%s (%.1f to %.1f): ", prompt, min, max);
        result = scanf("%f", &value);
        
        if(result != 1) {
            printf("Invalid! Please enter a number.\n");
            while(getchar() != '\n');  // clear buffer
            continue;
        }
        
        if(value < min || value > max)
            printf("Out of range! Must be between %.1f and %.1f\n", min, max);
        
    } while(result != 1 || value < min || value > max);
    
    return value;
}

int validate_int(char *prompt, int min, int max){

    int value;
    int result;
    
    do {
        printf("%s (%d to %d): ", prompt, min, max);
        result = scanf("%d", &value);
        
        if(result != 1) {
            printf("Invalid! Please enter an integer.\n");
            while(getchar() != '\n');  // clear buffer
            continue;
        }
        
        if(value < min || value > max)
            printf("Out of range! Must be between %d and %d\n", min, max);
        
    } while(result != 1 || value < min || value > max);
    
    return value;
}

void get_target_position(struct Target *target , struct Arm arm)
{

    printf("\n=== ENTER TARGET POSITION ===\n");
    printf("Arm reach: %.1f cm to %.1f cm\n", arm.min_reach, arm.max_reach);

    target->x = validate_float("Enter target X position", -arm.min_reach, arm.max_reach);
    target->y = validate_float("Enter target Y position", -arm.max_reach, arm.max_reach);
}

int check_target_reached(struct Target *target, struct Arm arm)
{
    // This function would calculate the distance from the end effector to the target and set target.is_reached accordingly
    // For this example, we will just return 0 (not reached)
    target->distance_to_target = sqrt(target->x * target->x + target->y * target->y); // Magnitude of the target position vector as a placeholder for distance

    if(target->distance_to_target <= arm.max_reach && target->distance_to_target >= arm.min_reach)
    {
        target->is_reached = 1; // Target is reachable
        printf("Target (%.1f, %.1f) is reachable!\n", target->x, target->y);
    } 
    else 
    {
        target->is_reached = 0; // Target is not reachable
        printf("Target (%.1f, %.1f) is not reachable.\n", target->x, target->y);
        printf("Distance: %.1f cm | Valid range: %.1f to %.1f cm\n",
               target->distance_to_target, arm.min_reach, arm.max_reach);
    }


    return target->is_reached; 
}