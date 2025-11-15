#include "example_action_rclcpp/robot.h"

float Robot::move_step() {
    int direct = move_distance_/fabs(move_distance_);
    float step=3.0;
    current_pose_+=step;
    std::cout<<"移动了："<<step<<"当前位置："<<current_pose_<<"\n";
    return current_pose_;
}

bool Robot::set_goal(float distance) {
    move_distance_=distance;
    target_pose_+=move_distance_;

    if(close_goal()) {
        status_ = MoveRobot::Feedback::STATUS_STOP;
        return false;
    }
    status_=MoveRobot::Feedback::STATUS_MOVEING;
    return true;
}

float Robot::get_current_pose() {return current_pose_;}
int Robot::get_status() {return status_;}
bool Robot::close_goal() {return target_pose_ <= current_pose_;}
void Robot::stop_move() {status_=MoveRobot::Feedback::STATUS_STOP;}