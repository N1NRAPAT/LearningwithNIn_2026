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
void Calculate_IK(struct Arm *arm ,struct Target target , float *t_angle1 , float *t_angle2 ) ;

int main()
{
    struct Arm robot_arm ;
    struct Target target ;


    Arm_setup(&robot_arm) ; // Initialize the robot arm parameters

    do
    {
        get_target_position(&target, robot_arm) ; // Get target position from user input
    
        check_target_reached(&target, robot_arm) ; // Check if the target is reachable and update target.is_reached
        
    }while (!target.is_reached) ; // Loop until the target is reached
    
    printf("\n === TARGET REACHED! === \n") ;
    printf("Target position: (%.1f, %.1f)\n", target.x, target.y) ;


    Calculate_IK(&robot_arm, target, &robot_arm.joints[0].angle, &robot_arm.joints[1].angle) ; // Calculate the inverse kinematics to find joint angles for the target position


    return 0 ; 
}


void Arm_setup(struct Arm *arm)
{
    printf("Setting up robot arm parameters...\n");

    arm->joints[0].length = validate_float("Enter length of joint 1 (cm)", 1.0f, 50.0f) ;
    arm->joints[1].length = validate_float("Enter length of joint 2 (cm)", 1.0f, 50.0f) ;
    arm->joints[0].angle  = 0.0f; // Initial angle of joint 1
    arm->joints[1].angle  = 0.0f; // Initial angle of joint 2
    arm->max_reach        = arm->joints[0].length + arm->joints[1].length ; // Maximum reach is the sum of both segments
    arm->min_reach        = arm->joints[0].length - arm->joints[1].length ; // Minimum reach is the difference of both segments 
    
    if (arm->min_reach < 0) arm->min_reach = -arm->min_reach ; // Ensure minimum reach is non-negative
    
    arm->base_x           = 0.0f; // Base position x
    arm->base_y           = 0.0f; // Base position y

    printf("\n=== ARM SETUP COMPLETE ===\n");
    printf("Joint 1 length: %.1f cm | Joint 2 length: %.1f cm\n", arm->joints[0].length, arm->joints[1].length);
    printf("Max range of motion: %.1f cm | Min range of motion: %.1f cm\n", arm->max_reach, arm->min_reach);
    printf("Base (x,y) position at starting point: (%.1f, %.1f)\n", arm->base_x, arm->base_y);

    printf("\n=== SELECT SPEED ===\n");
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
    printf("=== CHECKING TARGET REACHABILITY ===\n") ;
    if(target->distance_to_target <= arm.max_reach && target->distance_to_target >= arm.min_reach)
    {
        target->is_reached = 1; // Target is reachable
        printf("Target at (%.1f, %.1f) is reachable!\n", target->x, target->y);
    } 
    else 
    {
        target->is_reached = 0; // Target is not reachable
        printf("Target at (%.1f, %.1f) is not reachable.\n", target->x, target->y);
        printf("Distance: %.1f cm | Valid range: %.1f to %.1f cm\n",
               target->distance_to_target, arm.min_reach, arm.max_reach);
    }


    return target->is_reached; 
}

void Calculate_IK(struct Arm *arm ,struct Target target , float *t_angle1 , float *t_angle2 )
{
    float x  = target.x ; 
    float y  = target.y ; 
    float L1 = arm->joints[0].length ; 
    float L2 = arm->joints[1].length ; 
    float d  = target.distance_to_target ; 

    float cos_angle2 = (d*d - L1*L1 - L2*L2) / (2.0f * L1 * L2) ; // Law of cosines to find angle2
    
    if (cos_angle2 > 1.0f) cos_angle2  = 1.0f; // Clamp to valid range
    if (cos_angle2 < -1.0f) cos_angle2 = -1.0f;

    *t_angle2 = acos(cos_angle2) ; // Angle of joint 2
    *t_angle1 = atan2(y, x) - atan2(L2 * sin(*t_angle2), L1 + L2 * cos(*t_angle2)) ; // Angle of joint 1

    printf("\n=== IK SOLVED ===\n");
    printf("Joint 1 target angle: %.2f degrees\n", *t_angle1 * 180.0f / M_PI);
    printf("Joint 2 target angle: %.2f degrees\n", *t_angle2 * 180.0f / M_PI);

}

void draw_arm_animation(struct Arm arm, float target_angle1, float target_angle2)
{
    // This function would contain the logic to animate the arm movement from current angles to target angles
    // For this example, we will just print the target angles
    printf("\n=== ANIMATING ARM MOVEMENT ===\n");
    printf("Moving joint 1 to %.2f degrees\n", target_angle1 * 180.0f / M_PI);
    printf("Moving joint 2 to %.2f degrees\n", target_angle2 * 180.0f / M_PI);
}