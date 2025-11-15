#include "example_action_rclcpp/robot.h"
#include "rclcpp/rclcpp.hpp"
#include "rclcpp_action/rclcpp_action.hpp"
#include "robot_control_interfaces/action/move_robot.hpp"

class ActionRobot01 : public rclcpp::Node {
public:
    using MoveRobot = robot_control_interfaces::action::MoveRobot;
    using GoalHandleMoveRobot = rclcpp_action::ServerGoalHandle<MoveRobot>;

    explicit ActionRobot01(std::string name) :Node(name) {
        RCLCPP_INFO(this->get_logger(),"The node %s has been launched.",name.c_str());//提示节点开启
        
        using namespace std::placeholders;//使用占位符

        this->action_server_ = rclcpp_action::create_server<MoveRobot>(this,"move_robot",
            std::bind(&ActionRobot01::handle_goal,this,_1,_2),
            std::bind(&ActionRobot01::handle_cancel,this,_1),
            std::bind(&ActionRobot01::handle_accepted,this,_1));
        //创建一个动作监听者，自身接受来自"move_robot"的客户端请求，creat_server则是注册了对于这些请求的回调模式
    }

private:
    Robot robot;
    rclcpp_action::Server<MoveRobot>::SharedPtr action_server_; //动作服务器指针（共享智能指针类型，）
    //GoalRsponse是一个枚举类型，表示对于收到的目标请求的响应（拒绝1，接受并运行2，接受并挂起3）
    rclcpp_action::GoalResponse handle_goal(const rclcpp_action::GoalUUID& uuid,std::shared_ptr<const MoveRobot::Goal> goal) {
        RCLCPP_INFO(this->get_logger(),"Receive goal request with distance %f",goal->distance);
        (void)uuid;//避免编译器警告未使用变量
        if(fabs(goal->distance > 100)) {//fabs是取绝对值函数
            RCLCPP_WARN(this->get_logger(),"The target is too far!");
            return rclcpp_action::GoalResponse::REJECT;
        }
        RCLCPP_INFO(this->get_logger(),"Distance: %f ,Let's go!",goal->distance);
        return rclcpp_action::GoalResponse::ACCEPT_AND_EXECUTE;
        //在接受请求之后，ros2会在内部创建一个GoalHandle追踪goal的生命周期，后续会通过它来发送反馈或终极目标
    }

    rclcpp_action::CancelResponse handle_cancel(const std::shared_ptr<GoalHandleMoveRobot> goal_handle) {
        RCLCPP_INFO(this->get_logger(),"Received request to cancel goal");
        (void)goal_handle;//避免编译器警告未使用变量
        robot.stop_move();//停止动作
        return rclcpp_action::CancelResponse::ACCEPT;
    }//在接受请求后，机器人会立即停止
    
    void execute_move(const std::shared_ptr<GoalHandleMoveRobot> goal_handle) {
        const auto goal = goal_handle->get_goal();//取出目标
        RCLCPP_INFO(this->get_logger(),"开始执行移动 %f ...",goal->distance);

        auto result = std::make_shared<MoveRobot::Result>();//创建结果指针
        rclcpp::Rate rate = rclcpp::Rate(2); //设置反馈频率为2Hz
        robot.set_goal(goal->distance); //设置目标距离
        while(rclcpp::ok() && !robot.close_goal()) { //过程中需要反复检查是否已经关闭目标或已经到达目标
            //rclcpp::ok()用于检查ROS2系统是否仍在运行
            robot.move_step();
            auto feedback = std::make_shared<MoveRobot::Feedback>();
            feedback->pose = robot.get_current_pose();
            feedback->status = robot.get_status();
            goal_handle->publish_feedback(feedback); //发布反馈

            if(goal_handle->is_canceling()) { //检测中途是否出现强制停止机器人工作的情况
                result->pose = robot.get_current_pose(); //当前到达的位置就是result的位置
                goal_handle->canceled(result); //设置目标为取消状态（*
                RCLCPP_INFO(this->get_logger(),"Goal Canceled");
                return;
            }
            RCLCPP_INFO(this->get_logger(),"Publish Feedback");
            rate.sleep(); //按照设定的频率休眠
        }

        result->pose = robot.get_current_pose(); //对结果进行赋值
        goal_handle->succeed(result); //设置目标为成功状态
        RCLCPP_INFO(this->get_logger(),"Goal Succeeded");
    }

    void handle_accepted(const std::shared_ptr<GoalHandleMoveRobot> goal_handle) {
        using std::placeholders::_1; //使用占位符
        //这个回调函数会在一个新的线程中执行，以避免阻塞动作服务器
        std::thread{std::bind(&ActionRobot01::execute_move,this,_1),goal_handle}.detach();//启动新线程执行机器人移动任务
    }
};

int main(int argc,char **argv) {
    rclcpp::init(argc,argv);
    auto action_server = std::make_shared<ActionRobot01>("action_robot_01");
    rclcpp::spin(action_server);
    rclcpp::shutdown();
    return 0;
}