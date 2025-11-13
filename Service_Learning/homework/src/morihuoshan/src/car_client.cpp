#include "rclcpp/rclcpp.hpp"
#include "lr_interfaces/srv/climb.hpp"
#include <chrono>
#include <cstdlib>
#include <memory>
#include <iostream>

using namespace std :: chrono_literals;

class CarClient : public rclcpp :: Node {
    public:
        CarClient () : Node ("car_client") {
            client_ = this -> create_client <lr_interfaces :: srv :: Climb> ("car_climb");
            while (!client_ -> wait_for_service (1s)) {
                if (!rclcpp :: ok ()) {
                    RCLCPP_INFO (this -> get_logger (), "Client interrupted while waiting for service. Exiting.");
                    return ;
                }
                RCLCPP_INFO (this -> get_logger (), "Service not available, waiting again...");
            }
            RCLCPP_INFO (this -> get_logger (), "Service is available.");
        }

        void send_request (double target_height) {
            auto request = std :: make_shared <lr_interfaces :: srv :: Climb :: Request> ();
            request -> target_height = target_height;

            RCLCPP_INFO (this -> get_logger (), "Sending request with target height: %.2f", target_height);
            auto result_future = client_ -> async_send_request (
                request,
                std :: bind (&CarClient :: response_callback, this, std :: placeholders :: _1)
            );

            rclcpp :: spin_until_future_complete (this -> get_node_base_interface (), result_future);
        }
    
    private:
        void response_callback (rclcpp :: Client <lr_interfaces :: srv :: Climb> :: SharedFuture future) {
            auto response = future.get ();
            RCLCPP_INFO (
                this -> get_logger (), 
                "Climb completed in %d days, reached height %.2f",
                response -> days, response -> current_height
            );
        }

        rclcpp :: Client <lr_interfaces :: srv :: Climb> :: SharedPtr client_;
};

int main (int argc, char ** argv) {
    rclcpp :: init (argc, argv);
    auto node = std :: make_shared <CarClient> ();

    double target_height;
    std :: cout << "Enter target height for the car to climb: ";
    std :: cin >> target_height;

    node -> send_request (target_height);

    rclcpp :: shutdown ();
    return 0;
}