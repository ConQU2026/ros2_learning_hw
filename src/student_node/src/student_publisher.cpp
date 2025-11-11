#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/string.hpp"
#include<chrono>
#include<memory>

using namespace std::chrono_literals;

class StudentNode : public rclcpp::Node {
public:
    StudentNode() : Node("student_node") {
        RCLCPP_INFO(this->get_logger(),"Hello,I'm waya");
        publisher_=this->create_publisher<std_msgs::msg::String>("makeboat",10);
        subscriber_=this->create_subscription<std_msgs::msg::String>("mingyue",10,std::bind(&StudentNode::mingyue_callback,this,std::placeholders::_1));
        timer_=this->create_wall_timer(10000ms,std::bind(&StudentNode::timer_callback,this));
    }
private:
    void timer_callback() {
        auto message=std_msgs::msg::String();
        message.data="boat.data - I'm following niluo.";
        RCLCPP_INFO(this->get_logger(),"Send message: %s",message.data.c_str());
    }

    void mingyue_callback(const std_msgs::msg::String::SharedPtr msg) {
        RCLCPP_INFO(this->get_logger(),"Receive message from niluo: %s ,I'll learning him!",msg->data.c_str());
    }

    rclcpp::TimerBase::SharedPtr timer_;
    rclcpp::Publisher<std_msgs::msg::String>::SharedPtr publisher_;
    rclcpp::Subscription<std_msgs::msg::String>::SharedPtr subscriber_;
};

int main(int argc,char **argv) {
    rclcpp::init(argc,argv);
    auto node=std::make_shared<StudentNode>();
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}