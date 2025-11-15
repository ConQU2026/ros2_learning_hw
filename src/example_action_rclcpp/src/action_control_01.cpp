#include "rclcpp/rclcpp.hpp"
#include "rclcpp_action/rclcpp_action.hpp"
#include "robot_control_interfaces/action/move_robot.hpp"

class ActionControl01 : public rclcpp::Node {
public:
    using MoveRobot = robot_control_interfaces::action::MoveRobot;
    using GoalHandleMoveRobot = rclcpp_action::ClientGoalHandle<MoveRobot>;

    explicit ActionControl01(std::string name,float fDistance): Node(name) {
        RCLCPP_INFO(this->get_logger(),"The Node %s has been launched.",name.c_str());
        this->client_ptr_ = rclcpp_action::create_client<MoveRobot>(this,"move_robot");//创建动作客户端，连接到"move_robot"动作服务器
        this->timer_ = this->create_wall_timer(std::chrono::milliseconds(500),std::bind(&ActionControl01::send_goal,this));//创建一个定时器，每500ms调用一次send_goal函数
        //为什么叫wall_timer，因为是墙上挂着的（
        fDistance_=fDistance;
    }

    void send_goal() {
        using namespace std::placeholders;

        this->timer_->cancel();//只发送一次目标后取消定时器

        if(!this->client_ptr_->wait_for_action_server(std::chrono::seconds(10))) { //等待动作服务器可用，最长等待10秒（注意单位）
            RCLCPP_ERROR(this->get_logger(),"Action server not available after waiting");//如果等待超时，输出错误信息并关闭节点
            rclcpp::shutdown();
            return;
        }
        auto goal_msg = MoveRobot::Goal();//创建一个目标消息对象
        goal_msg.distance = fDistance_;//设置目标

        RCLCPP_INFO(this->get_logger(),"Sending goal");

        auto send_goal_options = rclcpp_action::Client<MoveRobot>::SendGoalOptions(); //创建发送目标选项对象
        send_goal_options.goal_response_callback = std::bind(&ActionControl01::goal_response_callback,this,_1);//绑定目标响应回调函数
        send_goal_options.feedback_callback = std::bind(&ActionControl01::feedback_callback,this,_1,_2);//绑定反馈回调函数
        send_goal_options.result_callback = std::bind(&ActionControl01::result_callback,this,_1);//绑定结果回调函数
        this->client_ptr_->async_send_goal(goal_msg,send_goal_options);//异步发送目标
    }
private:
    rclcpp_action::Client<MoveRobot>::SharedPtr client_ptr_;//动作客户端指针
    rclcpp::TimerBase::SharedPtr timer_;//定时器指针
    float fDistance_;

    void goal_response_callback(GoalHandleMoveRobot::SharedPtr goal_handle) { //目标响应回调函数
        if (!goal_handle) RCLCPP_ERROR(this->get_logger(), "Goal was rejected by server"); //如果目标句柄为空，表示目标被服务器拒绝
        else RCLCPP_INFO(this->get_logger(),"Goal accepted by server, waiting for result"); //否则表示目标被接受
    }

    void feedback_callback(GoalHandleMoveRobot::SharedPtr,const std::shared_ptr<const MoveRobot::Feedback> feedback) {//反馈回调函数
        RCLCPP_INFO(this->get_logger(), "Feedback current pose:%f", feedback->pose);//输出当前反馈的位姿信息
    }

    void result_callback(const GoalHandleMoveRobot::WrappedResult& result) {//结果回调函数
        switch (result.code) {
            case rclcpp_action::ResultCode::SUCCEEDED: break;
            case rclcpp_action::ResultCode::ABORTED: RCLCPP_ERROR(this->get_logger(), "Goal was aborted"); return;
            case rclcpp_action::ResultCode::CANCELED: RCLCPP_ERROR(this->get_logger(), "Goal was canceled"); return;
            //Succeeded表示目标成功完成，Aborted表示目标被中止，Canceled表示目标被取消
            //abort和cancel都是失败的情况
            //其中，aborted表示由于某种错误或异常情况导致目标无法完成，而canceled则表示目标在执行过程中被主动取消
            default: RCLCPP_ERROR(this->get_logger(), "Unknown result code"); return;
        }

        RCLCPP_INFO(this->get_logger(), "Result received: %f", result.result->pose);
        // rclcpp::shutdown();
    }
};

int main(int argc,char **argv) {
    rclcpp::init(argc,argv);
    float fDistance=argv[1]?atof(argv[1]):50.0;//默认目标距离50.0，可以通过命令行传值
    auto action_client = std::make_shared<ActionControl01>("action_robot_cpp",fDistance);
    rclcpp::spin(action_client);
    rclcpp::shutdown();
    return 0;
}