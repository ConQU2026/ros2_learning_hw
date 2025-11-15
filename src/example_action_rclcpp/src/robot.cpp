#include "example_action_rclcpp/robot.h"

float Robot::move_step() {//每次调用移动一步（这里使用的是最简单的靠近减速的运行方式，当机器人距离目标接近的时候，速度会慢下来）
    int direct = move_distance_/fabs(move_distance_);
    float step=direct*fabs(target_pose_-current_pose_)*0.1;
    current_pose_+=step;
    std::cout<<"移动了："<<step<<"当前位置："<<current_pose_<<"\n";
    return current_pose_;
}

bool Robot::set_goal(float distance) {//设置目标距离
    move_distance_=distance;
    target_pose_+=move_distance_;

    if(close_goal()) { //如果已经到达目标位置，自然就不需要设置目标了
        status_ = MoveRobot::Feedback::STATUS_STOP;
        return false;
    }
    status_=MoveRobot::Feedback::STATUS_MOVEING; //设置状态为正在移动
    return true;
}

float Robot::get_current_pose() {return current_pose_;}
int Robot::get_status() {return status_;}
bool Robot::close_goal() {return fabs(target_pose_-current_pose_)<0.01;}
void Robot::stop_move() {status_=MoveRobot::Feedback::STATUS_STOP;}