#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/string.hpp"

class Student_Node : public rclcpp :: Node {
    public:
        // 构造函数,有一个参数为节点名称
        Student_Node (std :: string name) : Node (name) {
            name = "waya";
            RCLCPP_INFO (this -> get_logger (), "大家好，我是 %s", name.c_str ());
            // 创建发布者
            publisher_ = this -> create_publisher <std_msgs :: msg :: String> ("makeboat", 10);
            // 创建定时器，3000ms 为周期，定时发布
            timer_ = this -> create_wall_timer (
                std :: chrono :: milliseconds (5000),
                std :: bind (&Student_Node :: timer_callback, this)
            );

            subscribe_ = this -> create_subscription <std_msgs :: msg :: String> (
                "mingyue", 
                10, 
                std :: bind (&Student_Node :: receive_callback, this, std :: placeholders :: _1)
            );            
        }


    private: // empty
        void timer_callback () {
            // 创建消息
            std_msgs :: msg :: String message;
            message.data = "这已经是这周出的第 " + std :: to_string (rand () % 10 + 6) + " 次金光了！";
            // 日志打印
            RCLCPP_INFO (this -> get_logger (), "%s", message.data.c_str ());
            // 发布消息
            publisher_ -> publish (message);
        }
        // 声名定时器指针
        rclcpp :: TimerBase :: SharedPtr timer_;
        // 声明话题发布者指针
        rclcpp :: Publisher <std_msgs :: msg :: String> :: SharedPtr publisher_;

        // 收到话题数据的回调函数
        void receive_callback (const std_msgs :: msg :: String :: SharedPtr msg) {
            
        }
        rclcpp :: Subscription <std_msgs :: msg :: String> :: SharedPtr subscribe_;
};

int main (int argc, char **argv) {
    rclcpp :: init (argc, argv);
    auto node = std :: make_shared <Student_Node> ("Student_Node");
    rclcpp :: spin (node);
    rclcpp :: shutdown ();
    return 0;
}