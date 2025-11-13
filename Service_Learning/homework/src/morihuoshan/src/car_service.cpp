#include "rclcpp/rclcpp.hpp"
#include "lr_interfaces/srv/climb.hpp"
#include <memory>

using std :: placeholders :: _1;
using std :: placeholders :: _2;

class CarService : public rclcpp :: Node {
    public: 
        CarService () : Node ("car_service") {
            service_ = this -> create_service <lr_interfaces :: srv :: Climb> (
                "car_climb", 
                std :: bind (&CarService :: handle_service, this, _1, _2)
            );
            RCLCPP_INFO (this -> get_logger (), "Car Climb Service is ready.");
        }

    private:
        rclcpp :: Service <lr_interfaces :: srv :: Climb> :: SharedPtr service_;

        void handle_service (
            const std :: shared_ptr <lr_interfaces :: srv :: Climb :: Request> request,
            std :: shared_ptr <lr_interfaces :: srv :: Climb :: Response> response) {
            double target = request -> target_height, current = 0;
            int days = 0;
            RCLCPP_INFO (this -> get_logger (), "Received target height: %.2f", target);

            while (current < target) {
                current += 3.0;
                days++;
                RCLCPP_INFO (this -> get_logger (), "Day %d: Current height = %.2f", days, current);
            }

            response -> days = days;
            response -> current_height = current;
            RCLCPP_INFO (this -> get_logger (), "Climb completed in %d days to reach height %.2f", days, current);
        }
};

int main (int argc, char ** argv) {
    rclcpp :: init (argc, argv);
    rclcpp :: spin (std :: make_shared <CarService> ());
    rclcpp :: shutdown ();
    return 0;
}