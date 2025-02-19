#include "main.h"
#include "autons.hpp"
#include "pros/misc.h"
#include "pros/motors.h"
#include "pros/rtos.hpp"
#include "subsystems.hpp"

/////
// For installation, upgrading, documentations, and tutorials, check out our website!
// https://ez-robotics.github.io/EZ-Template/
/////

// Chassis constructor
ez::Drive chassis(
    // These are your drive motors, the first motor is used for sensing!
    {-1,-4,-5},     // Left Chassis Ports (negative port will reverse it!)
    {7,8,9},  // Right Chassis Ports (negative port will reverse it!)*/
    2,      // IMU Port
    2.75,  // Wheel Diameter (Remember, 4" wheels without screw holes are actually 4.125!)
    450);   // Wheel RPM

ez::tracking_wheel horiz_tracker(6, 2.0, 0.68);

/**
 * Runs initialization code. This occurs as soon as the program is started.
 *
 * All other competition modes are blocked by initialize; it is recommended
 * to keep execution time for this mode under a few seconds.
 */

//code for arm config
const double heights[4] = {0, 52, 480, 600};//different lift heights
int positionIndex = 0;
auto armControl = AsyncPosControllerBuilder().withMotor({18, -11}).withGearset(okapi::AbstractMotor::GearsetRatioPair(okapi::AbstractMotor::gearset::green, 36.0 / 12.0)).build(); //schmobedying up smth vicious


void intakeControl()
{
  while (true) 
  {
    if (master.get_digital(DIGITAL_R1))
      intakeMotors.move_velocity(600);
    else if (master.get_digital(DIGITAL_R2))
      intakeMotors.move_velocity(-600);
    /*else if (positionIndex == 2)
    {
      intakeMotors.move_relative(-100, 600);
      while (!((intakeMotors.get_position() > -105) && (intakeMotors.get_position() < -95))) {
      // Continue running this loop as long as the motor is not within +-5 units of its goal
        pros::delay(2);
      }

      intakeMotors.tare_position();
    }*/
    else 
      intakeMotors.move_velocity(0);
  } 
  
}

void initialize() 
{
  //pros::Task colorSortThread(colorSort);
  // Print our branding over your terminal :D
  ez::ez_template_print();
  rotationSensor.reset();
  colorSensor.set_led_pwm(100);
  pros::delay(500);  // Stop the user from doing anything while legacy ports configure

  // Configure your chassis controls
  chassis.opcontrol_curve_buttons_toggle(false);  // Enables modifying the controller curve with buttons on the joysticks
  chassis.opcontrol_drive_activebrake_set(0);    // Sets the active brake kP. We recommend ~2.  0 will disable.
  chassis.opcontrol_curve_default_set(0, 0);     // Defaults for curve. If using tank, only the first parameter is used. (Comment this line out if you have an SD card!)

  chassis.odom_tracker_back_set(&horiz_tracker);

  //setting motors to coast for setup
  armMotor.set_brake_mode_all(MOTOR_BRAKE_COAST);

  // Set the drive to your own constants from autons.cpp!
  default_constants();

  // These are already defaulted to these buttons, but you can change the left/right curve buttons here!
  // chassis.opcontrol_curve_buttons_left_set(pros::E_CONTROLLER_DIGITAL_LEFT, pros::E_CONTROLLER_DIGITAL_RIGHT);  // If using tank, only the left side is used.
  // chassis.opcontrol_curve_buttons_right_set(pros::E_CONTROLLER_DIGITAL_Y, pros::E_CONTROLLER_DIGITAL_A);

  // Autonomous Selector using LLEMU blue_six_ring() blue_FREEZE_IVE_SEEN_THESE_PATHS_BEFORE()
  ez::as::auton_selector.autons_add({
      Auton("SKILLS :)\n\nSKILLS", skills),
      Auton("Ring rush SAWP Red!!! :)\n\nRED - Ring Rush SAWP", ringrush_SAWP),
      Auton("Blue ring rush :)\n\nBlue - ring rush", blueRingRush),
      Auton("RED_FREEZE!!! :)\n\nRED - FREEZE IVE SEEN THESE PATHS BEFORE", red_FREEZE_IVE_SEEN_THESE_PATHS_BEFORE),
      Auton("BLUE_FREEZE!!! :)\n\nBLUE - FREEZE IVE SEEN THESE PATHS BEFORE", blue_FREEZE_IVE_SEEN_THESE_PATHS_BEFORE),
      Auton("RED SAWP :)\n\nRED - Solo's autononomous win point.", red_sawp),
      Auton("BLUE SAWP :)\n\nBLUE - Solo's autononomous win point.", blue_sawp),
      Auton("RED SIX RING :)\n\nRED - Gets six ring(hopefully)", red_six_ring),
      Auton("BLUE SIX RING :)\n\nBLUE - Gets six ring(hopefully)", blue_six_ring),
      Auton("RED MOGO disrupt\n\nRED - disrupts the thirds mogo and scores 3 -save until elims-", red_mogo_disrupt),
      Auton("BLUE MOGO disrupt\n\nBLUE - disrupts the thirds mogo and scores 3 -save until elims-", blue_mogo_disrupt),
  });

  // Initialize chassis and auton selector
  chassis.initialize();
  ez::as::initialize();
  master.rumble(".");
  chassis.drive_imu_reset();                  // Reset gyro position to 0
}

