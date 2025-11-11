#include "rclcpp/rclcpp.hpp"
#include "lr_interfaces/srv/car_service.hpp"
#include <chrono>
#include <cstdlib>
#include <memory>
#include <iostream>
#include <string>

using namespace std::chrono_literals;

class CarClientNode : public rclcpp::Node {
public:
    CarClientNode(int target_height) : Node("car_client"), target_height_(target_height) {
        client_ = this->create_client<lr_interfaces::srv::CarService>("car_climb_service");
        
        while(!client_->wait_for_service(1s)) {
            if(!rclcpp::ok) {
                RCLCPP_ERROR(this->get_logger(),"Exit When Waiting for Service");
                return;
            }
            RCLCPP_INFO(this->get_logger(),"Waiting for the Service Start...");
        }

        send_request();
    }
private:
    void send_request() {
        auto request = std::make_shared<lr_interfaces::srv::CarService::Request>();
        request->target_height = target_height_;
        
        RCLCPP_INFO(this->get_logger(),"Send Request:Target Height: %d m",target_height_);

        auto future = client_->async_send_request(request);

        if(rclcpp::spin_until_future_complete(this->get_node_base_interface(),future) == rclcpp::FutureReturnCode::SUCCESS) {
            auto response = future.get();
            RCLCPP_INFO(this->get_logger(),"Response: Current Height: %d m,Time: %d d",response->current_height,response->days);
        } else RCLCPP_ERROR(this->get_logger(),"Response Failed!");
    }

    rclcpp::Client<lr_interfaces::srv::CarService>::SharedPtr client_;
    int target_height_;
};

int main(int argc,char **argv) {
    rclcpp::init(argc,argv);

    if(argc != 2) {
        std:: printf("Example: ros2 run morihuoshan car_client TARGET_HEIGHT\n");
        return 1;
    }

    try {
        int target_height = std::stoi(argv[1]);
        std::cout<<"Set the Target as: "<<target_height<<" m";
        auto node = std::make_shared<CarClientNode>(target_height);
    } catch (const std::exception& e) {
        std::cout<<"error"<<e.what()<<std::endl;
        return 1;
    }

    //auto node = std:: make_shared<CarClientNode>();
    rclcpp::shutdown();
    return 0;
}