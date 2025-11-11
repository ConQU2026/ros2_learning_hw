#include "rclcpp/rclcpp.hpp"
#include "lr_interfaces/srv/car_service.hpp"
#include <memory>

using namespace std::chrono_literals;

class CarServiceNode : public rclcpp::Node {
public:
    CarServiceNode() : Node("car_service"),current_height_(0),days_(0) {
        service_ = this->create_service<lr_interfaces::srv::CarService>("car_climb_service",std::bind(&CarServiceNode::handle_service,this,std::placeholders::_1,std::placeholders::_2));
        RCLCPP_INFO(this->get_logger(),"The car is ready! Current Height: %d m",current_height_);
    }
private:
    void handle_service(const std::shared_ptr<lr_interfaces::srv::CarService::Request> request,std::shared_ptr<lr_interfaces::srv::CarService::Response> response) {
        int target_height = request->target_height;
        RCLCPP_INFO(this->get_logger(),"Receive Target Height: %d m",target_height);

        days_ = 0;
        current_height_ = 0;

        while(current_height_ < target_height) {
            current_height_+=3;
            ++days_;
            RCLCPP_INFO(this->get_logger(),"Day %d ,Climb to %d m",days_,current_height_);
            rclcpp::sleep_for(500ms);
        }

        response->current_height = current_height_;
        response->days = days_;

        RCLCPP_INFO(this->get_logger(),"Get the Target Height! It takes %d d.",days_);
    }
    rclcpp::Service<lr_interfaces::srv::CarService>::SharedPtr service_;
    
    int current_height_;
    int days_;
};

int main(int argc,char **argv) {
    rclcpp::init(argc,argv);
    auto node= std::make_shared<CarServiceNode>();
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}