/**
 * Runs while the robot is in the disabled state of Field Management System or
 * the VEX Competition Switch, following either autonomous or opcontrol. When
 * the robot is enabled, this task will exit.
 */
void disabled() {
  // . . .
}

/**
 * Runs after initialize(), and before autonomous when connected to the Field
 * Management System or the VEX Competition Switch. This is intended for
 * competition-specific initialization routines, such as an autonomous selector
 * on the LCD.
 *
 * This task will exit when the robot is enabled and autonomous or opcontrol
 * starts.
 */
void competition_initialize() {
  // . . .
}

/**
 * Runs the user autonomous code. This function will be started in its own task
 * with the default priority and stack size whenever the robot is enabled via
 * the Field Management System or the VEX Competition Switch in the autonomous
 * mode. Alternatively, this function may be called in initialize or opcontrol
 * for non-competition testing purposes.
 *
 * If the robot is disabled or communications is lost, the autonomous task
 * will be stopped. Re-enabling the robot will restart the task, not re-start it
 * from where it left off.
 */
void autonomous() 
{
  chassis.pid_targets_reset();                // Resets PID targets to 0
  
  chassis.drive_sensor_reset();               // Reset drive sensors to 0
  chassis.drive_brake_set(MOTOR_BRAKE_HOLD);  // Set motors to hold.  This helps autonomous consistency

  ez::as::auton_selector.selected_auton_call();  // Calls selected auton from autonomous selector
}

void opcontrol() 
{
  pros::Task intakeThread(intakeControl);

  armMotor.set_brake_mode_all(MOTOR_BRAKE_HOLD);//dont touch or ill touch you
  armControl->setMaxVelocity(200);//tuneable
  bool clampState = false;
  bool sweeperState = false;
  // This is preference to what you like to drive on
  pros::motor_brake_mode_e_t driver_preference_brake = pros::E_MOTOR_BRAKE_COAST;

  chassis.drive_brake_set(driver_preference_brake);

  while (true) {
    // chassis.opcontrol_tank();  // Tank control
    chassis.opcontrol_arcade_standard(ez::SPLIT);   // Standard split arcade
    // chassis.opcontrol_arcade_standard(ez::SINGLE);  // Standard single arcade
    // chassis.opcontrol_arcade_flipped(ez::SPLIT);    // Flipped split arcade
    // chassis.opcontrol_arcade_flipped(ez::SINGLE);   // Flipped single arcade

    // . . .
    // Put more user control code here!
    // . . .
    
    if (master.get_digital_new_press(DIGITAL_Y))
    {
      clampCylinder.set_value(!clampState); 
      clampState = !clampState;
    }

    if (master.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_RIGHT))
    {
      sweeperCylinder.set_value(!sweeperState);
      sweeperState = !sweeperState;
    } 

    if (master.get_digital_new_press(DIGITAL_L1) && positionIndex != 3){
        positionIndex++;
        armControl->setTarget(heights[positionIndex]);//raising arm 1 state
    }

    if (master.get_digital_new_press(DIGITAL_L2) && positionIndex != 0)
    {
      positionIndex--;
      armControl->setTarget(heights[positionIndex]);//dropping arm 1 state
    }
  }

    pros::delay(ez::util::DELAY_TIME);  // This is used for timer calculations!  Keep this ez::util::DELAY_TIME
  
  }
