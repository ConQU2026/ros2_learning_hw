#include "example_action_rclcpp/robot.h"
#include "rclcpp/rclcpp.hpp"
#include "rclcpp_action/rclcpp_action.hpp"
#include "robot_control_interfaces/action/move_robot.hpp"

class ActionRobot01 : public rclcpp::Node {
public:
    using MoveRobot = robot_control_interfaces::action::MoveRobot;
    using GoalHandleMoveRobot = rclcpp_action::ServerGoalHandle<MoveRobot>;

    explicit ActionRobot01(std::string name) :Node(name) {
        RCLCPP_INFO(this->get_logger(),"The node %s has been launched.",name.c_str());
        
        using namespace std::placeholders;

        this->action_server_ = rclcpp_action::create_server<MoveRobot>(this,"move_robot",
            std::bind(&ActionRobot01::handle_goal,this,_1,_2),
            std::bind(&ActionRobot01::handle_cancel,this,_1),
            std::bind(&ActionRobot01::handle_accepted,this,_1));
    }

private:
    Robot robot; int days_=0;
    rclcpp_action::Server<MoveRobot>::SharedPtr action_server_;
    rclcpp_action::GoalResponse handle_goal(const rclcpp_action::GoalUUID& uuid,std::shared_ptr<const MoveRobot::Goal> goal) {
        RCLCPP_INFO(this->get_logger(),"Receive goal request with distance %f",goal->distance);
        (void)uuid;
        if(fabs(goal->distance > 100)) {
            RCLCPP_WARN(this->get_logger(),"The target is too far!");
            return rclcpp_action::GoalResponse::REJECT;
        }
        RCLCPP_INFO(this->get_logger(),"Height: %f ,Let's go!",goal->distance);
        return rclcpp_action::GoalResponse::ACCEPT_AND_EXECUTE;
    }

    rclcpp_action::CancelResponse handle_cancel(const std::shared_ptr<GoalHandleMoveRobot> goal_handle) {
        RCLCPP_INFO(this->get_logger(),"Received request to cancel goal");
        (void)goal_handle;
        robot.stop_move();
        return rclcpp_action::CancelResponse::ACCEPT;
    }
    
    void execute_move(const std::shared_ptr<GoalHandleMoveRobot> goal_handle) {
        const auto goal = goal_handle->get_goal();
        RCLCPP_INFO(this->get_logger(),"GO GO GO ~ 开始爬山 %f ...",goal->distance);

        auto result = std::make_shared<MoveRobot::Result>();
        rclcpp::Rate rate = rclcpp::Rate(2);
        robot.set_goal(goal->distance);
        while(rclcpp::ok() && !robot.close_goal()) {
            ++days_;
            robot.move_step();
            auto feedback = std::make_shared<MoveRobot::Feedback>();
            feedback->pose = robot.get_current_pose();
            feedback->status = robot.get_status();
            goal_handle->publish_feedback(feedback);

            if(goal_handle->is_canceling()) {
                result->pose = robot.get_current_pose();
                goal_handle->canceled(result);
                RCLCPP_INFO(this->get_logger(),"Goal Canceled");
                return;
            }
            RCLCPP_INFO(this->get_logger(),"Publish Feedback");
            rate.sleep();
        }

        result->pose = robot.get_current_pose();
        goal_handle->succeed(result);
        RCLCPP_INFO(this->get_logger(),"Goal Succeeded,it takes %d days.",days_);
    }

    void handle_accepted(const std::shared_ptr<GoalHandleMoveRobot> goal_handle) {
        using std::placeholders::_1;
        std::thread{std::bind(&ActionRobot01::execute_move,this,_1),goal_handle}.detach();
    }
};

int main(int argc,char **argv) {
    rclcpp::init(argc,argv);
    auto action_server = std::make_shared<ActionRobot01>("action_robot_01");
    rclcpp::spin(action_server);
    rclcpp::shutdown();
    return 0;
}