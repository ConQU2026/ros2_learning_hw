#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/string.hpp"

class Niluo_Node : public rclcpp :: Node {
    public:
        // 构造函数,有一个参数为节点名称
        Niluo_Node (std :: string name) : Node (name) {
            name = "niluo";
            RCLCPP_INFO (this -> get_logger (), "大家好，我是 %s", name.c_str ());
            // 创建发布者
            publisher_ = this -> create_publisher <std_msgs :: msg :: String> ("mingyue", 10);
            // 创建定时器，5000ms 为周期，定时发布
            timer_ = this -> create_wall_timer (
                std :: chrono :: milliseconds (5000),
                std :: bind (&Niluo_Node :: timer_callback, this)
            );

            subscribe_ = this -> create_subscription <std_msgs :: msg :: String> (
                "makeboat", 
                10, 
                std :: bind (&Niluo_Node :: receive_callback, this, std :: placeholders :: _1)
            );            
        }


    private: // empty
        void timer_callback () {
            // 创建消息
            static int Tot = 1;
            std_msgs :: msg :: String message;
            message.data = "我上了第 " + std :: to_string (Tot++) + " 节工程设计";
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
            RCLCPP_INFO (this -> get_logger (), "waya 说 '%s' 这家伙叽里呱啦说什么呢！", msg -> data.c_str ());
        }
        rclcpp :: Subscription <std_msgs :: msg :: String> :: SharedPtr subscribe_;
};

int main (int argc, char **argv) {
    rclcpp :: init (argc, argv);
    auto node = std :: make_shared <Niluo_Node> ("Niluo_Node");
    rclcpp :: spin (node);
    rclcpp :: shutdown ();
    return 0;
}