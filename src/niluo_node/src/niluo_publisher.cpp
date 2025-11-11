#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/string.hpp"
#include <chrono>
#include <memory>

using namespace std::chrono_literals;

class NiluoNode : public rclcpp::Node {
public:
    NiluoNode() : Node("niluo_node"), count_(0) {
        RCLCPP_INFO(this->get_logger(),"Hello!I'm niluo");
        publisher_=this->create_publisher<std_msgs::msg::String>("mingyue",10);
        subscriber_=this->create_subscription<std_msgs::msg::String>("makeboat",10,std::bind(&NiluoNode::makeboat_callback,this,std::placeholders::_1));
        timer_=this->create_wall_timer(5000ms,std::bind(&NiluoNode::timer_callback,this));
    }
private:
    void timer_callback() {
        auto message=std_msgs::msg::String();
        count_++;
        message.data="I have No."+std::to_string(count_)+"Engineering Design";
        RCLCPP_INFO(this->get_logger(),"Send message: %s",message.data.c_str());
        publisher_->publish(message);
    }

    void makeboat_callback(const std_msgs::msg::String::SharedPtr msg) {
        RCLCPP_INFO(this->get_logger(),"Receive the student's message: %s,I'm study hard!",msg->data.c_str());
    }

    rclcpp::TimerBase::SharedPtr timer_;
    rclcpp::Publisher<std_msgs::msg::String>::SharedPtr publisher_;
    rclcpp::Subscription<std_msgs::msg::String>::SharedPtr subscriber_;
    int count_;
};

int main(int argc,char **argv) {
    rclcpp::init(argc,argv);
    auto node=std::make_shared<NiluoNode>();
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}
